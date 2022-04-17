add_subdirectory(renderer)

configure_file(main.h.in main.h)

add_executable (VulkanEngine "main.cpp")

target_include_directories(VulkanEngine PUBLIC 
                           ${PROJECT_BINARY_DIR}/src)

target_link_libraries(VulkanEngine 
                      PRIVATE renderer)

