file(GLOB RENDERER_SOURCE *.cpp)
add_library(renderer ${RENDERER_SOURCE})
target_include_directories(renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(renderer vulkan glfw)