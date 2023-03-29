#include "render_pass.hpp"
#include "device.hpp"
#include "initializer.hpp"
#include "logging.hpp"

namespace zen::vkh {
Renderpass::Renderpass(const Device& device, const RenderpassInfo& rp_info) : m_device(device) {
  const auto subpass_count = rp_info.subpass_descriptions.size();
  const auto dependencies  = get_subpass_deps(rp_info.subpass_dep_info, subpass_count);
  VkRenderPassCreateInfo renderpass_ci{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
  renderpass_ci.attachmentCount = rp_info.att_descriptions.size();
  renderpass_ci.pAttachments    = rp_info.att_descriptions.data();
  renderpass_ci.subpassCount    = subpass_count;
  renderpass_ci.pSubpasses      = rp_info.subpass_descriptions.data();
  renderpass_ci.dependencyCount = dependencies.size();
  renderpass_ci.pDependencies   = dependencies.data();
  m_device.create_render_pass(renderpass_ci, &m_render_pass, "render pass");
}

Renderpass::~Renderpass() {
  m_device.destroy_render_pass(m_render_pass);
}

std::vector<VkSubpassDependency> Renderpass::get_subpass_deps(const SubpassDepInfo& info,
                                                              uint32_t subpass_count) {
  uint32_t last_subpass = subpass_count - 1;
  // external dependencies
  VkSubpassDependency init = {};
  std::vector<VkSubpassDependency> deps(subpass_count+1, init);
  if (info.extern_depth_dep) {
    deps[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    deps[0].dstSubpass = 0;
    deps[0].srcStageMask |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    deps[0].dstStageMask |=
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    deps[0].srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    deps[0].dstAccessMask |=
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    deps[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    deps[subpass_count].srcSubpass = last_subpass;
    deps[subpass_count].dstSubpass = VK_SUBPASS_EXTERNAL;
    deps[subpass_count].srcStageMask |=
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    deps[subpass_count].dstStageMask |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    deps[subpass_count].srcAccessMask |=
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    deps[subpass_count].dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    ;
    deps[subpass_count].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
  }
  if (info.extern_color_dep) {
    deps[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    deps[0].dstSubpass = 0;
    deps[0].srcStageMask |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    deps[0].dstStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    deps[0].srcAccessMask |= VK_ACCESS_MEMORY_READ_BIT;
    deps[0].dstAccessMask |=
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    deps[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    deps[subpass_count].srcSubpass = last_subpass;
    deps[subpass_count].dstSubpass = VK_SUBPASS_EXTERNAL;
    deps[subpass_count].srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    deps[subpass_count].dstStageMask |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    deps[subpass_count].srcAccessMask |=
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    deps[subpass_count].dstAccessMask |= VK_ACCESS_MEMORY_READ_BIT;
    deps[subpass_count].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
  }
  for (uint32_t subpass = 1; subpass < subpass_count; subpass++) {
    deps[subpass].srcSubpass = subpass - 1;
    deps[subpass].dstSubpass = subpass;
    if (info.color_att_read_write & (1u << (subpass - 1))) {
      deps[subpass].srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      deps[subpass].srcAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    if (info.depth_att_write & (1u << (subpass - 1))) {
      deps[subpass].srcStageMask |=
          VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
      deps[subpass].srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }

    if (info.color_att_read_write & (1u << subpass)) {
      deps[subpass].dstStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      deps[subpass].dstAccessMask |=
          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    }

    if (info.depth_att_read & (1u << subpass)) {
      deps[subpass].dstStageMask |=
          VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
      deps[subpass].dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    }

    if (info.depth_att_write & (1u << subpass)) {
      deps[subpass].dstStageMask |=
          VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
      deps[subpass].dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
                                     VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    }

    if (info.input_att_read & (1u << subpass)) {
      deps[subpass].dstStageMask |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      deps[subpass].dstAccessMask |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
    }
  }
  return deps;
}
}  // namespace zen::vkh