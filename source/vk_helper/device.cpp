#include "device.hpp"
#include "debug.hpp"
#include "logging.hpp"

namespace zen::vkh {
static const char* QUEUE_NAMES[] = {"Graphics", "Compute", "transfer", "video_decode"};
Device::~Device() {
  if (m_allocator) {
    vmaDestroyAllocator(m_allocator);
  }
  if (m_device != VK_NULL_HANDLE) {
    vkDeviceWaitIdle(m_device);
  }
  if (m_device != VK_NULL_HANDLE) {
    vkDestroyDevice(m_device, nullptr);
  }
}

void Device::set_context(Context& ctx) {
  m_gpu        = ctx.m_gpu;
  m_instance   = ctx.m_instance;
  m_device     = ctx.m_device;
  m_queue_info = ctx.m_q_info;
  m_features   = ctx.m_feature;
  m_gpu_props  = ctx.m_gpu_props;
  m_mem_props  = ctx.m_mem_props;

  init_vma();
  display_info();
}

void Device::create_image_view(const VkImageViewCreateInfo& image_view_ci, VkImageView* image_view,
                               const std::string& name) const {
  VK_CHECK(vkCreateImageView(m_device, &image_view_ci, nullptr, image_view), "vkCreateImageView");
  DebugUtil::get().set_obj_name(*image_view, name.data());
}

void Device::destroy_image_view(VkImageView image_view) const {
  vkDestroyImageView(m_device, image_view, nullptr);
}

VkDevice Device::get_device() const {
  return m_device;
}
std::vector<VkSurfaceFormatKHR> Device::get_surface_formats(VkSurfaceKHR surface) const {
  VK_ASSERT(m_gpu);
  std::uint32_t count = 0;
  VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_gpu, surface, &count, nullptr),
           "vkGetPhysicalDeviceSurfaceFormatsKHR");

  std::vector<VkSurfaceFormatKHR> surface_formats(count);
  VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_gpu, surface, &count, surface_formats.data()),
           "vkGetPhysicalDeviceSurfaceFormatsKHR");
  return surface_formats;
}

std::vector<VkPresentModeKHR> Device::get_surface_present_modes(VkSurfaceKHR surface) const {
  assert(m_gpu);
  std::uint32_t count = 0;
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_gpu, surface, &count, nullptr),
           "vkGetPhysicalDeviceSurfaceFormatsKHR");
  std::vector<VkPresentModeKHR> present_modes(count);
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_gpu, surface, &count, present_modes.data()),
           "vkGetPhysicalDeviceSurfaceFormatsKHR");
  return present_modes;
}

VkSurfaceCapabilitiesKHR Device::get_surface_capabilities(VkSurfaceKHR surface) const {
  VkSurfaceCapabilitiesKHR caps{};
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_gpu, surface, &caps),
           "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
  return caps;
}

VmaAllocator Device::get_allocator() const {
  return m_allocator;
}

VkPhysicalDevice Device::get_gpu() const {
  return m_gpu;
}

void Device::create_semaphore(const VkSemaphoreCreateInfo& semaphore_ci, VkSemaphore* semaphore,
                              const std::string& name) const {
  VK_CHECK(vkCreateSemaphore(m_device, &semaphore_ci, nullptr, semaphore), "vkCreateSemaphore");
  DebugUtil::get().set_obj_name(*semaphore, name.data());
}

void Device::destroy_semaphore(VkSemaphore semaphore) const {
  vkDestroySemaphore(m_device, semaphore, nullptr);
}

void Device::init_vma() {
  VmaAllocatorCreateInfo allocatorInfo{};
  allocatorInfo.physicalDevice = m_gpu;
  allocatorInfo.device         = m_device;
  allocatorInfo.instance       = m_instance;
  // let VMA fetch vulkan function pointers dynamically
  VmaVulkanFunctions vmaVulkanFunctions{};
  vmaVulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
  vmaVulkanFunctions.vkGetDeviceProcAddr   = vkGetDeviceProcAddr;

  allocatorInfo.pVulkanFunctions = &vmaVulkanFunctions;
  if (vmaCreateAllocator(&allocatorInfo, &m_allocator) != VK_SUCCESS) {
    logger::critical("Failed to initialize vma");
  }
}

void Device::display_info() {
  logger::info("Using GPU: {}", m_gpu_props.deviceName);
  logger::info("The GPU has a minimum buffer alignment of {}",
               m_gpu_props.limits.minUniformBufferOffsetAlignment);
  logger::info("Max number of color attachments: {}", m_gpu_props.limits.maxColorAttachments);
  logger::info("Using Device Queues: ");
  logger::set_list_pattern();
  for (int i = 0; i < QUEUE_INDEX_COUNT; i++) {
    if (m_queue_info.family_indices[i] != VK_QUEUE_FAMILY_IGNORED) {
      logger::info("Queue: {}, family index: {}, queue index: {}", QUEUE_NAMES[i],
                   m_queue_info.family_indices[i], m_queue_info.queue_indices[i]);
    }
  }
  logger::set_default_pattern();
}
}  // namespace zen::vkh