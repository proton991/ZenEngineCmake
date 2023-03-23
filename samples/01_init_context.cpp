#include <vk_helper/context.hpp>
#include <vk_helper/device.hpp>
#include <logging.hpp>

using namespace zen;
int main() {
  vkh::Context context;
  if (!context.create_instance(nullptr, 0)) {
    logger::error("Failed to create instance");
    return 1;
  }
  if (!context.create_device(VK_NULL_HANDLE, nullptr, 0, nullptr)) {
    logger::error("Failed to create device");
    return 1;
  }
  VK_ASSERT(volkGetLoadedDevice());
  vkh::Device device;
  device.set_context(context);
  return 0;
}