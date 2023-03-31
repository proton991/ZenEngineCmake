#include "pipeline.hpp"
#include "debug.hpp"
#include "device.hpp"
#include "initializer.hpp"

namespace zen::vkh {
PipelineBuilder& PipelineBuilder::reset() {
  m_name.clear();
  m_shader_stages.clear();
  m_vertex_input_description = {};
  m_vertex_input_state       = {};

  m_input_assembly_state = {};

  m_tessellation_state = {};

  m_viewport       = {};
  m_scissor        = {};
  m_viewport_state = {};

  m_rasterization_state = {};

  m_multisample_state = {};

  m_depth_stencil_state = {};

  m_color_blend_att   = {};
  m_color_blend_state = {};

  m_dynamic_state_enables.clear();
  m_dynamic_state = {};

  return* this;
}

PipelineBuilder& PipelineBuilder::set_name(std::string name) {
  m_name = std::move(name);
  return *this;
}

PipelineBuilder& PipelineBuilder::set_shader_stages(
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages) {
  m_shader_stages = std::move(shader_stages);
  return *this;
}

PipelineBuilder& PipelineBuilder::set_vertex_specification(
    VertexInputDescription vertex_input_description, VkPrimitiveTopology topology) {
  m_vertex_input_description = std::move(vertex_input_description);
  m_vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  m_vertex_input_state.pVertexAttributeDescriptions = m_vertex_input_description.attributes.data();
  m_vertex_input_state.vertexAttributeDescriptionCount =
      m_vertex_input_description.attributes.size();
  m_vertex_input_state.pVertexBindingDescriptions    = m_vertex_input_description.bindings.data();
  m_vertex_input_state.vertexBindingDescriptionCount = m_vertex_input_description.bindings.size();

  m_input_assembly_state = input_assembly_state_ci(topology, false);
  return *this;
}

PipelineBuilder& PipelineBuilder::set_view_port(VkExtent2D extent, uint32_t viewport_count,
                                                uint32_t scissor_count) {
  m_viewport = {
      .x        = 0.0f,
      .y        = 0.0f,
      .width    = (float)extent.width,
      .height   = (float)extent.height,
      .minDepth = 0.0f,
      .maxDepth = 1.0f  //
  };
  m_scissor = {
      .offset = {0, 0},
      .extent = extent  //
  };
  m_viewport_state = viewport_state_ci(viewport_count, &m_viewport, scissor_count, &m_scissor);
  return *this;
}

PipelineBuilder& PipelineBuilder::set_rasterization(VkPolygonMode polygon_mode,
                                                    VkCullModeFlags cull_mode,
                                                    VkFrontFace front_face) {
  m_rasterization_state = rasterization_state_ci(polygon_mode, cull_mode, front_face);
  return *this;
}

PipelineBuilder& PipelineBuilder::set_multisample(VkSampleCountFlagBits samples) {
  m_multisample_state = multisample_state_ci(samples);
  return *this;
}

PipelineBuilder& PipelineBuilder::set_depth_stencil(bool enable_depth_test, bool enable_depth_write,
                                                    VkCompareOp depth_compare_op) {

  m_depth_stencil_state =
      depth_stencil_state_ci(enable_depth_test, enable_depth_write, depth_compare_op);
  return *this;
}

PipelineBuilder& PipelineBuilder::enable_blend(bool flag) {
  m_color_blend_att   = color_blend_att(flag);
  m_color_blend_state = color_blend_state_ci(1, &m_color_blend_att);
  return *this;
}

VkPipeline PipelineBuilder::build(VkPipelineLayout layout, VkRenderPass render_pass,
                                  uint32_t subpass_index) {
  m_dynamic_state_enables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  m_dynamic_state         = dynamic_state_ci(m_dynamic_state_enables);

  VkGraphicsPipelineCreateInfo pipeline_ci{};
  // basic infos
  pipeline_ci.sType              = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_ci.layout             = layout;
  pipeline_ci.renderPass         = render_pass;
  pipeline_ci.flags              = 0;
  pipeline_ci.basePipelineIndex  = -1;
  pipeline_ci.basePipelineHandle = VK_NULL_HANDLE;
  // state infos
  pipeline_ci.pVertexInputState   = &m_vertex_input_state;
  pipeline_ci.pInputAssemblyState = &m_input_assembly_state;
  pipeline_ci.pRasterizationState = &m_rasterization_state;
  pipeline_ci.pColorBlendState    = &m_color_blend_state;
  pipeline_ci.pMultisampleState   = &m_multisample_state;
  pipeline_ci.pViewportState      = &m_viewport_state;
  pipeline_ci.pDepthStencilState  = &m_depth_stencil_state;
  pipeline_ci.pDynamicState       = &m_dynamic_state;
  pipeline_ci.stageCount          = static_cast<uint32_t>(m_shader_stages.size());
  pipeline_ci.pStages             = m_shader_stages.data();
  pipeline_ci.subpass             = subpass_index;

  VkPipeline pipeline;
  VK_CHECK(
      vkCreateGraphicsPipelines(m_device.handle(), nullptr, 1, &pipeline_ci, nullptr, &pipeline),
      "vkCreateGraphicsPipelines");
  DebugUtil::get().set_obj_name(pipeline, m_name.data());
  return pipeline;
}
}  // namespace zen::vkh