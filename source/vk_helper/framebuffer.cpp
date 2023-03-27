#include "framebuffer.hpp"
#include <algorithm>
#include "debug.hpp"
#include "device.hpp"

namespace zen::vkh {
Framebuffer::Framebuffer(const Device& device, const FramebufferInfo& info) : m_device(device) {
  VkFramebufferCreateInfo framebuffer_ci = {
      .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .renderPass      = info.render_pass,
      .attachmentCount = static_cast<uint32_t>(info.image_views.size()),
      .pAttachments    = info.image_views.data(),
      .width           = info.extent.width,
      .height          = info.extent.height,
      .layers          = info.layer_count};
  VK_CHECK(vkCreateFramebuffer(m_device.get_device(), &framebuffer_ci, nullptr, &m_framebuffer),
           "vkCreateFramebuffer");
  DebugUtil::get().set_obj_name(m_framebuffer, info.name.data());
}

Framebuffer::~Framebuffer() {
  vkDestroyFramebuffer(m_device.get_device(), m_framebuffer, nullptr);
}

uint32_t Framebuffer::get_max_layers(const std::vector<ImageInfo>& image_infos) {
  auto max_element = std::max_element(image_infos.begin(), image_infos.end(),
                                      [](const ImageInfo& a, const ImageInfo& b) {
                                        return a.layer_count < b.layer_count;
                                      });

  return max_element->layer_count;
}
}  // namespace zen::vkh