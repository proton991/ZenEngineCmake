#ifndef ZENENGINE_DEVICE_HPP
#define ZENENGINE_DEVICE_HPP
#include <string>
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
  VkQueue present_queue() const { return m_queue_info.queues[QUEUE_INDEX_GRAPHICS]; }

  void create_semaphore(const VkSemaphoreCreateInfo& semaphore_ci, VkSemaphore* semaphore,
                        const std::string& name) const;
  void destroy_semaphore(VkSemaphore semaphore) const;

  void create_image_view(const VkImageViewCreateInfo& image_view_ci, VkImageView* image_view,
                         const std::string& name) const;
  void destroy_image_view(VkImageView image_view) const;

  std::vector<VkSurfaceFormatKHR> get_surface_formats(VkSurfaceKHR surface) const;
  std::vector<VkPresentModeKHR> get_surface_present_modes(VkSurfaceKHR surface) const;
  VkSurfaceCapabilitiesKHR get_surface_capabilities(VkSurfaceKHR surface) const;

  VkDevice get_device() const;
  VmaAllocator get_allocator() const;
  VkPhysicalDevice get_gpu() const;

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
}  // namespace zen::vkh
#endif  //EASYGRAPHICS_DEVICE_HPP
