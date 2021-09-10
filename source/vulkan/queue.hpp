#pragma once

#include "external.hpp"
#include "noncopyable.hpp"
#include "physical.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "framebuffer.hpp"

namespace vulkan
{

using VkFences = std::vector<VkFence>;

class Queue : NonCopyable
{
public:
    inline Queue(const Physical &physical, const Device &device, const Swapchain &swapchain);

    inline ~Queue();

    void present(const Framebuffer &framebuffer) const;

private:
    VkQueue handle = VK_NULL_HANDLE;

    VkSemaphore semaphore_image_available = VK_NULL_HANDLE;

    VkSemaphore semaphore_image_rendered = VK_NULL_HANDLE;

    VkFences fences;

    VkDevice device;

    VkSwapchainKHR swapchain;
};

inline Queue::Queue(const Physical &physical, const Device &device, const Swapchain &swapchain)
: device(device.get_handle())
, swapchain(swapchain.get_handle())
{
    ///////////////////////////////////////////////////////////////////////////
    /// Get queue from device.

    vkGetDeviceQueue(device.get_handle(), physical.get_queue_family_index(), 0, &handle);

    ///////////////////////////////////////////////////////////////////////////
    /// Set up semaphores.

    VkSemaphoreCreateInfo semaphore_create_info;
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_create_info.pNext = nullptr;
    semaphore_create_info.flags = 0;

    if (vkCreateSemaphore(device.get_handle(), &semaphore_create_info, nullptr, &semaphore_image_available) != VK_SUCCESS)
    {
        std::cerr << "Failed to create semaphore." << std::endl;
        throw 1;
    }

    if (vkCreateSemaphore(device.get_handle(), &semaphore_create_info, nullptr, &semaphore_image_rendered   ) != VK_SUCCESS)
    {
        std::cerr << "Failed to create semaphore." << std::endl;
        throw 1;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Set up fences.

    VkFenceCreateInfo fence_create_info;
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.pNext = nullptr;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    fences.resize(swapchain.get_image_views().size());

    for (unsigned int i = 0; i < swapchain.get_image_views().size(); i++)
    {
        if (vkCreateFence(device.get_handle(), &fence_create_info, nullptr, &fences[i]) != VK_SUCCESS)
        {
            std::cerr << "Failed to create fence." << std::endl;
            throw 1;
        }
    }
}

inline Queue::~Queue()
{
    for (const auto &fence : fences)
    {
        vkDestroyFence(device, fence, nullptr);
    }

    vkDestroySemaphore(device, semaphore_image_available, nullptr);
    vkDestroySemaphore(device, semaphore_image_rendered, nullptr);
}

inline void Queue::present(const Framebuffer &framebuffer) const
{
    unsigned int image_index = 0;
    if (vkAcquireNextImageKHR(device, swapchain, std::numeric_limits<uint64_t>::max(), semaphore_image_available, VK_NULL_HANDLE, &image_index) != VK_SUCCESS)
    {
        std::cerr << "Failed to acquire next image.";
        throw 1;
    }

    // Wait fence
    if (vkWaitForFences(device, 1, &fences[image_index], VK_TRUE, std::numeric_limits<uint64_t>::max()) != VK_SUCCESS)
    {
        std::cerr << "Failed to wait for fence." << std::endl;
        throw 1;
    }

    // Reset fence
    if (vkResetFences(device, 1, &fences[image_index]) != VK_SUCCESS)
    {
        std::cerr << "Failed to wait for fence." << std::endl;
        throw 1;
    }

    // When should we wait for the semaphore?
    VkPipelineStageFlags pipeline_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    // Set which semaphore to wait for and which semaphore to set
    VkSubmitInfo submit_info;
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pNext = nullptr;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &semaphore_image_available;
    submit_info.pWaitDstStageMask = &pipeline_stage_flags;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &framebuffer.get_command_bufers()[image_index];
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &semaphore_image_rendered;

    if (vkQueueSubmit(handle, 1, &submit_info, fences[image_index]) != VK_SUCCESS)
    {
        std::cerr << "Failed submitting to queue." << std::endl;
        throw 1;
    }

    VkPresentInfoKHR present_info;
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pNext = nullptr;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &semaphore_image_rendered;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain;
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr;

    if (vkQueuePresentKHR(handle, &present_info) != VK_SUCCESS)
    {
        std::cerr << "Failed to present queue." << std::endl;
        throw 1;
    }
}

}
