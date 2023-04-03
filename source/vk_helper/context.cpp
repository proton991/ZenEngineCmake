#include "context.hpp"
#include "debug.hpp"
#include "logging.hpp"

namespace zen::vkh {
#ifdef ZEN_DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL
vulkan_messenger_cb(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
  if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT ||
      messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
    switch (messageSeverity) {
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        logger::error("{}", pCallbackData->pMessage);
        break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        logger::warn("{}", pCallbackData->pMessage);
        break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        logger::info("{}", pCallbackData->pMessage);
        break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        logger::trace("{}", pCallbackData->pMessage);
        break;
      default:
        return VK_FALSE;
    }
  }

  bool log_object_names = false;
  for (uint32_t i = 0; i < pCallbackData->objectCount; i++) {
    auto* name = pCallbackData->pObjects[i].pObjectName;
    if (name) {
      log_object_names = true;
      break;
    }
  }

  if (log_object_names) {
    logger::set_list_pattern();
    for (uint32_t i = 0; i < pCallbackData->objectCount; i++) {
      auto* name = pCallbackData->pObjects[i].pObjectName;
      logger::error("  Object {}: {}", i, name ? name : "N/A");
    }
    logger::set_default_pattern();
  }
  return VK_FALSE;
}
#endif

Context::Context() {
  if (volkInitialize() != VK_SUCCESS) {
    logger::error("Failed to initialize through volk!");
  } else {
    logger::info("Initialized through volk successfully!");
  }
}

Context::~Context() {
  if (m_debug_messenger != VK_NULL_HANDLE) {
    vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
  }
  if (m_instance != VK_NULL_HANDLE) {
    vkDestroyInstance(m_instance, nullptr);
  }
}

const VkApplicationInfo& Context::get_application_info() const {
  static const VkApplicationInfo info_11 = {
      VK_STRUCTURE_TYPE_APPLICATION_INFO,
      nullptr,
      "ZenEngine",
      0,
      "ZenEngine",
      0,
      VK_API_VERSION_1_1,
  };
  return m_app_info ? *m_app_info : info_11;
}

void Context::set_application_info(const VkApplicationInfo* app_info) {
  m_app_info = app_info;
  VK_ASSERT(!app_info || app_info->apiVersion >= VK_API_VERSION_1_1);
}

