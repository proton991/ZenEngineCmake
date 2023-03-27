#ifndef ZENENGINE_IMAGE_HPP
#define ZENENGINE_IMAGE_HPP
#include <string>
#include "base.hpp"
namespace zen::vkh {
class Device;
struct ImageInfo {
  VkFormat format;  //The color format.
  VkImageUsageFlags image_usage;  //The image usage flags.
  uint32_t layer_count{1};  // The image layer count
  VkSampleCountFlagBits sample_count{VK_SAMPLE_COUNT_1_BIT};  // The sample count.
  VkExtent2D image_extent;  // The width and height of the image.
  std::string name; // The name of the VkImage used for DebugUtil.
};
class Image {
public:
  Image(const Device& device, ImageInfo info);
  Image(Image&&) noexcept;

  ~Image();

  Image& operator=(const Image&) = delete;
  Image& operator=(Image&&)      = delete;
  Image(const Image&)            = delete;

  VkImageView get_view() const { return m_image_view; }

private:
  const Device& m_device;
  ImageInfo m_info;
  VmaAllocation m_allocation{VK_NULL_HANDLE};
  VkImage m_image{VK_NULL_HANDLE};
  VkImageView m_image_view{VK_NULL_HANDLE};
};
}  // namespace zen::vkh
#endif  //ZENENGINE_IMAGE_HPP
