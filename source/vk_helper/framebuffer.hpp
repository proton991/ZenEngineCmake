#ifndef ZENENGINE_FRAMEBUFFER_HPP
#define ZENENGINE_FRAMEBUFFER_HPP
#include <vector>
#include "base.hpp"
#include "image.hpp"

namespace zen::vkh {
class Device;
struct FramebufferInfo {
  VkExtent2D extent{};
  VkRenderPass render_pass{nullptr};
  const std::vector<VkImageView>& image_views;
  uint32_t layer_count{1};
  std::string name;
};

class Framebuffer {
public:
  static uint32_t get_max_layers(const std::vector<ImageInfo>& image_infos);
  Framebuffer(const Device& device, const FramebufferInfo& info);
  ~Framebuffer();

private:
  const Device& m_device;
  VkFramebuffer m_framebuffer{nullptr};
};
}
#endif  //ZENENGINE_FRAMEBUFFER_HPP
