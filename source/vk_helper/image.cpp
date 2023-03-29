#include "image.hpp"
#include "debug.hpp"
#include "device.hpp"
#include "logging.hpp"

namespace zen::vkh {
Image::Image(const Device& device, ImageInfo info) : m_device(device), m_info(std::move(info)) {
  VkImageAspectFlags aspect_mask;
  if (m_info.image_usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
    aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
  } else if (m_info.image_usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
    aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
  } else {
    spdlog::error("Image usage not supported!");
    VK_ASSERT(false);
  }

  const VkImageCreateInfo image_ci = {
      .sType     = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .imageType = VK_IMAGE_TYPE_2D,
      .format    = m_info.format,
      .extent{
          .width  = m_info.image_extent.width,
          .height = m_info.image_extent.height,
          .depth  = 1,
      },
      .mipLevels     = 1,
      .arrayLayers   = m_info.layer_count,
      .samples       = m_info.sample_count,
      .tiling        = VK_IMAGE_TILING_OPTIMAL,
      .usage         = m_info.image_usage,
      .sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
  };
  VmaAllocationCreateInfo vma_alloc_ci{};
  vma_alloc_ci.usage    = VMA_MEMORY_USAGE_AUTO;
  vma_alloc_ci.flags    = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
  vma_alloc_ci.priority = 1.0f;

  VK_CHECK(vmaCreateImage(m_device.get_allocator(), &image_ci, &vma_alloc_ci, &m_image,
                          &m_allocation, nullptr),
           "vmaCreateImage");
  vmaSetAllocationName(m_device.get_allocator(), m_allocation, m_info.name.c_str());
  DebugUtil::get().set_obj_name(m_image, m_info.name.c_str());
  VkImageViewCreateInfo image_view_ci = {
      .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image    = m_image,
      .viewType = (m_info.layer_count == 1) ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY,
      .format   = m_info.format,
      .subresourceRange{
          .aspectMask     = aspect_mask,
          .baseMipLevel   = 0,
          .levelCount     = 1,
          .baseArrayLayer = 0,
          .layerCount     = m_info.layer_count,
      },
  };
  m_device.create_image_view(image_view_ci, &m_image_view, m_info.name);
}

Image::Image(Image&& other) noexcept : m_device(other.m_device) {
  m_allocation = other.m_allocation;
  m_image      = other.m_image;
  m_info       = std::move(other.m_info);
  m_image_view = other.m_image_view;
}

Image::~Image() {
  m_device.destroy_image_view(m_image_view);
  vmaDestroyImage(m_device.get_allocator(), m_image, m_allocation);
}

}  // namespace zen::vkh