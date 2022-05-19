cmake_minimum_required(VERSION 3.8.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(VulkanEngine VERSION 0.1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(external)
add_subdirectory(src)

add_custom_target(VULKAN_ENGINE ALL DEPENDS VulkanEngine)
add_custom_command(TARGET VULKAN_ENGINE POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${GLFW_DLL}
                    $<TARGET_FILE_DIR:VulkanEngine>)