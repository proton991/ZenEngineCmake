#ifndef ZENENGINE_RENDER_PASS_HPP
#define ZENENGINE_RENDER_PASS_HPP
#include <vector>
#include "base.hpp"

namespace zen::vkh {
class Device;
using AttachmentDescriptions = std::vector<VkAttachmentDescription>;
using SubpassDescriptions = std::vector<VkSubpassDescription>;

struct SubpassDepInfo {
  uint32_t input_att_read{0};
  uint32_t color_att_read_write{0};
  uint32_t depth_att_write{0};
  uint32_t depth_att_read{0};
  bool extern_depth_dep{false};
  bool extern_color_dep{true};
};

struct RenderpassInfo {
  AttachmentDescriptions att_descriptions;
  SubpassDescriptions subpass_descriptions;
  SubpassDepInfo subpass_dep_info;
};
class Renderpass {
public:
  Renderpass(const Device& device, const RenderpassInfo& rp_info);
  ~Renderpass();

  auto handle() const { return m_render_pass; }
private:
  std::vector<VkSubpassDependency> get_subpass_deps(const SubpassDepInfo& info,
                                                    uint32_t subpass_count);
  const Device& m_device;
  VkRenderPass m_render_pass{nullptr};
};
}  // namespace zen::vkh
#endif  //ZENENGINE_RENDER_PASS_HPP
