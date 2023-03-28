#ifndef ZENENGINE_COMMAND_BUFFER_HPP
#define ZENENGINE_COMMAND_BUFFER_HPP
#include <memory>
#include <string>
#include "base.hpp"
#include "fence.hpp"

namespace zen::vkh {
class Device;

class CommandBuffer {
  friend class CommandPool;

public:
  ZEN_NO_COPY(CommandBuffer)
  CommandBuffer(const Device& device, VkCommandPool cmd_pool, VkCommandBufferLevel level,
                std::string name);

  CommandBuffer(CommandBuffer&&) noexcept;
  CommandBuffer& operator=(CommandBuffer&&) = delete;

  ~CommandBuffer() = default;

  void begin(VkCommandBufferUsageFlags flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT) const;

  VkResult fence_status() const;
  void reset_fence() const;

private:
  const Device& m_device;
  VkCommandBuffer m_cmd_buffer{nullptr};
  std::string m_name;
  std::unique_ptr<Fence> m_wait_fence;
};
}  // namespace zen::vkh
#endif  //ZENENGINE_COMMAND_BUFFER_HPP
