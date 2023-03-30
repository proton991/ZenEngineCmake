#include "render_pass.hpp"
#include "device.hpp"
#include "initializer.hpp"
#include "logging.hpp"

namespace zen::vkh {
SubpassInfo::SubpassInfo(const std::vector<uint32_t>& colors, const std::vector<uint32_t>& inputs,
                         uint32_t depth_stencil) {
  for (const auto color : colors) {
    color_refs.emplace_back(color_att_ref(color));
  }
  for (const auto input : inputs) {
    color_refs.emplace_back(input_att_ref(input));
  }
  depth_stencil_ref = depth_stencil_att_ref(depth_stencil);
}

RenderPassBuilder& RenderPassBuilder::add_present_att(VkFormat format) {
  VkAttachmentDescription att = {
      .format         = format,
      .samples        = VK_SAMPLE_COUNT_1_BIT,
      .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR  // present layout
  };
  m_attachments.emplace_back(att);
  return *this;
}

RenderPassBuilder& RenderPassBuilder::add_color_att(VkFormat format, bool clear) {
  VkAttachmentDescription att = {
      .format         = format,
      .samples        = VK_SAMPLE_COUNT_1_BIT,
      .loadOp         = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL  // color attachment layout
  };
  m_attachments.emplace_back(att);
  return *this;
}

RenderPassBuilder& RenderPassBuilder::add_depth_stencil_att(VkFormat format) {
  VkAttachmentDescription att = {
      .format         = format,
      .samples        = VK_SAMPLE_COUNT_1_BIT,
      .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL  // depth stencil layout
  };
  m_attachments.emplace_back(att);
  return *this;
}

RenderPassBuilder& RenderPassBuilder::add_subpass(const std::vector<uint32_t>& color_refs,
                                                  const std::vector<uint32_t>& input_refs,
                                                  uint32_t depth_stencil_ref) {
  m_subpass_infos.emplace_back(color_refs, input_refs, depth_stencil_ref);
  return *this;
}

RenderPassBuilder& RenderPassBuilder::set_subpass_deps(const SubpassDepInfo& info) {
  auto subpass_count    = static_cast<uint32_t>(m_subpass_infos.size());
  uint32_t last_subpass = subpass_count - 1;
  // external dependencies
  VkSubpassDependency init = {};
  std::vector<VkSubpassDependency> deps(subpass_count + 1, init);
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
  m_subpass_deps = std::move(deps);
  return *this;
}

VkRenderPass RenderPassBuilder::build() {
  std::vector<VkSubpassDescription> subpass_descriptions;
  for (const auto& subpass_info : m_subpass_infos) {
    VkSubpassDescription subpass{
        .pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = static_cast<uint32_t>(subpass_info.input_refs.size()),
        .pInputAttachments =
            subpass_info.input_refs.empty() ? nullptr : subpass_info.input_refs.data(),
        .colorAttachmentCount = static_cast<uint32_t>(subpass_info.color_refs.size()),
        .pColorAttachments =
            subpass_info.color_refs.empty() ? nullptr : subpass_info.color_refs.data(),
        .pDepthStencilAttachment = &subpass_info.depth_stencil_ref  //
    };
    subpass_descriptions.emplace_back(subpass);
  }
  VkRenderPassCreateInfo render_pass_ci{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
  render_pass_ci.attachmentCount = static_cast<uint32_t>(m_attachments.size());
  render_pass_ci.pAttachments    = m_attachments.data();
  render_pass_ci.subpassCount    = static_cast<uint32_t>(subpass_descriptions.size());
  render_pass_ci.pSubpasses      = subpass_descriptions.data();
  render_pass_ci.dependencyCount = static_cast<uint32_t>(m_subpass_deps.size());
  render_pass_ci.pDependencies   = m_subpass_deps.data();
  VkRenderPass render_pass;
  m_device.create_render_pass(render_pass_ci, &render_pass, "render pass");

  return render_pass;
}
}  // namespace zen::vkh