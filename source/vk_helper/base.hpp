#ifndef ZENENGINE_BASE_HPP
#define ZENENGINE_BASE_HPP
#define VK_NO_PROTOTYPES
#include "vk_mem_alloc.h"
#include <volk.h>

#define ZEN_NON_COPIABLE(ClassName)             \
  ClassName(const ClassName&)            = delete; \
  ClassName(ClassName&&)                 = delete; \
  ClassName& operator=(const ClassName&) = delete; \
  ClassName& operator=(ClassName&&)      = delete;

#define ZEN_NO_COPY(ClassName)                         \
  ClassName(const ClassName&)            = delete; \
  ClassName& operator=(const ClassName&) = delete;

#define ZEN_NO_MOVE(ClassName)                    \
  ClassName(ClassName&&)            = delete; \
  ClassName& operator=(ClassName&&) = delete;
#endif  //ZENENGINE_BASE_HPP
