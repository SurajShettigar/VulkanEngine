#include "vulkan_graphics.h"

engine::vulkan::RenderData engine::vulkan::getRenderData(const vk::Device& device, const SwapchainData& swapchainData)
{
    RenderData data;

    vector<vk::AttachmentDescription> attachments;
    // Description of the image render pass will be writing into.
    vk::AttachmentDescription colorAttachmentDesc(vk::AttachmentDescriptionFlags(),
        swapchainData.imageFormat,
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::ePresentSrcKHR);

    // TODO: Create Depth attachment

    attachments.push_back(colorAttachmentDesc);

    // Create attachment references for subpasses
    vk::AttachmentReference colorAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);

    // Create subpass
    vk::SubpassDescription subpassDesc(vk::SubpassDescriptionFlags(),
        vk::PipelineBindPoint::eGraphics,
        {},
        colorAttachmentRef);

    // Create Renderpass
    vk::RenderPassCreateInfo createInfo(vk::RenderPassCreateFlags(), attachments, subpassDesc);

    try
    {
        data.renderPass = device.createRenderPass(createInfo);

        vk::FramebufferCreateInfo frameBufferInfo(vk::FramebufferCreateFlags(),
            data.renderPass,
            {},
            swapchainData.imageExtent.width,
            swapchainData.imageExtent.height,
            1);

        data.framebuffers.clear();
        for (const auto& i : swapchainData.imageViews)
        {
            frameBufferInfo.setAttachments(i);
            data.framebuffers.push_back(device.createFramebuffer(frameBufferInfo));
        }

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

    return data;
}