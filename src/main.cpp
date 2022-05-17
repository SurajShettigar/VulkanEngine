#include <iostream>
#include <string>

#include <main.h>
#include <vulkan_renderer.h>
#include <window.h>

using engine::Window;
using engine::vulkan::VulkanRenderer;

int main(int argc, char **argv)
{
  const int VERSION[3] = {APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH};
  Window window(APP_NAME);
  VulkanRenderer renderer(window, APP_NAME, VERSION, false);
  renderer.run();
  return 0;
}