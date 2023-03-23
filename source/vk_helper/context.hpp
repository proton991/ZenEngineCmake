#ifndef ZENENGINE_CONTEXT_HPP
#define ZENENGINE_CONTEXT_HPP
#include <vector>
#include "base.hpp"

namespace zen::vkh {
struct DeviceFeatures {
  bool supports_debug_utils                      = false;
  bool supports_mirror_clamp_to_edge             = false;
  bool supports_google_display_timing            = false;
  bool supports_nv_device_diagnostic_checkpoints = false;
  bool supports_external_memory_host             = false;
  bool supports_surface_capabilities2            = false;
  bool supports_full_screen_exclusive            = false;
  bool supports_descriptor_indexing              = false;
  bool supports_conservative_rasterization       = false;
  bool supports_draw_indirect_count              = false;
  bool supports_driver_properties                = false;
  bool supports_calibrated_timestamps            = false;
  bool supports_memory_budget                    = false;
  bool supports_astc_decode_mode                 = false;
  bool supports_sync2                            = false;
  bool supports_video_queue                      = false;
  bool supports_video_decode_queue               = false;
  bool supports_video_decode_h264                = false;
  bool supports_pipeline_creation_cache_control  = false;
  bool supports_format_feature_flags2            = false;
  bool supports_external                         = false;
  bool supports_image_format_list                = false;
  bool supports_shader_float_control             = false;
  bool supports_tooling_info                     = false;
  bool supports_hdr_metadata                     = false;
  bool supports_swapchain_colorspace             = false;

  // Vulkan 1.1 core
  VkPhysicalDeviceFeatures enabled_features                                        = {};
  VkPhysicalDeviceMultiviewFeatures multiview_features                             = {};
  VkPhysicalDeviceShaderDrawParametersFeatures shader_draw_parameters_features     = {};
  VkPhysicalDeviceSamplerYcbcrConversionFeatures sampler_ycbcr_conversion_features = {};
  VkPhysicalDeviceMultiviewProperties multiview_properties                         = {};
  VkPhysicalDeviceSubgroupProperties subgroup_properties                           = {};

  // KHR
  VkPhysicalDeviceTimelineSemaphoreFeaturesKHR timeline_semaphore_features = {};
  VkPhysicalDevicePerformanceQueryFeaturesKHR performance_query_features   = {};
  VkPhysicalDeviceDriverPropertiesKHR driver_properties                    = {};
  VkPhysicalDeviceSynchronization2FeaturesKHR sync2_features               = {};
  VkPhysicalDevicePresentIdFeaturesKHR present_id_features                 = {};
  VkPhysicalDevicePresentWaitFeaturesKHR present_wait_features             = {};
  VkPhysicalDevice8BitStorageFeaturesKHR storage_8bit_features             = {};
  VkPhysicalDevice16BitStorageFeaturesKHR storage_16bit_features           = {};
  VkPhysicalDeviceFloat16Int8FeaturesKHR float16_int8_features             = {};
  VkPhysicalDeviceFloatControlsPropertiesKHR float_control_properties      = {};
  VkPhysicalDeviceIDProperties id_properties                               = {};

  // EXT
  VkPhysicalDeviceExternalMemoryHostPropertiesEXT host_memory_properties            = {};
  VkPhysicalDeviceSubgroupSizeControlFeaturesEXT subgroup_size_control_features     = {};
  VkPhysicalDeviceSubgroupSizeControlPropertiesEXT subgroup_size_control_properties = {};
  VkPhysicalDeviceHostQueryResetFeaturesEXT host_query_reset_features               = {};
  VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT demote_to_helper_invocation_features =
      {};
  VkPhysicalDeviceScalarBlockLayoutFeaturesEXT scalar_block_features                           = {};
  VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR ubo_std430_features                   = {};
  VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptor_indexing_features                   = {};
  VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptor_indexing_properties               = {};
  VkPhysicalDeviceConservativeRasterizationPropertiesEXT conservative_rasterization_properties = {};
  VkPhysicalDeviceMemoryPriorityFeaturesEXT memory_priority_features                           = {};
  VkPhysicalDeviceASTCDecodeFeaturesEXT astc_decode_features                                   = {};
  VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT astc_hdr_features                       = {};
  VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT pipeline_creation_cache_control_features =
      {};

  // Vendor
  VkPhysicalDeviceComputeShaderDerivativesFeaturesNV compute_shader_derivative_features = {};
};

enum QueueIndices {
  QUEUE_INDEX_GRAPHICS,
  QUEUE_INDEX_COMPUTE,
  QUEUE_INDEX_TRANSFER,
  QUEUE_INDEX_VIDEO_DECODE,
  QUEUE_INDEX_COUNT
};

struct QueueInfo {
  QueueInfo() {
    for (auto& index : family_indices)
      index = VK_QUEUE_FAMILY_IGNORED;
  }
  VkQueue queues[QUEUE_INDEX_COUNT]          = {};
  uint32_t family_indices[QUEUE_INDEX_COUNT] = {};
  uint32_t queue_indices[QUEUE_INDEX_COUNT]  = {};
  uint32_t timestamp_valid_bits              = 0;
};

class Context {
  friend class Device;

public:
  Context();
  ~Context();
  void set_application_info(const VkApplicationInfo* app_info);
  bool create_instance(const char** instance_ext, uint32_t instance_ext_count);
  bool create_device(VkSurfaceKHR surface, const char** required_device_extensions,
                     unsigned num_required_device_extensions,
                     const VkPhysicalDeviceFeatures* required_features);

private:
  const VkApplicationInfo& get_application_info() const;
  bool find_gpu();
  bool find_proper_queue(uint32_t& family, uint32_t& index, VkQueueFlags required,
                         VkQueueFlags ignored, float priority);
  bool populate_queue_ci(std::vector<VkDeviceQueueCreateInfo>& queue_cis);
  void populate_dbg_messenger_ci(VkDebugUtilsMessengerCreateInfoEXT& create_info);

  VkInstance m_instance{VK_NULL_HANDLE};
  VkPhysicalDevice m_gpu{VK_NULL_HANDLE};
  VkDevice m_device{VK_NULL_HANDLE};
  VkSurfaceKHR m_surface{VK_NULL_HANDLE};
  VkPhysicalDeviceProperties m_gpu_props;
  VkPhysicalDeviceMemoryProperties m_mem_props;
#ifdef ZEN_DEBUG
  VkDebugUtilsMessengerEXT m_debug_messenger{VK_NULL_HANDLE};
#endif
  const VkApplicationInfo* m_app_info{nullptr};
  uint32_t m_queue_family_count{0};
  std::vector<VkQueueFamilyProperties> m_queue_props;
  std::vector<uint32_t> m_queue_offsets;
  std::vector<std::vector<float>> m_queue_priorities;
  QueueInfo m_q_info{};
  DeviceFeatures m_feature;
};
}  // namespace zen::vkh
#endif  //ZENENGINE_CONTEXT_HPP
