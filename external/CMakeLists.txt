# Vulkan
add_library(vulkan INTERFACE)

cmake_path(CONVERT $ENV{VULKAN_SDK}/Include TO_CMAKE_PATH_LIST VULKAN_SDK_INCLUDE_DIR)
cmake_path(CONVERT $ENV{VULKAN_SDK}/Lib TO_CMAKE_PATH_LIST VULKAN_SDK_LIB_DIR)

target_include_directories(vulkan INTERFACE ${VULKAN_SDK_INCLUDE_DIR})
target_link_directories(vulkan INTERFACE ${VULKAN_SDK_LIB_DIR})    
target_link_libraries(vulkan INTERFACE ${VULKAN_SDK_LIB_DIR}/vulkan-1.lib)

# GLFW
add_library(glfw INTERFACE)

set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glfw)
set(GLFW_DLL  ${GLFW_DIR}/lib-vc2019/glfw3.dll PARENT_SCOPE)

target_include_directories(glfw INTERFACE ${GLFW_DIR}/include)
target_link_directories(glfw INTERFACE ${GLFW_DIR}/lib-vc2019)
target_link_libraries(glfw INTERFACE ${GLFW_DIR}/lib-vc2019/glfw3dll.lib)