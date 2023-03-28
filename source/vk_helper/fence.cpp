#include "fence.hpp"
#include "device.hpp"

namespace zen::vkh {
Fence::Fence(const Device& device, std::string name, bool signaled_state)
    : m_device(device), m_name(std::move(name)) {
  VkFenceCreateInfo fence_ci = {
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .flags = static_cast<VkFenceCreateFlags>(signaled_state ? VK_FENCE_CREATE_SIGNALED_BIT : 0),
  };
  m_device.create_fence(fence_ci, &m_fence, m_name);
}

Fence::Fence(Fence&& other) noexcept : m_device(other.m_device) {
  m_fence = std::exchange(other.m_fence, nullptr);
  m_name  = std::move(other.m_name);
}

Fence::~Fence() {
  m_device.destroy_fence(m_fence);
}

void Fence::block(std::uint64_t timeout_limit) const {
  vkWaitForFences(m_device.handle(), 1, &m_fence, VK_TRUE, timeout_limit);
}

void Fence::reset() const {
  vkResetFences(m_device.handle(), 1, &m_fence);
}

VkResult Fence::status() const {
  return vkGetFenceStatus(m_device.handle(), m_fence);
}
}  // namespace zen::vkh