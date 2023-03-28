#include "command_buffer.hpp"
#include "device.hpp"

namespace zen::vkh {
CommandBuffer::CommandBuffer(const Device& device, VkCommandPool cmd_pool,
                             VkCommandBufferLevel level, std::string name)
    : m_device(device), m_name(std::move(name)) {
  VkCommandBufferAllocateInfo info = {
      .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool        = cmd_pool,
      .level              = level,
      .commandBufferCount = 1,
  };
  m_device.allocate_command_buffer(info, &m_cmd_buffer, m_name);
  m_wait_fence = std::make_unique<Fence>(m_device, m_name, false);
}

CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept : m_device(other.m_device) {
  m_cmd_buffer = std::exchange(other.m_cmd_buffer, VK_NULL_HANDLE);
  m_name       = std::move(other.m_name);
  m_wait_fence = std::exchange(other.m_wait_fence, nullptr);
}

void CommandBuffer::begin(const VkCommandBufferUsageFlags flags) const {
  // primary command buffer
  VkCommandBufferBeginInfo info = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                   .flags = flags};
  vkBeginCommandBuffer(m_cmd_buffer, &info);
}

VkResult CommandBuffer::fence_status() const {
  return m_wait_fence->status();
}

void CommandBuffer::reset_fence() const {
  m_wait_fence->reset();
}
}  // namespace zen::vkh