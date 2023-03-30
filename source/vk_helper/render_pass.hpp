#ifndef ZENENGINE_RENDER_PASS_HPP
#define ZENENGINE_RENDER_PASS_HPP
#include <vector>
#include "base.hpp"

namespace zen::vkh {
class Device;
struct SubpassDepInfo {
  uint32_t input_att_read{0};
  uint32_t color_att_read_write{0};
  uint32_t depth_att_write{0};
  uint32_t depth_att_read{0};
  bool extern_depth_dep{false};
  bool extern_color_dep{true};
};

struct SubpassInfo {
  SubpassInfo(const std::vector<uint32_t>& colors, const std::vector<uint32_t>& inputs,
              uint32_t depth_stencil);
  std::vector<VkAttachmentReference> color_refs;
  std::vector<VkAttachmentReference> input_refs;
  VkAttachmentReference depth_stencil_ref{};
};

class RenderPassBuilder {
public:
  explicit RenderPassBuilder(const Device& device) : m_device(device) {}
  ~RenderPassBuilder() = default;

  RenderPassBuilder& add_present_att(VkFormat format);
  RenderPassBuilder& add_color_att(VkFormat format, bool clear);
  RenderPassBuilder& add_depth_stencil_att(VkFormat format);

  RenderPassBuilder& add_subpass(const std::vector<uint32_t>& color_refs,
                                 const std::vector<uint32_t>& input_refs,
                                 uint32_t depth_stencil_ref);

  RenderPassBuilder& set_subpass_deps(const SubpassDepInfo& info);

  VkRenderPass build();
private:
  const Device& m_device;
  std::vector<VkAttachmentDescription> m_attachments;
  std::vector<SubpassInfo> m_subpass_infos;
  std::vector<VkAttachmentReference> m_attachment_refs;
  std::vector<VkSubpassDependency> m_subpass_deps;
};
}  // namespace zen::vkh
#endif  //ZENENGINE_RENDER_PASS_HPP
