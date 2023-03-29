#ifndef ZENENGINE_BUFFER_HPP
#define ZENENGINE_BUFFER_HPP
#include <string>
#include "base.hpp"

namespace zen::vkh {
class Device;

class Buffer {
public:
  ZEN_NO_COPY(Buffer)
  Buffer(const Device& device, std::string name, VkDeviceSize size, VkBufferUsageFlags buffer_usage,
         VmaAllocationCreateFlags vma_flags);

  Buffer(Buffer&& other) noexcept;
  virtual ~Buffer();

  void update(void* src_data, size_t data_size, uint32_t offset = 0);

  VkBuffer handle() const { return m_buffer; }
  VmaAllocationInfo allocation_info() const { return m_alloc_info; }

protected:
  const Device& m_device;
  std::string m_name;
  VkBuffer m_buffer{nullptr};
  VkDeviceSize m_size{0};
  VmaAllocation m_allocation{nullptr};
  VmaAllocationInfo m_alloc_info{};
};

class StorageBuffer : public Buffer {
public:
  StorageBuffer(const Device& device, const std::string& name, const VkDeviceSize& buffer_size);
  StorageBuffer(StorageBuffer&& other) noexcept;
};

class UniformBuffer : public Buffer {
public:
  UniformBuffer(const Device& device, const std::string& name, const VkDeviceSize& buffer_size);
  UniformBuffer(UniformBuffer&& other) noexcept;
};

class StagingBuffer : public Buffer {
  StagingBuffer(const Device& device, const std::string& name, const VkDeviceSize& buffer_size);
  StagingBuffer(StagingBuffer&& other) noexcept;
};
}  // namespace zen::vkh
#endif  //ZENENGINE_BUFFER_HPP
