#include "vulkan_initializer.h"

bool hasRequriredExtensions(const vector<const char *> &reqExtensions)
{
    // Search if Vulkan has all the required extensions
    vector<vk::ExtensionProperties> vExtenstions = vk::enumerateInstanceExtensionProperties();
    // For each required extension, check if it exists in the list of vulkan extensions
    auto missingExtensions = std::find_if(reqExtensions.begin(),
                                          reqExtensions.end(),
                                          [&vExtenstions](const char *reqE)
                                          {
                                              return std::find_if(vExtenstions.begin(),
                                                                  vExtenstions.end(),
                                                                  [&reqE](const vk::ExtensionProperties &e)
                                                                  {
                                                                      return strcmp(reqE, e.extensionName.data());
                                                                  }) == vExtenstions.end();
                                          });

    // If missing extensions iterator has reached the end, it found all the
    // required extensions, otherwise there are some missing extensions
    return missingExtensions == reqExtensions.end();
}

bool hasRequiredValidationLayers(const vector<const char *> validationLayers)
{
    vector<vk::LayerProperties> layerProps = vk::enumerateInstanceLayerProperties();

    auto missingLayers = std::find_if(validationLayers.begin(),
                                      validationLayers.end(),
                                      [&layerProps](const char *vLayer)
                                      {
                                          return std::find_if(layerProps.begin(),
                                                              layerProps.end(),
                                                              [&vLayer](const vk::LayerProperties &l)
                                                              {
                                                                  return strcmp(vLayer, l.layerName);
                                                              }) == layerProps.end();
                                      });

    return missingLayers == validationLayers.end();
}

bool engine::vulkan::createInstance(const InstanceCreateData &data,
                                    vk::Instance &instance,
                                    vk::DebugUtilsMessengerEXT &debugMessenger)
{
    // Check for extensions support
    if (!hasRequriredExtensions(data.requiredExtensions))
    {
        std::cerr << "Vulkan does not have required extensions" << std::endl;
        return false;
    }

    // Set vulkan layers
    if (data.enableValidationLayers && !hasRequiredValidationLayers(data.validationLayers))
    {
        std::cerr << "Requested validation layers are not available" << std::endl;
        return false;
    }

    vk::InstanceCreateInfo createInfo({},
                                      &data.appInfo,
                                      {},
                                      data.requiredExtensions);
    if (data.enableValidationLayers)
        createInfo.setPEnabledLayerNames(data.validationLayers);

    auto msgSeverity = /* vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | */ vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
    auto msgTypes = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;

    auto debugMessengerCreateInfo = vk::DebugUtilsMessengerCreateInfoEXT({},
                                                                         msgSeverity,
                                                                         msgTypes,
                                                                         engine::vulkan::debugCallback);
    if (data.enableValidationLayers)
        createInfo.pNext = static_cast<vk::DebugUtilsMessengerCreateInfoEXT *>(&debugMessengerCreateInfo);

    try
    {
        instance = vk::createInstance(createInfo);
        if (data.enableValidationLayers)
        {
            auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
            auto instanceFunc = reinterpret_cast<PFN_vkGetInstanceProcAddr>(instance.getProcAddr("vkGetInstanceProcAddr"));
            if (func == nullptr || instanceFunc == nullptr)
            {
                std::cout << "Failed to create debug messenger. Unable to Find vkCreateDebugUtilsMessengerEXT function" << std::endl;
                return false;
            }
            else
            {
                debugMessenger = instance.createDebugUtilsMessengerEXT(debugMessengerCreateInfo,
                                                                       nullptr,
                                                                       vk::DispatchLoaderDynamic(instance, instanceFunc));
            }
        }
    }
    catch (vk::SystemError &err)
    {
        std::cout << "Vulkan Excception: " << err.what() << std::endl;
        return false;
    }
    catch (std::exception &err)
    {
        std::cout << "Exception: " << err.what() << std::endl;
        return false;
    }
    catch (...)
    {
        std::cerr << "Unknown error when creating vulkan instance";
        return false;
    }

    return true;
}

bool engine::vulkan::destroyInstance(vk::Instance &instance,
                                     vk::DebugUtilsMessengerEXT &messenger)
{
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
    auto instanceFunc = reinterpret_cast<PFN_vkGetInstanceProcAddr>(instance.getProcAddr("vkGetInstanceProcAddr"));
    if (func == nullptr || instanceFunc == nullptr)
    {
        std::cout << "Failed to destroy debug messenger. Unable to Find vkDestroyDebugUtilsMessengerEXT function" << std::endl;
        return false;
    }
    else
    {
        try
        {
            instance.destroyDebugUtilsMessengerEXT(messenger,
                                                   nullptr,
                                                   vk::DispatchLoaderDynamic(instance, instanceFunc));

            instance.destroy();
        }
        catch (vk::SystemError &err)
        {
            std::cout << "Vulkan Excception: " << err.what() << std::endl;
            return false;
        }
        catch (std::exception &err)
        {
            std::cout << "Exception: " << err.what() << std::endl;
            return false;
        }
        catch (...)
        {
            std::cerr << "Unknown error when creating vulkan instance";
            return false;
        }
        return true;
    }
}

vk::PhysicalDevice engine::vulkan::selectPhysicalDevice(const vk::Instance &instance)
{
    vk::PhysicalDevice idealDevice = nullptr;
    vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();

    // Go through all the devices and rate them based on thier properties
    // or features. The devices are stored in a multi-map in ascending order
    // of their score
    std::multimap<int, vk::PhysicalDevice> deviceScores;
    int score = 0;
    for (const vk::PhysicalDevice &d : devices)
    {
        if (!d.getFeatures().geometryShader)
        {
            score = 0;
            deviceScores.insert(std::make_pair(score, d));
            continue;
        }

        if (d.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            score += 1000;
        score += d.getProperties().limits.maxImageDimension2D;
    }

    // If the last device has a score of 0, then it failed to find any
    // suitable device
    if (deviceScores.rbegin()->first > 0)
        idealDevice = deviceScores.rbegin()->second;
    else
        return nullptr;

    auto queueFamilies = idealDevice.getQueueFamilyProperties();

    int i = 0;
    for (const auto &q : queueFamilies)
    {
        if (q.queueFlags & vk::QueueFlagBits::eGraphics)
            break;
        i++;
    }
    // If selected device does not have graphics queue family,
    // the device is not suitable
    if (i == queueFamilies.size())
        return nullptr;

    return idealDevice;
}

engine::vulkan::QueueFamilyIndices engine::vulkan::getQueueFamilyIndices(const vk::PhysicalDevice &physicalDevice)
{
    QueueFamilyIndices indices;
    return indices;
}

vk::Device engine::vulkan::getLogicalDevice(const vk::PhysicalDevice &physicalDevice)
{
    vk::Device device;

    return device;
}