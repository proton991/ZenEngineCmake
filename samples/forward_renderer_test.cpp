#include <logging.hpp>
#include <zen.hpp>
#include <renderer/forward_renderer.hpp>
#include <systems/window_system.hpp>
#include <vk_helper/context.hpp>
#include <vk_helper/surface.hpp>
using namespace zen;

int main(int argc, char** argv) {
  Ref<sys::Window> window = CreateRef<Window>(sys::Window::default_config());
  auto instance_exts = vkh::Surface::get_instance_exts();
  Ref<vkh::Context> context = CreateRef<Context>();
  if (!context->create_instance(instance_exts.data(), instance_exts.size())) {
    logger::error("Failed to create instance");
    return 1;
  }
  ForwardRenderer forward_renderer{context, window};
  forward_renderer.init();
  return 0;
}