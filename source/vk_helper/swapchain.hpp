#ifndef ZENENGINE_SWAPCHAIN_HPP
#define ZENENGINE_SWAPCHAIN_HPP
#include <vector>
#include "base.hpp"

namespace zen::vkh {
class Device;
class Swapchain {
public:
  Swapchain(const Device& device, VkSurfaceKHR surface, uint32_t width, uint32_t height,
            bool vsync);
  ~Swapchain();

private:
  void setup(std::uint32_t width, std::uint32_t height, bool vsync_enabled,
             VkSwapchainKHR old_swapchain = nullptr);
  std::vector<VkImage> get_swapchain_images();
  VkSurfaceFormatKHR choose_surface_format(const std::vector<VkSurfaceFormatKHR>& available);
  VkExtent2D choose_swapchain_extent(const VkExtent2D& requested_extent,
                                     const VkExtent2D& min_extent, const VkExtent2D& max_extent);
  VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes,
                                       bool vsync_enabled);
  VkCompositeAlphaFlagBitsKHR choose_composite_alpha(VkCompositeAlphaFlagBitsKHR request_alpha,
                                                     VkCompositeAlphaFlagsKHR supported_alpha);

  const Device& m_device;
  VkSwapchainKHR m_swapchain{nullptr};
  VkSurfaceKHR m_surface{nullptr};
  VkSurfaceFormatKHR m_surface_format{};
  std::vector<VkImage> m_images;
  std::vector<VkImageView> m_image_views;
  VkExtent2D m_extent{};
  bool m_vsync{true};
};
}  // namespace zen::vkh
#endif  //ZENENGINE_SWAPCHAIN_HPP
