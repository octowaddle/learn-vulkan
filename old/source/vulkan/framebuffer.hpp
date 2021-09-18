#pragma once

#include "external.hpp"
#include "noncopyable.hpp"
#include "device.hpp"
#include "physical.hpp"
#include "pipeline.hpp"
#include "renderpass.hpp"
#include "surface.hpp"
#include "swapchain.hpp"
#include "impl.hpp"

namespace vulkan
{

class Framebuffer : NonCopyable
{
public:
    inline Framebuffer(const Physical &physical, const Device &device, const Swapchain &swapchain, const RenderPass &render_pass, const Surface &surface, const Pipeline &pipeline);

    inline ~Framebuffer();

    const std::vector<impl::CommandBuffer> &get_command_bufers() const;

private:
    std::vector<impl::Framebuffer> impl;

    std::vector<impl::CommandBuffer> command_buffers;

    impl::CommandPool command_pool = VK_NULL_HANDLE;

    impl::Device device;
};

inline Framebuffer::Framebuffer(const Physical &physical, const Device &device, const Swapchain &swapchain, const RenderPass &render_pass, const Surface &surface, const Pipeline &pipeline)
: device(*device)
{
    ///////////////////////////////////////////////////////////////////////////
    /// Create framebuffers.

    impl.resize(swapchain.get_image_views().size());

    for (unsigned int i = 0; i < swapchain.get_image_views().size(); i++)
    {
        impl[i] = impl::create_framebuffer(*render_pass, swapchain.get_image_views()[i], surface.get_capabilities().currentExtent.width, surface.get_capabilities().currentExtent.height, *device);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Create command buffers.

    command_pool = impl::create_command_pool(physical.get_queue_family_index(), *device);
    command_buffers = impl::allocate_command_buffers(command_pool, swapchain.get_image_views().size(), *device);

    impl::ClearValue clear_value = {0.0f, 0.0f, 0.0f, 1.0f}; // Set clear color

    // Record commands
    for (unsigned int i = 0; i < swapchain.get_image_views().size(); i++)
    {
        impl::record_command_buffer(command_buffers[i], *render_pass,
            impl[i], surface.get_capabilities().currentExtent.width,
            surface.get_capabilities().currentExtent.height, clear_value,
            *pipeline);
    }
}

inline Framebuffer::~Framebuffer()
{
    for (auto &framebuffer : impl)
    {
        impl::destroy_framebuffer(device, framebuffer);
    }

    impl::destroy_command_pool(device, command_pool);
}

const std::vector<impl::CommandBuffer> &Framebuffer::get_command_bufers() const
{
    return command_buffers;
}

}