bool Context::create_instance(const char** instance_ext, uint32_t instance_ext_count) {
  uint32_t target_instance_version = m_app_info ? m_app_info->apiVersion : VK_API_VERSION_1_1;
  if (volkGetInstanceVersion() < target_instance_version) {
    logger::error("Vulkan loader does not support target Vulkan version.");
    return false;
  }

  VkInstanceCreateInfo instance_ci{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
  instance_ci.pApplicationInfo = &get_application_info();

  std::vector<const char*> instance_exts;
  std::vector<const char*> instance_layers;
  for (uint32_t i = 0; i < instance_ext_count; i++)
    instance_exts.push_back(instance_ext[i]);

  uint32_t ext_count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &ext_count, nullptr);
  std::vector<VkExtensionProperties> queried_extensions(ext_count);
  if (ext_count)
    vkEnumerateInstanceExtensionProperties(nullptr, &ext_count, queried_extensions.data());

  uint32_t layer_count = 0;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  std::vector<VkLayerProperties> queried_layers(layer_count);
  if (layer_count)
    vkEnumerateInstanceLayerProperties(&layer_count, queried_layers.data());

  logger::info("Instance layer count: {}", layer_count);
  logger::set_list_pattern();
  for (auto& layer : queried_layers)
    logger::info("Found instance layer: {}", layer.layerName);
  logger::set_default_pattern();
  const auto has_extension = [&](const char* name) -> bool {
    auto itr = find_if(begin(queried_extensions), end(queried_extensions),
                       [name](const VkExtensionProperties& e) -> bool {
                         return strcmp(e.extensionName, name) == 0;
                       });
    return itr != end(queried_extensions);
  };

  for (uint32_t i = 0; i < instance_ext_count; i++)
    if (!has_extension(instance_ext[i]))
      return false;

  if (has_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
    instance_exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    m_feature.supports_debug_utils = true;
  }

  auto itr = std::find_if(instance_ext, instance_ext + instance_ext_count, [](const char* name) {
    return strcmp(name, VK_KHR_SURFACE_EXTENSION_NAME) == 0;
  });
  bool has_surface_extension = itr != (instance_ext + instance_ext_count);

  if (has_surface_extension && has_extension(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME)) {
    instance_exts.push_back(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);
    m_feature.supports_surface_capabilities2 = true;
  }

  if (has_surface_extension && has_extension(VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME)) {
    instance_exts.push_back(VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME);
    m_feature.supports_swapchain_colorspace = true;
  }

#ifdef ZEN_DEBUG
  const auto has_layer = [&](const char* name) -> bool {
    auto layer_itr = find_if(begin(queried_layers), end(queried_layers),
                             [name](const VkLayerProperties& e) -> bool {
                               return strcmp(e.layerName, name) == 0;
                             });
    return layer_itr != end(queried_layers);
  };

  VkValidationFeaturesEXT validation_features{VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT};
  VkDebugUtilsMessengerCreateInfoEXT inst_debug_info{};
  populate_dbg_messenger_ci(inst_debug_info);
  validation_features.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&inst_debug_info;
  if (has_layer("VK_LAYER_KHRONOS_validation")) {
    instance_layers.push_back("VK_LAYER_KHRONOS_validation");
    logger::info("Enabling VK_LAYER_KHRONOS_validation.");

    uint32_t layer_ext_count = 0;
    vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_validation", &layer_ext_count,
                                           nullptr);
    std::vector<VkExtensionProperties> layer_exts(layer_ext_count);
    vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_validation", &layer_ext_count,
                                           layer_exts.data());

    if (find_if(begin(layer_exts), end(layer_exts), [](const VkExtensionProperties& e) {
          return strcmp(e.extensionName, VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME) == 0;
        }) != end(layer_exts)) {
      instance_exts.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
      static const VkValidationFeatureEnableEXT validation_sync_features[1] = {
          VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
      };
      logger::info("Enabling VK_EXT_validation_features for synchronization validation.");
      validation_features.enabledValidationFeatureCount = 1;
      validation_features.pEnabledValidationFeatures    = validation_sync_features;
      instance_ci.pNext                                 = &validation_features;
    }

    if (!m_feature.supports_debug_utils &&
        find_if(begin(layer_exts), end(layer_exts), [](const VkExtensionProperties& e) {
          return strcmp(e.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0;
        }) != end(layer_exts)) {
      instance_exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
      m_feature.supports_debug_utils = true;
    }
  }
#endif

  instance_ci.enabledExtensionCount   = instance_exts.size();
  instance_ci.ppEnabledExtensionNames = instance_exts.empty() ? nullptr : instance_exts.data();
  instance_ci.enabledLayerCount       = instance_layers.size();
  instance_ci.ppEnabledLayerNames     = instance_layers.empty() ? nullptr : instance_layers.data();

  for (auto* ext_name : instance_exts)
    logger::info("Enabling instance extension: {}", ext_name);

  if (vkCreateInstance(&instance_ci, nullptr, &m_instance) != VK_SUCCESS) {
    return false;
  }

  volkLoadInstance(m_instance);

#if defined(ZEN_DEBUG) && !defined(ANDROID)
  if (m_feature.supports_debug_utils) {
    VkDebugUtilsMessengerCreateInfoEXT debug_info{};
    populate_dbg_messenger_ci(debug_info);
    // For some reason, this segfaults Android, sigh ... We get relevant output in logcat anyways.
    if (vkCreateDebugUtilsMessengerEXT) {
      if (vkCreateDebugUtilsMessengerEXT(m_instance, &debug_info, nullptr, &m_debug_messenger) !=
          VK_SUCCESS) {
        logger::error("Failed to create debug utils messenger ext!");
      }
    }
  }
#endif
  return true;
}

