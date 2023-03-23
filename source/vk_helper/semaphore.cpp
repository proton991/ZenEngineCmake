#include "semaphore.hpp"
#include "device.hpp"

namespace zen::vkh {
Semaphore::Semaphore(const Device& device, const std::string& name) : m_device(device) {
  VkSemaphoreCreateInfo create_info{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  m_device.create_semaphore(create_info, &m_semaphore, name);
}

Semaphore::~Semaphore() {
  m_device.destroy_semaphore(m_semaphore);
}
}  // namespace zen::vkh