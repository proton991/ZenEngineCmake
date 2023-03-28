#include <vk_helper/context.hpp>
#include <vk_helper/device.hpp>
#include <vk_helper/surface.hpp>
#include <vk_helper/swapchain.hpp>
#include <vk_helper/semaphore.hpp>
#include <vk_helper/fence.hpp>
#include <systems/window_system.hpp>
#include <logging.hpp>

using namespace zen;
int main() {
  sys::Window window{sys::Window::default_config()};

  auto instance_exts = vkh::Surface::get_instance_exts();
  vkh::Context context;
  if (!context.create_instance(instance_exts.data(), instance_exts.size())) {
    logger::error("Failed to create instance");
    return 1;
  }
  vkh::Surface surface(context.get_instance(), window.get());

  auto device_exts = vkh::Surface::get_device_exts();
  if (!context.create_device(surface.handle(), device_exts.data(), device_exts.size(), nullptr)) {
    logger::error("Failed to create device");
    return 1;
  }

  vkh::Device device;
  device.set_context(context);
  vkh::Swapchain swapchain(device, surface.handle(), window.get_width(), window.get_height(), true);
  vkh::Semaphore semaphore(device, "test");
  vkh::Fence fence(device, "test", false);
  while (!window.should_close()) {
    window.update();
  }
  return 0;
}