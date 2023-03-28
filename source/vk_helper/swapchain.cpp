#include "swapchain.hpp"
#include "device.hpp"
#include "logging.hpp"

namespace zen::vkh {
Swapchain::Swapchain(const Device& device, VkSurfaceKHR surface, uint32_t width, uint32_t height,
                     bool vsync)
    : m_device(device), m_surface(surface), m_vsync(vsync) {
  setup(width, height, vsync);
}

Swapchain::~Swapchain() {
  vkDestroySwapchainKHR(m_device.handle(), m_swapchain, nullptr);
  for (auto& image_view : m_image_views) {
    m_device.destroy_image_view(image_view);
  }
  m_images.clear();
  m_image_views.clear();

}

VkImageView Swapchain::get_image_view(uint32_t index) {
  VK_ASSERT(index >= 0 && index < m_image_views.size());
  return m_image_views.at(index);
}

void Swapchain::setup(std::uint32_t width, std::uint32_t height, bool vsync_enabled,
                      VkSwapchainKHR old_swapchain) {
  const auto caps = m_device.get_surface_capabilities(m_surface);
  m_extent = choose_swapchain_extent({width, height}, caps.minImageExtent, caps.maxImageExtent);
  m_surface_format = choose_surface_format(m_device.get_surface_formats(m_surface));
  const auto composite_alpha =
      choose_composite_alpha(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, caps.supportedCompositeAlpha);
  VkSwapchainCreateInfoKHR swapchain_ci = {
      .sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface          = m_surface,
      .minImageCount    = (caps.maxImageCount != 0)
                              ? std::min(caps.minImageCount + 1, caps.maxImageCount)
                              : std::max(caps.minImageCount + 1, caps.minImageCount),
      .imageFormat      = m_surface_format.format,
      .imageColorSpace  = m_surface_format.colorSpace,
      .imageExtent      = m_extent,
      .imageArrayLayers = 1,
      .imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .preTransform     = ((VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR & caps.supportedTransforms) != 0u)
                              ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
                              : caps.currentTransform,
      .compositeAlpha   = composite_alpha,
      .presentMode =
          choose_present_mode(m_device.get_surface_present_modes(m_surface), vsync_enabled),
      .clipped      = VK_TRUE,
      .oldSwapchain = old_swapchain,
  };
  spdlog::trace("Creating swapchain");
  VK_CHECK(vkCreateSwapchainKHR(m_device.handle(), &swapchain_ci, nullptr, &m_swapchain),
           "vkCreateSwapchainKHR");
  m_images = get_swapchain_images();
  m_image_views.resize(m_images.size());
  if (m_images.empty()) {
    spdlog::error("Failed to get swapchain images!");
  }
  spdlog::trace("Creating {} swapchain image views", m_images.size());
  for (std::size_t img_index = 0; img_index < m_images.size(); img_index++) {
    VkImageViewCreateInfo img_view_ci = {
        .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image    = m_images[img_index],
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format   = m_surface_format.format,
        .components{
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY,
        },
        .subresourceRange{
            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel   = 0,
            .levelCount     = 1,
            .baseArrayLayer = 0,
            .layerCount     = 1,
        },
    };
    std::string name = "swapchain image view " + std::to_string(img_index);
    m_device.create_image_view(img_view_ci, &m_image_views[img_index], name);
  }
}

std::vector<VkImage> Swapchain::get_swapchain_images() {
  std::uint32_t count = 0;
  VK_CHECK(vkGetSwapchainImagesKHR(m_device.handle(), m_swapchain, &count, nullptr),
           "vkGetSwapchainImagesKHR");
  std::vector<VkImage> images(count);
  VK_CHECK(vkGetSwapchainImagesKHR(m_device.handle(), m_swapchain, &count, images.data()),
           "vkGetSwapchainImagesKHR");
  return images;
}

VkExtent2D Swapchain::choose_swapchain_extent(const VkExtent2D& requested_extent,
                                              const VkExtent2D& min_extent,
                                              const VkExtent2D& max_extent) {
  VkExtent2D extent{};
  extent.width  = std::clamp(requested_extent.width, min_extent.width, min_extent.width);
  extent.height = std::clamp(requested_extent.height, max_extent.height, max_extent.height);
  return extent;
}

VkCompositeAlphaFlagBitsKHR Swapchain::choose_composite_alpha(
    VkCompositeAlphaFlagBitsKHR request_alpha, VkCompositeAlphaFlagsKHR supported_alpha) {
  if ((request_alpha & supported_alpha) != 0u) {
    return request_alpha;
  }

  static const std::vector<VkCompositeAlphaFlagBitsKHR> composite_alpha_flags{
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR};

  for (const auto flag : composite_alpha_flags) {
    if ((flag & supported_alpha) != 0u) {
      return flag;
    }
  }
  return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
}

VkPresentModeKHR Swapchain::choose_present_mode(
    const std::vector<VkPresentModeKHR>& available_present_modes, bool vsync_enabled) {
  static const std::vector<VkPresentModeKHR> default_present_mode_priorities{
      VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_RELAXED_KHR, VK_PRESENT_MODE_FIFO_KHR};
  if (!vsync_enabled) {
    for (const auto requested_present_mode : default_present_mode_priorities) {
      const auto present_mode = std::find(available_present_modes.begin(),
                                          available_present_modes.end(), requested_present_mode);
      if (present_mode != available_present_modes.end()) {
        return *present_mode;
      }
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR Swapchain::choose_surface_format(
    const std::vector<VkSurfaceFormatKHR>& available_formats) {
  static const std::vector<VkSurfaceFormatKHR> default_surface_format_priority_list{
      {VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
      {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}};
  VkSurfaceFormatKHR chosen_format{};
  for (const auto available_format : available_formats) {
    auto format = std::find_if(default_surface_format_priority_list.begin(),
                               default_surface_format_priority_list.end(),
                               [&](const VkSurfaceFormatKHR candidate) {
                                 return available_format.format == candidate.format &&
                                        available_format.colorSpace == candidate.colorSpace;
                               });

    if (format != default_surface_format_priority_list.end()) {
      chosen_format = *format;
    }
  }
  return chosen_format;
}
}  // namespace zen::vkh