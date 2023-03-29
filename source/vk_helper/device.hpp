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

  uint32_t graphics_queue_family_index() const {
    return m_queue_info.family_indices[QUEUE_INDEX_GRAPHICS];
  }
  VkQueue graphics_queue() const { return m_queue_info.queues[QUEUE_INDEX_GRAPHICS]; }
  VkQueue compute_queue() const { return m_queue_info.queues[QUEUE_INDEX_COMPUTE]; }
  VkQueue transfer_queue() const { return m_queue_info.queues[QUEUE_INDEX_TRANSFER]; }
  VkQueue present_queue() const { return m_queue_info.queues[QUEUE_INDEX_GRAPHICS]; }

  void create_render_pass(const VkRenderPassCreateInfo& info, VkRenderPass* render_pass, const std::string& name) const;
  void destroy_render_pass(VkRenderPass render_pass) const;

  void create_framebuffer(const VkFramebufferCreateInfo& info, VkFramebuffer* framebuffer, const std::string& name) const;
  void destroy_framebuffer(VkFramebuffer framebuffer) const;

  void create_shader_module(const VkShaderModuleCreateInfo& info, VkShaderModule* shader_module, const std::string& name) const;
  void destroy_shader_module(VkShaderModule shader_module) const;

  void create_semaphore(const VkSemaphoreCreateInfo& semaphore_ci, VkSemaphore* semaphore,
                        const std::string& name) const;
  void destroy_semaphore(VkSemaphore semaphore) const;

  void create_fence(const VkFenceCreateInfo& fence_ci, VkFence* fence, const std::string& name) const;
  void destroy_fence(VkFence fence) const;

  void create_command_pool(const VkCommandPoolCreateInfo& info, VkCommandPool* cmd_pool, const std::string& name) const;
  void destroy_command_pool(VkCommandPool cmd_pool) const;

  void allocate_command_buffer(const VkCommandBufferAllocateInfo& info, VkCommandBuffer* cmd_buffer, const std::string& name) const;

  void create_swapchain(const VkSwapchainCreateInfoKHR& info, VkSwapchainKHR* swapchain, const std::string& name) const;
  void destroy_swapchain(VkSwapchainKHR swapchain) const;

  void create_image_view(const VkImageViewCreateInfo& image_view_ci, VkImageView* image_view,
                         const std::string& name) const;
  void destroy_image_view(VkImageView image_view) const;

  std::vector<VkSurfaceFormatKHR> get_surface_formats(VkSurfaceKHR surface) const;
  std::vector<VkPresentModeKHR> get_surface_present_modes(VkSurfaceKHR surface) const;
  VkSurfaceCapabilitiesKHR get_surface_capabilities(VkSurfaceKHR surface) const;

  VkDevice handle() const;
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
