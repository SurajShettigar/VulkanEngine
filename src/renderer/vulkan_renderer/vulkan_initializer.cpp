#include "vulkan_initializer.h"

/**
 * @brief Find if all the required extensions are present in the available list
 * of vulkan extensions
 *
 * @param availExtensions vector of Vulkan Extension Properties
 * @param reqExtensions vector of required extensions
 * @return true if all required extensions are present
 * @return false if some or none of the requrired extensions are present
 */
bool hasRequiredExtensions(const vector<vk::ExtensionProperties> &availExtensions,
                           const vector<const char *> &reqExtensions)
{
    // For each required extension, check if it exists in the list of vulkan extensions
    auto missingExtensions = std::find_if(reqExtensions.begin(),
                                          reqExtensions.end(),
                                          [&availExtensions](const char *reqE)
                                          {
                                              return std::find_if(availExtensions.begin(),
                                                                  availExtensions.end(),
                                                                  [&reqE](const vk::ExtensionProperties &e)
                                                                  {
                                                                      return strcmp(reqE, e.extensionName.data());
                                                                  }) == availExtensions.end();
                                          });

    // If missing extensions iterator has reached the end, it found all the
    // required extensions, otherwise there are some missing extensions
    return missingExtensions == reqExtensions.end();
}

/**
 * @brief Find if Vulkan has all the required instance extensions
 *
 * @param reqExtensions vector of required extensions
 * @return true if all required extensions are present
 * @return false if some or none of the requrired extensions are present
 */
inline bool hasRequriredInstanceExtensions(const vector<const char *> &reqExtensions)
{
    return hasRequiredExtensions(vk::enumerateInstanceExtensionProperties(), reqExtensions);
}

/**
 * @brief Find if the given physical device has the required extensions
 *
 * @param device Vulkan Physical Device
 * @param reqExtensions vector of required extensions
 * @return true if all required extensions are present
 * @return false if some or none of the requrired extensions are present
 */
inline bool hasRequiredDeviceExtensions(const vk::PhysicalDevice &device,
                                        const vector<const char *> &reqExtensions)
{
    return hasRequiredExtensions(device.enumerateDeviceExtensionProperties(), reqExtensions);
}

/**
 * @brief Find if vulkan supports the required validation layers
 *
 * @param validationLayers vector of validation layers required
 * @return true if all required layers are present
 * @return false if some or none of the layers are present
 */
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
    if (!hasRequriredInstanceExtensions(data.requiredExtensions))
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

bool engine::vulkan::destroyDebugMessenger(vk::Instance &instance,
                                           vk::DebugUtilsMessengerEXT &messenger)
{
    if (instance && messenger)
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
    return true;
}

vk::PhysicalDevice engine::vulkan::selectPhysicalDevice(const vk::Instance &instance,
                                                        const vk::SurfaceKHR &surface,
                                                        const vector<const char *> &reqExtensions = {})
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
        deviceScores.insert(std::make_pair(score, d));
    }

    // If the last device has a score of 0, then it failed to find any
    // suitable device
    if (deviceScores.rbegin()->first > 0)
        idealDevice = deviceScores.rbegin()->second;
    else
    {
        std::cerr << "Failed to find ideal GPU" << std::endl;
        return nullptr;
    }

    // Check if the device has the necessary queue families
    auto indices = getQueueFamilyIndices(idealDevice, surface);
    if (!(indices.isGraphicsSupported() && indices.isPresentationSupported()))
    {
        std::cerr << "Failed to find GPU with necessary Queue Families" << std::endl;
        return nullptr;
    }

    if (!hasRequiredDeviceExtensions(idealDevice, reqExtensions))
    {
        std::cerr << "Failed to find GPU with required extensions" << std::endl;
        return nullptr;
    }

    return idealDevice;
}

engine::vulkan::QueueFamilyIndices engine::vulkan::getQueueFamilyIndices(const vk::PhysicalDevice &physicalDevice,
                                                                         const vk::SurfaceKHR &surface)
{
    QueueFamilyIndices indices;

    auto queueFamilies = physicalDevice.getQueueFamilyProperties();
    uint32_t i = 0;
    for (const auto &q : queueFamilies)
    {
        if (q.queueFlags & vk::QueueFlagBits::eGraphics)
            indices.graphics = i;
    }

    vk::Bool32 isPresentSupported = physicalDevice.getSurfaceSupportKHR(indices.graphics, surface);
    if (isPresentSupported)
        indices.presentation = indices.graphics;

    return indices;
}

vk::Device engine::vulkan::getLogicalDevice(const vk::PhysicalDevice &physicalDevice,
                                            const QueueFamilyIndices &queueFamilyIndices,
                                            const vector<const char *> &validationLayers,
                                            const vector<const char *> &reqExtensions)
{
    std::set<uint32_t> indices = queueFamilyIndices.getIndices();

    vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    float priority = 1.0f;
    for (auto i : indices)
    {
        vk::DeviceQueueCreateInfo createInfo(vk::DeviceQueueCreateFlags(), i, 1, &priority);
        queueCreateInfos.push_back(createInfo);
    }

    vk::PhysicalDeviceFeatures deviceFeatures{};
    vk::DeviceCreateInfo createInfo(vk::DeviceCreateFlags(),
                                    queueCreateInfos,
                                    validationLayers,
                                    reqExtensions,
                                    &deviceFeatures);
    vk::Device device;
    try
    {
        device = physicalDevice.createDevice(createInfo);
    }
    catch (vk::SystemError &err)
    {
        std::cout << "Vulkan Excception: " << err.what() << std::endl;
    }
    catch (std::exception &err)
    {
        std::cout << "Exception: " << err.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown error when creating vulkan instance";
    }

    return device;
}

engine::vulkan::SwapchainData engine::vulkan::createSwapchain(const vk::Device &device,
                                                              const SwapchainInitData &data)
{
    return SwapchainData{};
}