#include "command_pool.hpp"
#include <memory>
#include <utility>
#include "command_buffer.hpp"
#include "device.hpp"
#include "logging.hpp"

namespace zen::vkh {
CommandPool::CommandPool(const Device& device, std::string name)
    : m_device(device), m_name(std::move(name)) {
  VkCommandPoolCreateInfo cmd_pool_ci = {.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                         .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
                                                  VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
                                         .queueFamilyIndex = device.graphics_queue_family_index()};
  VK_CHECK(vkCreateCommandPool(m_device.handle(), &cmd_pool_ci, nullptr, &m_cmd_pool),
           "vkCreateCommandPool");
}

CommandPool::~CommandPool() {
  vkDestroyCommandPool(m_device.handle(), m_cmd_pool, nullptr);
}

const CommandBuffer& CommandPool::request_primary_command_buffer(const std::string& name) {
  // Try to find a command buffer which is currently not used
  for (const auto& cmd_buf : m_primary_cmd_buffers) {
    if (cmd_buf->fence_status() == VK_SUCCESS) {
      // Reset the command buffer's fence to make it usable again
      cmd_buf->reset_fence();
      cmd_buf->begin();
      return *cmd_buf;
    }
  }

  // We need to create a new command buffer because no free one was found
  // Note that there is currently no method for shrinking m_cmd_bufs, but this should not be a problem
  m_primary_cmd_buffers.emplace_back(std::make_unique<CommandBuffer>(
      m_device, m_cmd_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, "command buffer"));

  spdlog::trace("Creating new command buffer #{}", m_primary_cmd_buffers.size());

  m_primary_cmd_buffers.back()->begin();
  return *m_primary_cmd_buffers.back();
}

}  // namespace zen::vkh