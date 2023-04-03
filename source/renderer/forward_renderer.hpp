#ifndef ZENENGINE_FORWARD_RENDERER_HPP
#define ZENENGINE_FORWARD_RENDERER_HPP
#include "systems/window_system.hpp"
#include "vk_helper/context.hpp"
#include "vk_helper/device.hpp"
#include "vk_helper/surface.hpp"
#include "vk_helper/swapchain.hpp"
#include "zen.hpp"
using namespace zen::vkh;
using namespace zen::sys;

namespace zen {
class ForwardRenderer {
public:
  ForwardRenderer(const Ref<Context>& context, const Ref<Window>& window);
  void init();

private:
  // created by engine, shared by all renderers
  Ref<Window> m_window;
  Ref<Context> m_context;

  Scope<Surface> m_surface;
  Scope<Device> m_device;
  Scope<Swapchain> m_swapchain;
};
}  // namespace zen
#endif  //ZENENGINE_FORWARD_RENDERER_HPP
