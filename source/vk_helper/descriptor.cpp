#include "descriptor.hpp"
#include <algorithm>
#include "device.hpp"

namespace zen::vkh {
/** DescriptorAllocator **/
VkDescriptorPool DescriptorAllocator::create_pool(const DescriptorAllocator::PoolSizes& poolSizes,
                                                  int count, VkDescriptorPoolCreateFlags flags) {
  std::vector<VkDescriptorPoolSize> sizes;
  sizes.reserve(poolSizes.type2weight.size());
  for (auto it : poolSizes.type2weight) {
    sizes.push_back({it.first, static_cast<uint32_t>(it.second * count)});
  }
  VkDescriptorPoolCreateInfo pool_info{};
  pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags         = flags;
  pool_info.maxSets       = count;
  pool_info.poolSizeCount = sizes.size();
  pool_info.pPoolSizes    = sizes.data();

  VkDescriptorPool descriptor_pool;
  vkCreateDescriptorPool(m_device.handle(), &pool_info, nullptr, &descriptor_pool);

  return descriptor_pool;
}

DescriptorAllocator::~DescriptorAllocator() {
  cleanup();
}

void DescriptorAllocator::reset_pools() {
  for (auto p : m_used_pools) {
    vkResetDescriptorPool(m_device.handle(), p, 0);
  }

  m_free_pools = m_used_pools;
  m_used_pools.clear();
  m_current_pool = VK_NULL_HANDLE;
}

bool DescriptorAllocator::allocate(VkDescriptorSet* set, VkDescriptorSetLayout layout) {
  if (m_current_pool == VK_NULL_HANDLE) {
    m_current_pool = grab_pool();
    m_used_pools.push_back(m_current_pool);
  }

  VkDescriptorSetAllocateInfo alloc_info{};
  alloc_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.pNext              = nullptr;
  alloc_info.pSetLayouts        = &layout;
  alloc_info.descriptorPool     = m_current_pool;
  alloc_info.descriptorSetCount = 1;

  VkResult alloc_result = vkAllocateDescriptorSets(m_device.handle(), &alloc_info, set);
  bool need_reallocate;
  switch (alloc_result) {
    case VK_SUCCESS:
      return true;
    case VK_ERROR_FRAGMENTED_POOL:
    case VK_ERROR_OUT_OF_POOL_MEMORY:
      need_reallocate = true;
      break;
    default:
      return false;
  }
  if (need_reallocate) {
    m_current_pool = grab_pool();
    m_used_pools.push_back(m_current_pool);
    // use the new pool for allocation
    alloc_info.descriptorPool = m_current_pool;

    alloc_result = vkAllocateDescriptorSets(m_device.handle(), &alloc_info, set);
    if (alloc_result == VK_SUCCESS) {
      return true;
    }
  }
  return false;
}

void DescriptorAllocator::cleanup() {
  for (auto p : m_free_pools) {
    vkDestroyDescriptorPool(m_device.handle(), p, nullptr);
  }
  for (auto p : m_used_pools) {
    vkDestroyDescriptorPool(m_device.handle(), p, nullptr);
  }
}

VkDescriptorPool DescriptorAllocator::grab_pool() {
  if (!m_free_pools.empty()) {
    VkDescriptorPool pool = m_free_pools.back();
    m_free_pools.pop_back();
    return pool;
  } else {
    return create_pool(m_pool_sizes, 1000, 0);
  }
}

/** DescriptorLayoutCache **/

void DescriptorLayoutCache::cleanup() {
  for (const auto& p : m_layout_cache) {
    vkDestroyDescriptorSetLayout(m_device.handle(), p.second, nullptr);
  }
}

DescriptorLayoutCache::~DescriptorLayoutCache() {
  cleanup();
}

VkDescriptorSetLayout DescriptorLayoutCache::get_or_create(VkDescriptorSetLayoutCreateInfo* info) {
  DescriptorLayoutInfo layout_info;
  layout_info.bindings.reserve(info->bindingCount);
  bool is_sorted   = true;
  int last_binding = -1;

  for (int i = 0; i < info->bindingCount; i++) {
    layout_info.bindings.push_back(info->pBindings[i]);
    if (info->pBindings[i].binding > last_binding) {
      last_binding = info->pBindings[i].binding;
    } else {
      is_sorted = false;
    }
  }

  if (!is_sorted) {
    std::sort(layout_info.bindings.begin(), layout_info.bindings.end(),
              [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b) {
                return a.binding < b.binding;
              });
  }

  auto it = m_layout_cache.find(layout_info);
  if (it != m_layout_cache.end()) {
    return (*it).second;
  } else {
    VkDescriptorSetLayout layout;
    vkCreateDescriptorSetLayout(m_device.handle(), info, nullptr, &layout);

    m_layout_cache[layout_info] = layout;
    return layout;
  }
}

bool DescriptorLayoutCache::DescriptorLayoutInfo::operator==(
    const DescriptorLayoutCache::DescriptorLayoutInfo& other) const {
  if (other.bindings.size() != bindings.size()) {
    return false;
  } else {
    //compare each of the bindings is the same. Bindings are sorted so they will match
    for (int i = 0; i < bindings.size(); i++) {
      if (other.bindings[i].binding != bindings[i].binding) {
        return false;
      }
      if (other.bindings[i].descriptorType != bindings[i].descriptorType) {
        return false;
      }
      if (other.bindings[i].descriptorCount != bindings[i].descriptorCount) {
        return false;
      }
      if (other.bindings[i].stageFlags != bindings[i].stageFlags) {
        return false;
      }
    }
    return true;
  }
}

size_t DescriptorLayoutCache::DescriptorLayoutInfo::hash() const {
  using std::hash;
  using std::size_t;

  size_t result = hash<size_t>()(bindings.size());

  for (const VkDescriptorSetLayoutBinding& b : bindings) {
    //pack the binding data into a single int64. Not fully correct but it's ok
    size_t binding_hash =
        b.binding | b.descriptorType << 8 | b.descriptorCount << 16 | b.stageFlags << 24;

    //shuffle the packed binding data and xor it with the main hash
    result ^= hash<size_t>()(binding_hash);
  }

  return result;
}

/** DescriptorBuilder **/
DescriptorBuilder DescriptorBuilder::begin(DescriptorLayoutCache* layoutCache,
                                           DescriptorAllocator* allocator) {
  DescriptorBuilder builder;
  builder.m_cache     = layoutCache;
  builder.m_allocator = allocator;

  return builder;
}

DescriptorBuilder& DescriptorBuilder::bind_buffer(uint32_t binding,
                                                  VkDescriptorBufferInfo* bufferInfo,
                                                  VkDescriptorType type,
                                                  VkShaderStageFlags stageFlags) {
  VkDescriptorSetLayoutBinding new_binding{};
  new_binding.descriptorCount    = 1;
  new_binding.descriptorType     = type;
  new_binding.pImmutableSamplers = nullptr;
  new_binding.stageFlags         = stageFlags;
  new_binding.binding            = binding;

  m_bindings.push_back(new_binding);

  VkWriteDescriptorSet new_write{};
  new_write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  new_write.pNext           = nullptr;
  new_write.descriptorCount = 1;
  new_write.descriptorType  = type;
  new_write.pBufferInfo     = bufferInfo;
  new_write.dstBinding      = binding;

  m_writes.push_back(new_write);

  return *this;
}

DescriptorBuilder& DescriptorBuilder::bind_image(uint32_t binding, VkDescriptorImageInfo* imageInfo,
                                                 VkDescriptorType type,
                                                 VkShaderStageFlags stageFlags) {
  VkDescriptorSetLayoutBinding new_binding{};
  new_binding.descriptorCount    = 1;
  new_binding.descriptorType     = type;
  new_binding.pImmutableSamplers = nullptr;
  new_binding.stageFlags         = stageFlags;
  new_binding.binding            = binding;

  m_bindings.push_back(new_binding);

  VkWriteDescriptorSet new_write{};
  new_write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  new_write.pNext           = nullptr;
  new_write.descriptorCount = 1;
  new_write.descriptorType  = type;
  new_write.pImageInfo      = imageInfo;
  new_write.dstBinding      = binding;

  m_writes.push_back(new_write);

  return *this;
}

bool DescriptorBuilder::build(VkDescriptorSet& set, VkDescriptorSetLayout& layout) {
  VkDescriptorSetLayoutCreateInfo layout_info{};
  layout_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.pNext        = nullptr;
  layout_info.pBindings    = m_bindings.data();
  layout_info.bindingCount = static_cast<uint32_t>(m_bindings.size());

  layout = m_cache->get_or_create(&layout_info);

  if (!m_allocator->allocate(&set, layout)) {
    return false;
  }

  for (VkWriteDescriptorSet& w : m_writes) {
    w.dstSet = set;
  }

  vkUpdateDescriptorSets(m_allocator->m_device.handle(), static_cast<uint32_t>(m_writes.size()),
                         m_writes.data(), 0, nullptr);

  return true;
}

bool DescriptorBuilder::build(VkDescriptorSet& set) {
  VkDescriptorSetLayout layout;
  return build(set, layout);
}
}  // namespace zen::vkh