#ifndef EASYGRAPHICS_DEVICE_HPP
#define EASYGRAPHICS_DEVICE_HPP
//#include <vk_mem_alloc.h>
#include "context.hpp"

namespace zen::vkh {
class Device {
public:
  Device() = default;
  ~Device();
  void set_context(Context& ctx);

  VkQueue graphics_queue() const { return m_queue_info.queues[QUEUE_INDEX_GRAPHICS]; }
  VkQueue compute_queue() const { return m_queue_info.queues[QUEUE_INDEX_COMPUTE]; }
  VkQueue transfer_queue() const { return m_queue_info.queues[QUEUE_INDEX_TRANSFER]; }

  VmaAllocator get_allocator() const;

private:
  void init_vma();
  void display_info();

  VkInstance m_instance{nullptr};
  VkPhysicalDevice m_gpu{nullptr};
  VkDevice m_device{nullptr};
  QueueInfo m_queue_info{};
  VkPhysicalDeviceMemoryProperties m_mem_props{};
  VkPhysicalDeviceProperties m_gpu_props{};
  DeviceFeatures m_features;
  VmaAllocator m_allocator{VK_NULL_HANDLE};
};
}  // namespace ezg::vulkan
#endif  //EASYGRAPHICS_DEVICE_HPP
