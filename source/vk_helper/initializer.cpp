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
VkAttachmentReference depth_att_ref(uint32_t binding) {
  return {binding, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
}
}  // namespace zen::vkh