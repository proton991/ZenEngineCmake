#include "initializer.hpp"

namespace zen::vkh {
VkAttachmentDescription present_color_att_description(VkFormat format_,
                                                      VkSampleCountFlagBits samples_) {
  VkAttachmentDescription att{};
  att.format         = format_;
  att.samples        = samples_;
  att.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
  att.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
  att.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  att.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
  att.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  return att;
}

VkAttachmentDescription depth_att_description(VkFormat format_) {
  VkAttachmentDescription att{};
  att.format         = format_;
  att.samples        = VK_SAMPLE_COUNT_1_BIT;
  att.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
  att.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  att.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  att.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
  att.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  return att;
}

VkAttachmentDescription transient_color_att_description(VkFormat format_,
                                                        VkSampleCountFlagBits samples_) {
  VkAttachmentDescription att{};
  att.format         = format_;
  att.samples        = VK_SAMPLE_COUNT_1_BIT;
  att.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
  att.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  att.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  att.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
  att.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  return att;
}

VkAttachmentReference color_att_ref(uint32_t binding) {
  return {binding, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
}

VkAttachmentReference input_att_ref(uint32_t binding) {
  return {binding, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
}

VkAttachmentReference depth_stencil_att_ref(uint32_t binding) {
  return {binding, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
}

VkPipelineVertexInputStateCreateInfo empty_vertex_input_state_ci() {
  VkPipelineVertexInputStateCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  return info;
}

VkPipelineInputAssemblyStateCreateInfo input_assembly_state_ci(VkPrimitiveTopology topology,
                                                               bool enable_primitive_restart) {
  VkPipelineInputAssemblyStateCreateInfo info = {
      .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .flags                  = 0,
      .topology               = topology,
      .primitiveRestartEnable = enable_primitive_restart};
  return info;
}

VkPipelineViewportStateCreateInfo viewport_state_ci(uint32_t viewport_count,
                                                    const VkViewport* ptr_viewports,
                                                    uint32_t scissor_count,
                                                    const VkRect2D* ptr_scissors) {
  VkPipelineViewportStateCreateInfo info{};
  info.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  info.viewportCount = viewport_count;
  info.scissorCount  = scissor_count;
  info.pViewports    = ptr_viewports;
  info.pScissors     = ptr_scissors;
  info.flags         = 0;
  return info;
}

VkPipelineRasterizationStateCreateInfo rasterization_state_ci(VkPolygonMode polygon_mode,
                                                              VkCullModeFlags cull_mode,
                                                              VkFrontFace front_face) {
  VkPipelineRasterizationStateCreateInfo info{};
  info.sType            = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  info.polygonMode      = polygon_mode;
  info.cullMode         = cull_mode;
  info.frontFace        = front_face;
  info.depthClampEnable = VK_FALSE;
  info.lineWidth        = 1.0f;
  return info;
}

VkPipelineMultisampleStateCreateInfo multisample_state_ci(VkSampleCountFlagBits samples) {
  VkPipelineMultisampleStateCreateInfo info{};
  info.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  info.rasterizationSamples = samples;
  info.flags                = 0;
  return info;
}

VkPipelineDepthStencilStateCreateInfo depth_stencil_state_ci(bool enable_depth_test,
                                                             bool enable_depth_write,
                                                             VkCompareOp depth_compare_op) {
  VkPipelineDepthStencilStateCreateInfo info{};
  info.sType            = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  info.depthTestEnable  = enable_depth_test;
  info.depthWriteEnable = enable_depth_write;
  info.depthCompareOp   = depth_compare_op;
  info.back.compareOp   = VK_COMPARE_OP_ALWAYS;
  return info;
}

VkPipelineColorBlendAttachmentState color_blend_att(bool enable_blend) {
  VkPipelineColorBlendAttachmentState att{};
  att.blendEnable    = enable_blend;
  att.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                       VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  if (att.blendEnable) {
    att.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    att.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    att.colorBlendOp        = VK_BLEND_OP_ADD;
    att.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    att.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    att.alphaBlendOp        = VK_BLEND_OP_ADD;
  }
  return att;
}

VkPipelineColorBlendStateCreateInfo color_blend_state_ci(
    uint32_t att_count, const VkPipelineColorBlendAttachmentState* ptr_color_blend_att) {
  VkPipelineColorBlendStateCreateInfo info{};
  info.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  info.attachmentCount = att_count;
  info.pAttachments    = ptr_color_blend_att;
  return info;
}

VkPipelineDynamicStateCreateInfo dynamic_state_ci(
    const std::vector<VkDynamicState>& dynamic_states) {
  VkPipelineDynamicStateCreateInfo info{};
  info.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  info.pDynamicStates    = dynamic_states.data();
  info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
  info.flags             = 0;
  return info;
}
}  // namespace zen::vkh