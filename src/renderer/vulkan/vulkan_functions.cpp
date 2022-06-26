#include "vulkan_functions.h"

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif

/**
 * @brief Find if all the required extensions are present in the available list
 * of vulkan extensions
 *
 * @param availExtensions vector of Vulkan Extension Properties
 * @param reqExtensions vector of required extensions
 * @return true if all required extensions are present
 * @return false if some or none of the requrired extensions are present
 */
    bool hasRequiredExtensions(const vector<vk::ExtensionProperties>& availExtensions,
        const vector<const char*>& reqExtensions)
{
    // For each required extension, check if it exists in the list of vulkan extensions
    auto missingExtensions = std::find_if(reqExtensions.begin(),
        reqExtensions.end(),
        [&availExtensions](const char* reqE)
        {
            return std::find_if(availExtensions.begin(),
                availExtensions.end(),
                [&reqE](const vk::ExtensionProperties& e)
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
inline bool hasRequriredInstanceExtensions(const vector<const char*>& reqExtensions)
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
inline bool hasRequiredDeviceExtensions(const vk::PhysicalDevice& device,
    const vector<const char*>& reqExtensions)
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
bool hasRequiredValidationLayers(const vector<const char*> validationLayers)
{
    vector<vk::LayerProperties> layerProps = vk::enumerateInstanceLayerProperties();

    auto missingLayers = std::find_if(validationLayers.begin(),
        validationLayers.end(),
        [&layerProps](const char* vLayer)
        {
            return std::find_if(layerProps.begin(),
                layerProps.end(),
                [&vLayer](const vk::LayerProperties& l)
                {
                    return strcmp(vLayer, l.layerName);
                }) == layerProps.end();
        });

    return missingLayers == validationLayers.end();
}

bool engine::vulkan::createInstance(const InstanceCreateData& data,
    vk::Instance& instance,
    vk::DebugUtilsMessengerEXT& debugMessenger)
{
    // Setup dynamic dispatcher (Used for extension functions).
    // Without dynamic dispatcher program will crash when using extension functions.
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
    static vk::DynamicLoader dl;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
#endif

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

    // Create Vulkan Instance
    vk::InstanceCreateInfo createInfo({},
        &data.appInfo,
        data.validationLayers,
        data.requiredExtensions);
    try
    {
        instance = vk::createInstance(createInfo);
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
        VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
#endif

        if (data.enableValidationLayers)
        {
            auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
            if (func == nullptr)
            {
                std::cout << "Failed to create debug messenger. Unable to Find vkCreateDebugUtilsMessengerEXT function" << std::endl;
                return false;
            }
            else
            {
                auto msgSeverity = /* vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | */
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
                auto msgTypes = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                    vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                    vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;

                auto debugMessengerCreateInfo = vk::DebugUtilsMessengerCreateInfoEXT({},
                    msgSeverity,
                    msgTypes,
                    &engine::vulkan::debugCallback);

                debugMessenger = instance.createDebugUtilsMessengerEXT(debugMessengerCreateInfo, nullptr);
            }
        }
    }
    catch (vk::SystemError& err)
    {
        std::cout << "Vulkan Excception: " << err.what() << std::endl;
        return false;
    }
    catch (std::exception& err)
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

bool engine::vulkan::destroyDebugMessenger(vk::Instance& instance,
    vk::DebugUtilsMessengerEXT& messenger)
{
    if (instance && messenger)
    {
        auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
        if (func == nullptr)
        {
            std::cout << "Failed to destroy debug messenger. Unable to Find vkDestroyDebugUtilsMessengerEXT function" << std::endl;
            return false;
        }
        else
        {
            try
            {
                instance.destroyDebugUtilsMessengerEXT(messenger, nullptr);
            }
            catch (vk::SystemError& err)
            {
                std::cout << "Vulkan Excception: " << err.what() << std::endl;
                return false;
            }
            catch (std::exception& err)
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

engine::vulkan::QueueFamilyIndices engine::vulkan::getQueueFamilyIndices(const vk::PhysicalDevice& physicalDevice,
    const vk::SurfaceKHR& surface)
{
    QueueFamilyIndices indices;

    auto queueFamilies = physicalDevice.getQueueFamilyProperties();
    uint32_t i = 0;
    for (const auto& q : queueFamilies)
    {
        if (q.queueFlags & vk::QueueFlagBits::eGraphics)
            indices.graphics = i;
    }

    vk::Bool32 isPresentSupported = physicalDevice.getSurfaceSupportKHR(indices.graphics, surface);
    if (isPresentSupported)
        indices.presentation = indices.graphics;

    return indices;
}

engine::vulkan::SwapchainSupportInfo engine::vulkan::getSwapchainSupportInfo(const vk::PhysicalDevice& physicalDevice,
    const vk::SurfaceKHR& surface)
{
    SwapchainSupportInfo supportInfo;
    supportInfo.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    supportInfo.surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);
    supportInfo.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

    return supportInfo;
}

vk::PhysicalDevice engine::vulkan::selectPhysicalDevice(const vk::Instance& instance,
    const vk::SurfaceKHR& surface,
    const vector<const char*>& reqExtensions)
{
    vk::PhysicalDevice idealDevice = nullptr;
    vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();

    // Go through all the devices and rate them based on thier properties
    // or features. The devices are stored in a multi-map in ascending order
    // of their score
    std::multimap<int, vk::PhysicalDevice> deviceScores;
    int score = 0;
    for (const vk::PhysicalDevice& d : devices)
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

    // Check if device has required extensions
    if (!hasRequiredDeviceExtensions(idealDevice, reqExtensions))
    {
        std::cerr << "Failed to find GPU with required extensions" << std::endl;
        return nullptr;
    }

    // Check if device has swapchain capabilities
    if (!getSwapchainSupportInfo(idealDevice, surface).isSupported())
    {
        std::cerr << "Failed to find GPU with swapchain capabilities" << std::endl;
        return nullptr;
    }

    return idealDevice;
}

vk::Device engine::vulkan::getLogicalDevice(const vk::PhysicalDevice& physicalDevice,
    const QueueFamilyIndices& queueFamilyIndices,
    const vector<const char*>& validationLayers,
    const vector<const char*>& reqExtensions)
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
    vk::DeviceCreateInfo createInfo({},
        queueCreateInfos,
        validationLayers,
        reqExtensions);

    try
    {
        vk::Device device = physicalDevice.createDevice(createInfo);
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
        VULKAN_HPP_DEFAULT_DISPATCHER.init(device);
#endif
        return device;
    }
    catch (vk::SystemError& err)
    {
        std::cout << "Vulkan Excception: " << err.what() << std::endl;
    }
    catch (std::exception& err)
    {
        std::cout << "Exception: " << err.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown error when creating vulkan instance";
    }

    return nullptr;
}

engine::vulkan::SwapchainInitData engine::vulkan::selectSwapchainInitData(const SwapchainSupportInfo& supportInfo,
    const std::array<int, 2> windowResolution)
{
    SwapchainInitData initData;

    // Select surface resolution
    if (supportInfo.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        initData.extent = supportInfo.capabilities.currentExtent;
    else
    {
        uint32_t width = std::clamp(static_cast<uint32_t>(windowResolution[0]),
            supportInfo.capabilities.minImageExtent.width,
            supportInfo.capabilities.maxImageExtent.width);
        uint32_t height = std::clamp(static_cast<uint32_t>(windowResolution[1]),
            supportInfo.capabilities.minImageExtent.height,
            supportInfo.capabilities.maxImageExtent.height);
        initData.extent.setWidth(width);
        initData.extent.setHeight(height);
    }

    // Select image count
    initData.imageCount = supportInfo.capabilities.minImageCount + 1;
    if (supportInfo.capabilities.maxImageCount > 0
        && initData.imageCount > supportInfo.capabilities.maxImageCount)
        initData.imageCount = supportInfo.capabilities.maxImageCount;

    // Get current surface transform
    initData.transform = supportInfo.capabilities.currentTransform;

    // Select surface format
    auto format = std::find_if(supportInfo.surfaceFormats.begin(),
        supportInfo.surfaceFormats.end(),
        [](const vk::SurfaceFormatKHR& f)
        {
            return f.format == vk::Format::eB8G8R8A8Srgb
                && f.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
        });
    if (format != supportInfo.surfaceFormats.end())
        initData.surfaceFormat = *format;
    else
        initData.surfaceFormat = supportInfo.surfaceFormats[0];

    // Select present mode
    auto presentMode = std::find_if(supportInfo.presentModes.begin(),
        supportInfo.presentModes.end(),
        [](const vk::PresentModeKHR& p)
        {
            return p == vk::PresentModeKHR::eMailbox;
        });
    if (presentMode != supportInfo.presentModes.end())
        initData.presentMode = *presentMode;
    else
        initData.presentMode = vk::PresentModeKHR::eFifo;
    return initData;
}

engine::vulkan::SwapchainData engine::vulkan::createSwapchain(const vk::PhysicalDevice& physicalDevice,
    const vk::Device& device,
    const vk::SurfaceKHR& surface,
    const QueueFamilyIndices& queueFamilyIndices,
    const std::array<int, 2> windowResolution,
    const vk::SwapchainKHR& oldSwapchain)
{
    SwapchainSupportInfo supportInfo = getSwapchainSupportInfo(physicalDevice, surface);
    SwapchainInitData data = selectSwapchainInitData(supportInfo, windowResolution);

    vk::SharingMode sharingMode = queueFamilyIndices.isGraphicsAndPresentSame()
        ? vk::SharingMode::eExclusive
        : vk::SharingMode::eConcurrent;

    vk::SwapchainCreateInfoKHR createInfo(vk::SwapchainCreateFlagsKHR(),
        surface,
        data.imageCount,
        data.surfaceFormat.format,
        data.surfaceFormat.colorSpace,
        data.extent,
        1,
        vk::ImageUsageFlagBits::eColorAttachment,
        sharingMode,
        {},
        data.transform,
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        data.presentMode,
        true,
        oldSwapchain);

    if (sharingMode == vk::SharingMode::eConcurrent)
    {
        vector<uint32_t> indices = vector<uint32_t>(queueFamilyIndices.getIndices().begin(),
            queueFamilyIndices.getIndices().end());
        createInfo.setQueueFamilyIndices(indices);
    }

    SwapchainData swapchainData;
    swapchainData.imageFormat = data.surfaceFormat.format;
    swapchainData.imageExtent = data.extent;
    try
    {
        swapchainData.swapchain = device.createSwapchainKHR(createInfo);
    }
    catch (vk::SystemError& err)
    {
        std::cerr << "Vulkan Excception: " << err.what() << std::endl;
    }
    catch (std::exception& err)
    {
        std::cerr << "Exception: " << err.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown error when creating vulkan instance";
    }

    if (swapchainData.swapchain)
    {
        swapchainData.images = device.getSwapchainImagesKHR(swapchainData.swapchain);

        swapchainData.imageViews.clear();
        vk::ComponentMapping cmpMap;
        vk::ImageSubresourceRange subRR(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        for (const vk::Image& i : swapchainData.images)
        {
            vk::ImageViewCreateInfo createInfo(vk::ImageViewCreateFlags(),
                i,
                vk::ImageViewType::e2D,
                swapchainData.imageFormat,
                cmpMap,
                subRR);
            vk::ImageView imageView = device.createImageView(createInfo);
            swapchainData.imageViews.push_back(imageView);
        }
    }

    return swapchainData;
}

engine::vulkan::CommandData engine::vulkan::createCommandData(const vk::Device& device,
    const QueueFamilyIndices& queueFamilyIndices)
{
    CommandData data;

    vk::CommandPoolCreateInfo createInfo(vk::CommandPoolCreateFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer),
        queueFamilyIndices.graphics);

    try {
        data.pool = device.createCommandPool(createInfo);
        vk::CommandBufferAllocateInfo allocateInfo(data.pool, vk::CommandBufferLevel::ePrimary, 1);
        data.buffers = device.allocateCommandBuffers(allocateInfo);
    }
    catch (vk::SystemError& err)
    {
        std::cerr << "Vulkan Excception: " << err.what() << std::endl;
    }
    catch (std::exception& err)
    {
        std::cerr << "Exception: " << err.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown error when creating vulkan instance";
    }

    return data;
}