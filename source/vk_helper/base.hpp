#ifndef ZENENGINE_BASE_HPP
#define ZENENGINE_BASE_HPP
#define VK_NO_PROTOTYPES
#include "vk_mem_alloc.h"
#include <volk.h>
#include <GLFW/glfw3.h>

#define VULKAN_NON_COPIABLE(ClassName)             \
  ClassName(const ClassName&)            = delete; \
  ClassName(ClassName&&)                 = delete; \
  ClassName& operator=(const ClassName&) = delete; \
  ClassName& operator=(ClassName&&)      = delete;

#define NO_COPY(ClassName)                         \
  ClassName(const ClassName&)            = delete; \
  ClassName& operator=(const ClassName&) = delete;

#define NO_MOVE(ClassName)                    \
  ClassName(ClassName&&)            = delete; \
  ClassName& operator=(ClassName&&) = delete;
#endif  //ZENENGINE_BASE_HPP
