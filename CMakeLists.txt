cmake_minimum_required(VERSION 3.8.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(VulkanEngine VERSION 0.1.0)

add_subdirectory(external)
add_subdirectory(src)
