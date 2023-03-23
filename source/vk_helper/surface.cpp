#include "surface.hpp"
#include "logging.hpp"

namespace zen::vkh {
Surface::Surface(VkInstance instance, GLFWwindow* glfw_window) {
  VK_ASSERT(glfw_window);
  m_instance = instance;
  VK_CHECK(glfwCreateWindowSurface(m_instance, glfw_window, nullptr, &m_surface), "create surface");
}

Surface::~Surface() {
  vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
}

std::vector<const char*> Surface::get_device_exts() {
  return {"VK_KHR_swapchain"};
}

std::vector<const char*> Surface::get_instance_exts() {
  uint32_t count;
  const char** ext = glfwGetRequiredInstanceExtensions(&count);
  return {ext, ext + count};
}
}  // namespace zen::vkh