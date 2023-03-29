#include "buffer.hpp"
#include "debug.hpp"
#include "device.hpp"
#include "logging.hpp"

namespace zen::vkh {
Buffer::Buffer(const Device& device, std::string name, VkDeviceSize size,
               VkBufferUsageFlags buffer_usage, VmaAllocationCreateFlags vma_flags)
    : m_device(device), m_name(std::move(name)) {
  VkBufferCreateInfo buffer_ci{};
  buffer_ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_ci.size  = size;
  buffer_ci.usage = buffer_usage;

  VmaAllocationCreateInfo vma_alloc_info{};
  vma_alloc_info.flags = vma_flags;
  vma_alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
  VK_CHECK(vmaCreateBuffer(m_device.get_allocator(), &buffer_ci, &vma_alloc_info, &m_buffer,
                           &m_allocation, &m_alloc_info),
           "vmaCreateBuffer");
  DebugUtil::get().set_obj_name(m_buffer, m_name.c_str());
  vmaSetAllocationName(m_device.get_allocator(), m_allocation, m_name.c_str());
}

Buffer::~Buffer() {
  vmaDestroyBuffer(m_device.get_allocator(), m_buffer, m_allocation);
}

void Buffer::update(void* src_data, size_t data_size, uint32_t offset) {
  char* dst;
  vmaMapMemory(m_device.get_allocator(), m_allocation, (void**)&dst);
  dst += offset;
  std::memcpy(dst, src_data, data_size);
  vmaUnmapMemory(m_device.get_allocator(), m_allocation);
}

Buffer::Buffer(Buffer&& other) noexcept : m_device(other.m_device) {
  m_name       = std::move(other.m_name);
  m_buffer     = std::exchange(other.m_buffer, nullptr);
  m_allocation = std::exchange(other.m_allocation, nullptr);
  m_alloc_info = other.m_alloc_info;
}

StorageBuffer::StorageBuffer(const Device& device, const std::string& name,
                             const VkDeviceSize& buffer_size)
    : Buffer(device, name, buffer_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
             VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT) {}

StorageBuffer::StorageBuffer(StorageBuffer&& other) noexcept : Buffer(std::move(other)) {}

UniformBuffer::UniformBuffer(const Device& device, const std::string& name,
                             const VkDeviceSize& buffer_size)
    : Buffer(device, name, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
             VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT) {}

UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept : Buffer(std::move(other)) {}

StagingBuffer::StagingBuffer(const Device& device, const std::string& name,
                             const VkDeviceSize& buffer_size)
    : Buffer(device, name, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
             VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT) {}

StagingBuffer::StagingBuffer(StagingBuffer&& other) noexcept : Buffer(std::move(other)) {}

}  // namespace zen::vkh