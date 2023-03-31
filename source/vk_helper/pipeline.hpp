#ifndef ZENENGINE_PIPELINE_HPP
#define ZENENGINE_PIPELINE_HPP
#include <string>
#include <vector>
#include "base.hpp"

namespace zen::vkh {
// TODO: move this to other header files
struct VertexInputDescription {
  std::vector<VkVertexInputBindingDescription> bindings;
  std::vector<VkVertexInputAttributeDescription> attributes;
  VkPipelineVertexInputStateCreateFlags flags{0};
};
class Device;

class PipelineBuilder {
public:
  explicit PipelineBuilder(const Device& device) : m_device(device) {}
  PipelineBuilder& reset();
  PipelineBuilder& set_name(std::string name);
  PipelineBuilder& set_shader_stages(std::vector<VkPipelineShaderStageCreateInfo> shader_stages);
  PipelineBuilder& set_vertex_specification(VertexInputDescription vertex_input_description,
                                            VkPrimitiveTopology topology);
  PipelineBuilder& set_view_port(VkExtent2D extent, uint32_t viewport_count = 1,
                                 uint32_t scissor_count = 1);
  PipelineBuilder& set_rasterization(VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL,
                                     VkCullModeFlags cull_mode  = VK_CULL_MODE_BACK_BIT,
                                     VkFrontFace front_face     = VK_FRONT_FACE_COUNTER_CLOCKWISE);
  PipelineBuilder& set_multisample(VkSampleCountFlagBits samples);
  PipelineBuilder& set_depth_stencil(bool enable_depth_test, bool enable_depth_write,
                                     VkCompareOp depth_compare_op);
  PipelineBuilder& enable_blend(bool flag);

  VkPipeline build(VkPipelineLayout layout, VkRenderPass render_pass, uint32_t subpass_index);

private:
  const Device& m_device;
  std::string m_name;
  std::vector<VkPipelineShaderStageCreateInfo> m_shader_stages;

  VertexInputDescription m_vertex_input_description;
  VkPipelineVertexInputStateCreateInfo m_vertex_input_state{};

  VkPipelineInputAssemblyStateCreateInfo m_input_assembly_state{};

  [[maybe_unused]] VkPipelineTessellationStateCreateInfo m_tessellation_state{};

  VkViewport m_viewport{};
  VkRect2D m_scissor{};
  VkPipelineViewportStateCreateInfo m_viewport_state{};

  VkPipelineRasterizationStateCreateInfo m_rasterization_state{};

  VkPipelineMultisampleStateCreateInfo m_multisample_state{};

  VkPipelineDepthStencilStateCreateInfo m_depth_stencil_state{};

  VkPipelineColorBlendAttachmentState m_color_blend_att{};
  VkPipelineColorBlendStateCreateInfo m_color_blend_state{};

  std::vector<VkDynamicState> m_dynamic_state_enables;
  VkPipelineDynamicStateCreateInfo m_dynamic_state{};
};
}  // namespace zen::vkh
#endif  //ZENENGINE_PIPELINE_HPP
