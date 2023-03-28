#ifndef ZENENGINE_SURFACE_HPP
#define ZENENGINE_SURFACE_HPP
#include "base.hpp"
#include <GLFW/glfw3.h>
#include <vector>

namespace zen::vkh {
class Surface {
public:
  ZEN_NO_COPY_MOVE(Surface)
  Surface(VkInstance instance, GLFWwindow* glfw_window);
  ~Surface();
  static std::vector<const char*> get_device_exts();
  static std::vector<const char*> get_instance_exts();

  VkSurfaceKHR handle() const { return m_surface; }
private:
  VkSurfaceKHR m_surface;
  VkInstance m_instance;

};
}
#endif  //ZENENGINE_SURFACE_HPP
