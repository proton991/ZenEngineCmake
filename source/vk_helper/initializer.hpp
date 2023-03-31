#ifndef ZENENGINE_INITIALIZER_HPP
#define ZENENGINE_INITIALIZER_HPP
#include <vector>
#include "base.hpp"
namespace zen::vkh {
// attachment description
VkAttachmentDescription present_color_att_description(
    VkFormat format_, VkSampleCountFlagBits samples_ = VK_SAMPLE_COUNT_1_BIT);
VkAttachmentDescription depth_att_description(VkFormat format_);
VkAttachmentDescription transient_color_att_description(
    VkFormat format_, VkSampleCountFlagBits samples_ = VK_SAMPLE_COUNT_1_BIT);
// attachment reference
VkAttachmentReference color_att_ref(uint32_t binding);
VkAttachmentReference input_att_ref(uint32_t binding);
VkAttachmentReference depth_stencil_att_ref(uint32_t binding);

// Pipeline state infos
VkPipelineVertexInputStateCreateInfo empty_vertex_input_state_ci();
VkPipelineInputAssemblyStateCreateInfo input_assembly_state_ci(
    VkPrimitiveTopology topology  = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    bool enable_primitive_restart = VK_FALSE);
VkPipelineViewportStateCreateInfo viewport_state_ci(uint32_t viewport_count,
                                                    const VkViewport* ptr_viewports,
                                                    uint32_t scissor_count,
                                                    const VkRect2D* ptr_scissors);
VkPipelineRasterizationStateCreateInfo rasterization_state_ci(
    VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL,
    VkCullModeFlags cull_mode  = VK_CULL_MODE_BACK_BIT,
    VkFrontFace front_face     = VK_FRONT_FACE_COUNTER_CLOCKWISE);
VkPipelineMultisampleStateCreateInfo multisample_state_ci(VkSampleCountFlagBits samples);
VkPipelineDepthStencilStateCreateInfo depth_stencil_state_ci(bool enable_depth_test,
                                                             bool enable_depth_write,
                                                             VkCompareOp depth_compare_op);
VkPipelineColorBlendAttachmentState color_blend_att(bool enable_blend);
VkPipelineColorBlendStateCreateInfo color_blend_state_ci(
    uint32_t att_count, const VkPipelineColorBlendAttachmentState* ptr_color_blend_att);

VkPipelineDynamicStateCreateInfo dynamic_state_ci(
    const std::vector<VkDynamicState>& dynamic_states);

}  // namespace zen::vkh
#endif  //ZENENGINE_INITIALIZER_HPP
