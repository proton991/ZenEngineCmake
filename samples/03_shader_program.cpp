#include <logging.hpp>
#include <systems/window_system.hpp>
#include <vk_helper/context.hpp>
#include <vk_helper/device.hpp>
#include <vk_helper/shader.hpp>
#include <vk_helper/surface.hpp>

using namespace zen;
int main() {
  sys::Window window{sys::Window::default_config()};

  auto instance_exts = vkh::Surface::get_instance_exts();
  vkh::Context context;
  if (!context.create_instance(instance_exts.data(), instance_exts.size())) {
    logger::error("Failed to create instance");
    return 1;
  }
  vkh::Surface surface(context.get_instance(), window.handle());

  auto device_exts = vkh::Surface::get_device_exts();
  if (!context.create_device(surface.handle(), device_exts.data(), device_exts.size(), nullptr)) {
    logger::error("Failed to create device");
    return 1;
  }

  vkh::Device device;
  device.set_context(context);
  std::vector<VkPipelineShaderStageCreateInfo> infos;
  vkh::ShaderProgram test_shader(device, "test_shader");
  test_shader.add_stage("tri_mesh_ssbo_textured.vert.spv", vkh::ShaderType::Vertex)
      .add_stage("textured_lit.frag.spv", vkh::ShaderType::Fragment)
      .fill_stage_cis(infos)
      .reflect_layout();
  test_shader.show_ds_layout_info();
  VkPipelineLayout pipeline_layout = test_shader.get_pipeline_layout();
  VK_ASSERT(pipeline_layout != nullptr);
  vkDestroyPipelineLayout(device.handle(), pipeline_layout, nullptr);
  return 0;
}