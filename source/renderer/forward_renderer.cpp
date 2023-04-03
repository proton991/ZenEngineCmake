#include "forward_renderer.hpp"
#include "logging.hpp"


using namespace zen::vkh;
namespace zen {
ForwardRenderer::ForwardRenderer(const Ref<Context>& context, const Ref<Window>& window) {
  m_context = context;
  m_window  = window;
}

void ForwardRenderer::init() {
  m_surface = CreateScope<Surface>(m_context->get_instance(), m_window->handle());
  auto device_exts = vkh::Surface::get_device_exts();
  if (!m_context->create_device(m_surface->handle(), device_exts.data(), device_exts.size(), nullptr)) {
    logger::error("Failed to create device");
    return;
  }
  m_device = CreateScope<Device>();
  m_device->set_context(*m_context);
  m_swapchain = CreateScope<Swapchain>(*m_device, m_surface->handle(), m_window->get_width(),
                                       m_window->get_height(), true);
}
}  // namespace zen