bool Context::create_device(VkSurfaceKHR surface, const char** required_device_extensions,
                            unsigned int num_required_device_extensions,
                            const VkPhysicalDeviceFeatures* required_features) {
  VK_ASSERT(m_instance);
  m_surface = surface;
  // Find physical device
  if (!find_gpu()) {
    logger::error("Could not find gpu supporting Vulkan!");
    return false;
  }
  // Check extension support
  uint32_t ext_count = 0;
  vkEnumerateDeviceExtensionProperties(m_gpu, nullptr, &ext_count, nullptr);
  std::vector<VkExtensionProperties> queried_extensions(ext_count);
  if (ext_count)
    vkEnumerateDeviceExtensionProperties(m_gpu, nullptr, &ext_count, queried_extensions.data());

  const auto has_extension = [&](const char* name) -> bool {
    auto itr = find_if(begin(queried_extensions), end(queried_extensions),
                       [name](const VkExtensionProperties& e) -> bool {
                         return strcmp(e.extensionName, name) == 0;
                       });
    return itr != end(queried_extensions);
  };
  std::vector<const char*> enabled_extensions;
  for (uint32_t i = 0; i < num_required_device_extensions; i++) {
    if (!has_extension(required_device_extensions[i])) {
      return false;
    }
    enabled_extensions.push_back(required_device_extensions[i]);
  }
  // Create logical device
  VkDeviceCreateInfo device_ci = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
  std::vector<VkDeviceQueueCreateInfo> queue_cis;
  populate_queue_ci(queue_cis);
  device_ci.pQueueCreateInfos     = queue_cis.data();
  device_ci.queueCreateInfoCount  = uint32_t(queue_cis.size());
  device_ci.enabledExtensionCount = enabled_extensions.size();
  device_ci.ppEnabledExtensionNames =
      enabled_extensions.empty() ? nullptr : enabled_extensions.data();
  if (vkCreateDevice(m_gpu, &device_ci, nullptr, &m_device) != VK_SUCCESS) {
    return false;
  }
  logger::info("Enabled device extensions:");
  logger::LogList(enabled_extensions);
  volkLoadDevice(m_device);
  for (int i = 0; i < QUEUE_INDEX_COUNT; i++) {
    if (m_q_info.family_indices[i] != VK_QUEUE_FAMILY_IGNORED) {
      vkGetDeviceQueue(m_device, m_q_info.family_indices[i], m_q_info.queue_indices[i],
                       &m_q_info.queues[i]);
    } else {
      m_q_info.queues[i] = VK_NULL_HANDLE;
    }
  }
  DebugUtil::get().set_obj_name(m_device, "device");
}

bool Context::find_proper_queue(uint32_t& family, uint32_t& index, VkQueueFlags required,
                                VkQueueFlags ignored, float priority) {
  for (unsigned family_index = 0; family_index < m_queue_family_count; family_index++) {
    if (m_queue_props[family_index].queueFlags & ignored)
      continue;

    // A graphics queue candidate must support present for us to select it.
    if ((required & VK_QUEUE_GRAPHICS_BIT) && m_surface) {
      VkBool32 supported = VK_FALSE;
      VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(m_gpu, family_index, m_surface, &supported),
               "vkGetPhysicalDeviceSurfaceSupportKHR");
      if (!supported) {
        continue;
      }
    }

    if (m_queue_props[family_index].queueCount &&
        (m_queue_props[family_index].queueFlags & required) == required) {
      family = family_index;
      m_queue_props[family_index].queueCount--;
      index = m_queue_offsets[family_index]++;
      m_queue_priorities[family_index].push_back(priority);
      return true;
    }
  }
  return false;
}

bool Context::find_gpu() {
  uint32_t gpu_count = 0;
  if (vkEnumeratePhysicalDevices(m_instance, &gpu_count, nullptr) != VK_SUCCESS)
    return false;

  if (gpu_count == 0)
    return false;

  std::vector<VkPhysicalDevice> gpus(gpu_count);
  if (vkEnumeratePhysicalDevices(m_instance, &gpu_count, gpus.data()) != VK_SUCCESS)
    return false;

  for (auto& gpu : gpus) {
    vkGetPhysicalDeviceProperties(gpu, &m_gpu_props);
    logger::info("Found Vulkan GPU: {}", m_gpu_props.deviceName);
    logger::set_list_pattern();
    logger::info("GPU API: {}.{}.{}", VK_VERSION_MAJOR(m_gpu_props.apiVersion),
                 VK_VERSION_MINOR(m_gpu_props.apiVersion),
                 VK_VERSION_PATCH(m_gpu_props.apiVersion));
    logger::info("GPU Driver: {}.{}.{}", VK_VERSION_MAJOR(m_gpu_props.driverVersion),
                 VK_VERSION_MINOR(m_gpu_props.driverVersion),
                 VK_VERSION_PATCH(m_gpu_props.driverVersion));
    logger::set_default_pattern();
    if (m_gpu_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
        m_gpu_props.apiVersion >= VK_API_VERSION_1_1) {
      m_gpu = gpu;
    }
  }
  if (m_gpu == VK_NULL_HANDLE) {
    return false;
  } else {
    logger::info("Using GPU: {}", m_gpu_props.deviceName);
  }
  return true;
}

