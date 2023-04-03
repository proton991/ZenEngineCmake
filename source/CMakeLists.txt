file(GLOB_RECURSE ZEN_ENGINE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
add_library(zen_engine STATIC)

target_sources(zen_engine PRIVATE ${ZEN_ENGINE_SRC})
target_compile_definitions(zen_engine PUBLIC ZEN_DEBUG)
target_include_directories(zen_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(zen_engine PUBLIC volk glfw tinygltf spdlog glm vma spirv_reflect)
#target_link_libraries(zen_engine glfw tinygltf spdlog glm vma)