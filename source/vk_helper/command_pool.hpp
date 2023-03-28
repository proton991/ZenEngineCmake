#ifndef ZENENGINE_COMMAND_POOL_HPP
#define ZENENGINE_COMMAND_POOL_HPP
#include <memory>
#include <string>
#include <vector>
#include "base.hpp"

namespace zen::vkh {
class Device;
class CommandBuffer;

class CommandPool {
public:
  ZEN_NO_COPY_MOVE(CommandPool)
  CommandPool(const Device& device, std::string name);
  ~CommandPool();

  VkCommandPool handle() { return m_cmd_pool; }
  const CommandBuffer& request_primary_command_buffer(const std::string& name);

private:
  const Device& m_device;
  std::string m_name;
  VkCommandPool m_cmd_pool{nullptr};
  std::vector<std::unique_ptr<CommandBuffer>> m_primary_cmd_buffers;
};
}  // namespace zen::vkh
#endif  //ZENENGINE_COMMAND_POOL_HPP
