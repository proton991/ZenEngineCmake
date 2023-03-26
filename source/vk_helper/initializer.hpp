#ifndef ZENENGINE_INITIALIZER_HPP
#define ZENENGINE_INITIALIZER_HPP
#include "base.hpp"
namespace zen::vkh {
// color attachment description for presentation
VkAttachmentDescription present_color_att_description(
    VkFormat format_, VkSampleCountFlagBits samples_ = VK_SAMPLE_COUNT_1_BIT);
VkAttachmentDescription depth_att_description(VkFormat format_);
VkAttachmentDescription transient_color_att_description(
    VkFormat format_, VkSampleCountFlagBits samples_ = VK_SAMPLE_COUNT_1_BIT);

VkAttachmentReference color_att_ref(uint32_t binding);
VkAttachmentReference input_att_ref(uint32_t binding);
VkAttachmentReference depth_att_ref(uint32_t binding);
}  // namespace zen::vkh
#endif  //INITIALIZER_HPP
