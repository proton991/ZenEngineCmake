###########################
#         glfw            #
###########################
add_library(glfw INTERFACE)
set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.3.8)
target_include_directories(glfw INTERFACE ${GLFW_DIR}/include)
target_link_directories(glfw INTERFACE ${GLFW_DIR}/lib-vc2022)
target_link_libraries(glfw INTERFACE glfw3)

###########################
#       glm               #
###########################
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE glm)

###########################
#         spdlog          #
###########################
add_library(spdlog INTERFACE)
target_include_directories(spdlog INTERFACE spdlog-1.11.0/include)

###########################
#        tinygltf         #
###########################
add_library(tinygltf STATIC)
target_sources(tinygltf PRIVATE
        tinygltf/tiny_gltf.h
        tinygltf/tiny_gltf.cc
        tinygltf/json.hpp
        tinygltf/stb_image.h
        tinygltf/stb_image_write.h
        )
target_include_directories(tinygltf PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/tinygltf")

###########################
#       vma               #
###########################
add_library(vma STATIC)
target_sources(vma PRIVATE
    vma/vk_mem_alloc.h
    vma/vk_mem_alloc.cpp
    )
target_include_directories(vma PRIVATE ${VULKAN_INCLUDE_DIR})
target_include_directories(vma PUBLIC vma)

###########################
#         volk            #
###########################
# volk must be STATIC.
add_library(volk STATIC volk/volk.c volk/volk.h)
if (WIN32)
  target_compile_definitions(volk PRIVATE VK_USE_PLATFORM_WIN32_KHR)
else ()
  target_link_libraries(volk PRIVATE dl)
endif ()
target_include_directories(volk PUBLIC volk ${VULKAN_INCLUDE_DIR})

###########################
#      spirv_reflect      #
###########################
add_library(spirv_reflect STATIC)
target_sources(spirv_reflect PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/spirv_reflect/spirv_reflect.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/spirv_reflect/spirv_reflect.c"
    )
target_include_directories(spirv_reflect PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/spirv_reflect" )
target_include_directories(spirv_reflect PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/spirv_reflect/include" )