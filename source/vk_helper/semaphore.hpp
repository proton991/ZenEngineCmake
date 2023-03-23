#ifndef ZENENGINE_SEMAPHORE_HPP
#define ZENENGINE_SEMAPHORE_HPP
#include <string>
#include "base.hpp"

namespace zen::vkh {
class Device;
/// RAII wrapper class for VkSemaphore
class Semaphore {
public:
  ZEN_NON_COPIABLE(Semaphore)

  Semaphore(const Device& device, const std::string& name);
  ~Semaphore();
  [[nodiscard]] VkSemaphore semaphore() const { return m_semaphore; }

private:
  const Device& m_device;
  VkSemaphore m_semaphore{VK_NULL_HANDLE};
  std::string m_name;
};
}  // namespace zen::vkh
#endif  //ZENENGINE_SEMAPHORE_HPP
