#ifndef ZENENGINE_DEBUG_HPP
#define ZENENGINE_DEBUG_HPP
#include "base.hpp"
#include "logging.hpp"

namespace zen::vkh {
class DebugUtil final {
public:
  ZEN_NO_COPY_MOVE(DebugUtil)

  static DebugUtil& get() {
    static DebugUtil debugUtil;
    return debugUtil;
  }
  ~DebugUtil() = default;

  void set_obj_name(const VkDebugUtilsMessengerEXT& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT);
  }

  void set_obj_name(const VkAccelerationStructureKHR& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR);
  }
  void set_obj_name(const VkBuffer& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_BUFFER);
  }
  void set_obj_name(const VkCommandBuffer& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_COMMAND_BUFFER);
  }
  void set_obj_name(const VkDescriptorSet& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_DESCRIPTOR_SET);
  }
  void set_obj_name(const VkDescriptorSetLayout& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT);
  }
  void set_obj_name(const VkDeviceMemory& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_DEVICE_MEMORY);
  }
  void set_obj_name(const VkFramebuffer& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_FRAMEBUFFER);
  }
  void set_obj_name(const VkImage& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_IMAGE);
  }
  void set_obj_name(const VkImageView& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_IMAGE_VIEW);
  }
  void set_obj_name(const VkPipeline& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_PIPELINE);
  }
  void set_obj_name(const VkQueue& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_QUEUE);
  }
  void set_obj_name(const VkRenderPass& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_RENDER_PASS);
  }
  void set_obj_name(const VkSemaphore& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_SEMAPHORE);
  }
  void set_obj_name(const VkFence& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_FENCE);
  }
  void set_obj_name(const VkShaderModule& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_SHADER_MODULE);
  }
  void set_obj_name(const VkSwapchainKHR& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_SWAPCHAIN_KHR);
  }
  void set_obj_name(const VkSurfaceKHR& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_SURFACE_KHR);
  }
  void set_obj_name(const VkDevice& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_DEVICE);
  }
  void set_obj_name(const VkInstance& object, const char* name) const {
    set_obj_name(object, name, VK_OBJECT_TYPE_INSTANCE);
  }

private:
  DebugUtil() = default;
  template <typename T>
  void set_obj_name(const T& object, const char* name, VkObjectType type) const {
#ifdef ZEN_DEBUG
    VkDebugUtilsObjectNameInfoEXT info{};
    info.sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    info.pNext        = nullptr;
    info.objectHandle = reinterpret_cast<const uint64_t&>(object);
    info.objectType   = type;
    info.pObjectName  = name;

    if (vkSetDebugUtilsObjectNameEXT(volkGetLoadedDevice(), &info) != VK_SUCCESS) {
      logger::error("Failed to set object name for {}", name);
    }
#endif
  }
};
}  // namespace zen::vkh
#endif  //ZENENGINE_DEBUG_HPP