bool Context::populate_queue_ci(std::vector<VkDeviceQueueCreateInfo>& queue_cis) {
  vkGetPhysicalDeviceQueueFamilyProperties(m_gpu, &m_queue_family_count, nullptr);
  if (m_queue_family_count <= 0) {
    return false;
  }
  m_queue_props.resize(m_queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(m_gpu, &m_queue_family_count, m_queue_props.data());
  m_queue_family_count = m_queue_props.size();
  m_queue_offsets.resize(m_queue_family_count);
  m_queue_priorities.resize(m_queue_family_count);
  if (!find_proper_queue(m_q_info.family_indices[QUEUE_INDEX_GRAPHICS],  // family index
                         m_q_info.queue_indices[QUEUE_INDEX_GRAPHICS],   // queue index
                         VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT,   // required flags
                         0,                                              // ignore flags
                         0.5f                                            // priority
                         )) {
    logger::error("Failed to find graphics queue!");
    return false;
  }
  // XXX: This assumes timestamp valid bits is the same for all queue types.
  m_q_info.timestamp_valid_bits =
      m_queue_props[m_q_info.family_indices[QUEUE_INDEX_GRAPHICS]].timestampValidBits;
  // Prefer another graphics queue since we can do async graphics that way.
  // The compute queue is to be treated as high priority since we also do async graphics on it.
  if (!find_proper_queue(m_q_info.family_indices[QUEUE_INDEX_COMPUTE],
                         m_q_info.queue_indices[QUEUE_INDEX_COMPUTE],
                         VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, 0, 1.0f) &&
      !find_proper_queue(m_q_info.family_indices[QUEUE_INDEX_COMPUTE],
                         m_q_info.queue_indices[QUEUE_INDEX_COMPUTE], VK_QUEUE_COMPUTE_BIT, 0,
                         1.0f)) {
    // Fallback to the graphics queue if we must.
    m_q_info.family_indices[QUEUE_INDEX_COMPUTE] = m_q_info.family_indices[QUEUE_INDEX_GRAPHICS];
    m_q_info.queue_indices[QUEUE_INDEX_COMPUTE]  = m_q_info.queue_indices[QUEUE_INDEX_GRAPHICS];
  }

  // For transfer, try to find a queue which only supports transfer, e.g. DMA queue.
  // If not, fallback to a dedicated compute queue.
  // Finally, fallback to same queue as compute.
  if (!find_proper_queue(m_q_info.family_indices[QUEUE_INDEX_TRANSFER],
                         m_q_info.queue_indices[QUEUE_INDEX_TRANSFER], VK_QUEUE_TRANSFER_BIT,
                         VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, 0.5f) &&
      !find_proper_queue(m_q_info.family_indices[QUEUE_INDEX_TRANSFER],
                         m_q_info.queue_indices[QUEUE_INDEX_TRANSFER], VK_QUEUE_COMPUTE_BIT,
                         VK_QUEUE_GRAPHICS_BIT, 0.5f)) {
    m_q_info.family_indices[QUEUE_INDEX_TRANSFER] = m_q_info.family_indices[QUEUE_INDEX_COMPUTE];
    m_q_info.queue_indices[QUEUE_INDEX_TRANSFER]  = m_q_info.queue_indices[QUEUE_INDEX_COMPUTE];
  }
  for (auto family_index = 0u; family_index < m_queue_family_count; family_index++) {
    if (m_queue_offsets[family_index] == 0) {
      continue;
    }
    VkDeviceQueueCreateInfo queue_ci{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    queue_ci.queueFamilyIndex = family_index;
    queue_ci.queueCount       = m_queue_offsets[family_index];
    queue_ci.pQueuePriorities = m_queue_priorities[family_index].data();
    queue_cis.push_back(queue_ci);
  }
  return true;
}

void Context::populate_dbg_messenger_ci(VkDebugUtilsMessengerCreateInfoEXT& create_info) {
  create_info.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
  create_info.pfnUserCallback = vulkan_messenger_cb;
  create_info.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
  create_info.pUserData = this;
}
}  // namespace zen::vkh