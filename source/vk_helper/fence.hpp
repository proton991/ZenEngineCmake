#ifndef ZENENGINE_FENCE_HPP
#define ZENENGINE_FENCE_HPP
#include <string>
#include "base.hpp"

namespace zen::vkh {
class Device;
class Fence {
public:
  ZEN_NO_COPY(Fence)
  Fence(const Device& device, std::string name, bool signaled_state);
  Fence(Fence&& other) noexcept;
  ~Fence();
  Fence& operator=(Fence&&) = delete;

  void block(std::uint64_t timeout_limit = std::numeric_limits<std::uint64_t>::max()) const;

  void reset() const;

  VkResult status() const;

private:
  const Device& m_device;
  VkFence m_fence{nullptr};
  std::string m_name;
};
}  // namespace zen::vkh
#endif  //ZENENGINE_FENCE_HPP
