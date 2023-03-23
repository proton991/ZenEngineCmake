cmake_minimum_required(VERSION 3.23)
project(ZenEngine VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(VULKAN_INCLUDE_DIR $ENV{VULKAN_SDK}/Include)
find_package(Vulkan REQUIRED)
message(STATUS "Vulkan include directory: ${VULKAN_INCLUDE_DIR}")
message(STATUS "Vulkan lib directory: ${Vulkan_LIBRARIES}")
find_program(GLSL_VALIDATOR glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)
if (NOT GLSL_VALIDATOR)
  message(FATAL_ERROR "glslangValidator not found!")
else ()
  message(STATUS "Found glslangValidator: ${GLSL_VALIDATOR}")
endif ()
############################################################
#                    Add Sub Directories                   #
############################################################
add_subdirectory(source)
add_subdirectory(samples)
add_subdirectory(external)



############################################################
#                    Build Shaders                         #
############################################################
set(ZEN_SHADER_SRC_PATH ${PROJECT_SOURCE_DIR}/data/shaders)
set(ZEN_SHADER_SPV_PATH ${PROJECT_SOURCE_DIR}/data/spv_shaders)
file(GLOB ZEN_SHADER_FILES
    "${ZEN_SHADER_SRC_PATH}/*.frag"
    "${ZEN_SHADER_SRC_PATH}/*.vert"
    "${ZEN_SHADER_SRC_PATH}/*.comp"
    )

message(STATUS "Zen shader path: ${ZEN_SHADER_SPV_PATH}")

foreach (GLSL ${ZEN_SHADER_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${ZEN_SHADER_SPV_PATH}/${FILE_NAME}.spv")
  message(STATUS "Compiling shader ${GLSL} -> ${SPIRV}")
  add_custom_command(
      OUTPUT ${SPIRV}
      COMMAND ${GLSL_VALIDATOR} --target-env spirv1.3 -V ${GLSL} -o ${SPIRV}
      DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach (GLSL)

add_custom_target(
    zen_shaders
    DEPENDS ${SPIRV_BINARY_FILES}
    SOURCES ${ZEN_SHADER_FILES}
)

## Add a custom command to delete the output file before building the shader target
#add_custom_command(
#    TARGET zen_shaders
#    PRE_BUILD
#    COMMAND ${CMAKE_COMMAND} -E remove ${SPIRV_BINARY_FILES}
#    COMMENT "Deleting shader output file"
#)


add_dependencies(zen_engine zen_shaders)
target_compile_definitions(zen_engine PUBLIC ZEN_SHADER_PATH="${ZEN_SHADER_SPV_PATH}")
