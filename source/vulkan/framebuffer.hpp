#pragma once

#include "external.hpp"
#include "noncopyable.hpp"
#include "device.hpp"
#include "physical.hpp"
#include "pipeline.hpp"
#include "renderpass.hpp"
#include "surface.hpp"
#include "swapchain.hpp"

namespace vulkan
{

using VkFramebuffers = std::vector<VkFramebuffer>;
using VkCommandBuffers = std::vector<VkCommandBuffer>;

class Framebuffer : NonCopyable
{
public:
    inline Framebuffer(const Physical &physical, const Device &device, const Swapchain &swapchain, const RenderPass &render_pass, const Surface &surface, const Pipeline &pipeline);

    inline ~Framebuffer();

    const VkFramebuffers &get_handle() const;

    const VkCommandBuffers &get_command_bufers() const;

private:
    VkFramebuffers handle;

    VkCommandBuffers command_buffers;

    VkCommandPool command_pool = VK_NULL_HANDLE;

    VkDevice device;
};

inline Framebuffer::Framebuffer(const Physical &physical, const Device &device, const Swapchain &swapchain, const RenderPass &render_pass, const Surface &surface, const Pipeline &pipeline)
: device(device.get_handle())
{
    ///////////////////////////////////////////////////////////////////////////
    /// Create framebuffers.

    handle.resize(swapchain.get_image_views().size());

    for (unsigned int i = 0; i < swapchain.get_image_views().size(); i++)
    {
        VkFramebufferCreateInfo framebuffer_create_info;
        framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_create_info.pNext = nullptr;
        framebuffer_create_info.flags = 0;
        framebuffer_create_info.renderPass = render_pass.get_handle();
        framebuffer_create_info.attachmentCount = 1;
        framebuffer_create_info.pAttachments = &swapchain.get_image_views()[i];
        framebuffer_create_info.width = surface.get_capabilities().currentExtent.width;
        framebuffer_create_info.height = surface.get_capabilities().currentExtent.height;
        framebuffer_create_info.layers = 1;

        // Needs Vulkan clean up.
        if (vkCreateFramebuffer(device.get_handle(), &framebuffer_create_info, nullptr, &handle[i]) != VK_SUCCESS)
        {
            std::cerr << "Failed to create framebuffer." << std::endl;
            throw 1;
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Create command buffers.


    VkCommandPoolCreateInfo command_pool_create_info;
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.pNext = nullptr;
    command_pool_create_info.flags = 0;
    command_pool_create_info.queueFamilyIndex = physical.get_queue_family_index();

    if (vkCreateCommandPool(device.get_handle(), &command_pool_create_info, nullptr, &command_pool) != VK_SUCCESS)
    {
        std::cerr << "Failed to create command pool." << std::endl;
        throw 1;
    }

    VkCommandBufferAllocateInfo command_buffer_allocate_info;
    command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocate_info.pNext = nullptr;
    command_buffer_allocate_info.commandPool = command_pool;
    command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocate_info.commandBufferCount = swapchain.get_image_views().size(); // Every frame buffer needs a command buffer

    command_buffers.resize(swapchain.get_image_views().size());

    if (vkAllocateCommandBuffers(device.get_handle(), &command_buffer_allocate_info, command_buffers.data()) != VK_SUCCESS)
    {
        std::cerr << "Failed to allocate command buffers." << std::endl;
        throw 1;
    }

    VkCommandBufferBeginInfo command_buffer_begin_info;
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.pNext = nullptr;
    command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    command_buffer_begin_info.pInheritanceInfo = nullptr; // Important if secondary command buffer is in use.

    VkClearValue clear_value = {0.0f, 0.0f, 0.0f, 1.0f}; // Set clear color

    // Record commands
    for (unsigned int i = 0; i < swapchain.get_image_views().size(); i++)
    {
        if (vkBeginCommandBuffer(command_buffers[i], &command_buffer_begin_info) != VK_SUCCESS)
        {
            std::cerr << "Failed to begin command buffer recording." << std::endl;
            throw 1;
        }

        VkRenderPassBeginInfo render_pass_begin_info;
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.pNext = nullptr;
        render_pass_begin_info.renderPass = render_pass.get_handle();
        render_pass_begin_info.framebuffer = handle[i];
        render_pass_begin_info.renderArea.offset.x = 0;
        render_pass_begin_info.renderArea.offset.y = 0;
        render_pass_begin_info.renderArea.extent.width = surface.get_capabilities().currentExtent.width;
        render_pass_begin_info.renderArea.extent.height = surface.get_capabilities().currentExtent.height;
        render_pass_begin_info.clearValueCount = 1;
        render_pass_begin_info.pClearValues = &clear_value;

        vkCmdBeginRenderPass(command_buffers[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE /* because only primary command buffers */);

        // Record: Bind pipeline and draw.
        vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get_handle());
        vkCmdDraw(command_buffers[i], 3, 1, 0, 0); // Hard coded triangle (in vertex shader).

        vkCmdEndRenderPass(command_buffers[i]);

        if (vkEndCommandBuffer(command_buffers[i]) != VK_SUCCESS)
        {
            std::cerr << "Failed to stop command buffer recording." << std::endl;
            throw 1;
        }
    }
}

inline Framebuffer::~Framebuffer()
{
    for (auto &framebuffer : handle)
    {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    vkDestroyCommandPool(device, command_pool, nullptr);
}

const VkFramebuffers &Framebuffer::get_handle() const
{
    return handle;
}

const VkCommandBuffers &Framebuffer::get_command_bufers() const
{
    return command_buffers;
}

}
