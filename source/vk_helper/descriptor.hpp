#ifndef ZENENGINE_DESCRIPTOR_HPP
#define ZENENGINE_DESCRIPTOR_HPP
#include <unordered_map>
#include <vector>
#include "base.hpp"

namespace zen::vkh {
class Device;
class DescriptorAllocator {
  friend class DescriptorBuilder;

public:
  DescriptorAllocator(const Device& device) : m_device(device) {}
  ~DescriptorAllocator();

  struct PoolSizes {
    std::vector<std::pair<VkDescriptorType, float>> type2weight = {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.f},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.f},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.f},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f}};
  };

  void reset_pools();
  bool allocate(VkDescriptorSet* set, VkDescriptorSetLayout layout);

private:
  void cleanup();
  VkDescriptorPool create_pool(const DescriptorAllocator::PoolSizes& poolSizes, int count,
                               VkDescriptorPoolCreateFlags flags);
  VkDescriptorPool grab_pool();

  const Device& m_device;
  VkDescriptorPool m_current_pool{VK_NULL_HANDLE};
  PoolSizes m_pool_sizes;
  std::vector<VkDescriptorPool> m_used_pools;
  std::vector<VkDescriptorPool> m_free_pools;
};

class DescriptorLayoutCache {
public:
  DescriptorLayoutCache(const Device& device) : m_device(device) {}
  ~DescriptorLayoutCache();

  VkDescriptorSetLayout get_or_create(VkDescriptorSetLayoutCreateInfo* info);

  struct DescriptorLayoutInfo {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bool operator==(const DescriptorLayoutInfo& other) const;
    size_t hash() const;
  };

private:
  void cleanup();

  const Device& m_device;
  struct DescriptorLayoutHash {
    size_t operator()(const DescriptorLayoutInfo& k) const { return k.hash(); }
  };
  std::unordered_map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash>
      m_layout_cache;
};

class DescriptorBuilder {
public:
  static DescriptorBuilder begin(DescriptorLayoutCache* layoutCache,
                                 DescriptorAllocator* allocator);

  DescriptorBuilder& bind_buffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo,
                                 VkDescriptorType type, VkShaderStageFlags stageFlags);

  DescriptorBuilder& bind_image(uint32_t binding, VkDescriptorImageInfo* imageInfo,
                                VkDescriptorType type, VkShaderStageFlags stageFlags);

  bool build(VkDescriptorSet& set, VkDescriptorSetLayout& layout);

  bool build(VkDescriptorSet& set);

private:
  std::vector<VkWriteDescriptorSet> m_writes;
  std::vector<VkDescriptorSetLayoutBinding> m_bindings;

  DescriptorLayoutCache* m_cache;
  DescriptorAllocator* m_allocator;
};
}  // namespace zen::vkh
#endif  //ZENENGINE_DESCRIPTOR_HPP
