#include "shader.hpp"
#include <utility>
#include "device.hpp"
#include "utils/file_util.hpp"

namespace zen::vkh {
Shader::Shader(const Device& device, VkShaderStageFlagBits type, std::string name,
               const std::string& file_name, std::string entry_point)
    : m_device(device),
      m_type(type),
      m_name(std::move(name)),
      m_entry_point(std::move(entry_point)) {
  auto code                     = util::read_file_binary_data(file_name);
  VkShaderModuleCreateInfo info = {
      .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = code.size(),
      .pCode    = reinterpret_cast<const std::uint32_t*>(code.data()),
  };
  m_device.create_shader_module(info, &m_shader_module, m_name);
}

Shader::Shader(Shader&& other) noexcept : m_device(other.m_device) {
  m_type          = other.m_type;
  m_name          = std::move(other.m_name);
  m_entry_point   = std::move(other.m_entry_point);
  m_shader_module = std::exchange(other.m_shader_module, nullptr);
}

Shader::~Shader() {
  m_device.destroy_shader_module(m_shader_module);
}

VkPipelineShaderStageCreateInfo Shader::get_pipeline_shader_stage_ci() const {
  VkPipelineShaderStageCreateInfo info = {
      .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage  = m_type,
      .module = m_shader_module,
      .pName  = m_entry_point.c_str()};
  return info;
}
}  // namespace zen::vkh