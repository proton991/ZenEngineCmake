#ifndef ZENENGINE_INITIALIZER_HPP
#define ZENENGINE_INITIALIZER_HPP
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
}  // namespace zen::vkh
#endif  //ZENENGINE_INITIALIZER_HPP
