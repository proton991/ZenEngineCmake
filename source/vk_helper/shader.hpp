#ifndef ZENENGINE_SHADER_HPP
#define ZENENGINE_SHADER_HPP
#include <array>
#include <string>
#include <unordered_map>
#include <vector>
#include "base.hpp"

namespace zen::vkh {
class Device;

enum class ShaderType : decltype(1) {
  Vertex   = VK_SHADER_STAGE_VERTEX_BIT,
  Fragment = VK_SHADER_STAGE_FRAGMENT_BIT
};

struct ShaderStage {
  ShaderStage() = default;
  ShaderStage(std::vector<char> code_, VkShaderModule shader_module_, VkShaderStageFlagBits flag_);
  std::vector<char> code;
  VkShaderModule shader_module{nullptr};
  VkShaderStageFlagBits flag{};
};

struct ReflectedBinding {
  uint32_t set;
  uint32_t binding;
  VkDescriptorType type;
};

class ShaderProgram {
public:
  ShaderProgram(const Device& device, std::string name);
  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram(ShaderProgram&&) noexcept;

  ~ShaderProgram();

  ShaderProgram& operator=(const ShaderProgram&) = delete;
  ShaderProgram& operator=(ShaderProgram&&)      = delete;

  ShaderProgram& add_stage(const std::string& file_name, ShaderType type);
  ShaderProgram& fill_stage_cis(std::vector<VkPipelineShaderStageCreateInfo>& pipeline_stages);
  ShaderProgram& reflect_layout();

  auto get_name() const { return m_name; }
  auto get_pipeline_layout() const { return m_pipeline_layout; }

  void show_ds_layout_info() const;
private:
  const Device& m_device;
  std::string m_name;
  std::vector<ShaderStage> m_stages;
  std::array<VkDescriptorSetLayout, 4> m_ds_layouts;
  std::unordered_map<std::string, ReflectedBinding> m_reflected_bindings;
  VkPipelineLayout m_pipeline_layout{nullptr};
};
}  // namespace zen::vkh
#endif  //ZENENGINE_SHADER_HPP
