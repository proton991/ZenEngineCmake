#include "device.hpp"
#include "logging.hpp"

namespace zen::vkh {
static const char* QUEUE_NAMES[] = {"Graphics", "Compute", "transfer", "video_decode"};
Device::~Device() {
  if (m_allocator) {
    vmaDestroyAllocator(m_allocator);
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

VmaAllocator Device::get_allocator() const {
  return m_allocator;
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