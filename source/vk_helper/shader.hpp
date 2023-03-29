#ifndef ZENENGINE_SHADER_HPP
#define ZENENGINE_SHADER_HPP
#include <string>
#include <vector>
#include "base.hpp"

namespace zen::vkh {
class Device;

class Shader {
public:
  /// @brief Construct a shader module from a SPIR-V file.
  /// This constructor loads the file content and just calls the other constructor.
  /// @param device The const reference to a device RAII wrapper instance.
  /// @param type The shader type.
  /// @param name The internal debug marker name of the VkShaderModule.
  /// @param file_name The name of the SPIR-V shader file to load.
  /// @param entry_point The name of the entry point, "main" by default.
  Shader(const Device& device, VkShaderStageFlagBits type, std::string name,
         const std::string& file_name, std::string entry_point = "main");

  Shader(const Shader&) = delete;
  Shader(Shader&&) noexcept;

  ~Shader();

  Shader& operator=(const Shader&) = delete;
  Shader& operator=(Shader&&)      = delete;

  VkPipelineShaderStageCreateInfo get_pipeline_shader_stage_ci() const;

  const std::string& name() const { return m_name; }

  const std::string& entry_point() const { return m_entry_point; }

  VkShaderStageFlagBits type() const { return m_type; }

  VkShaderModule module() const { return m_shader_module; }

private:
  const Device& m_device;
  std::string m_name;
  std::string m_entry_point;
  VkShaderStageFlagBits m_type;
  VkShaderModule m_shader_module{nullptr};
};
}  // namespace zen::vkh
#endif  //ZENENGINE_SHADER_HPP
