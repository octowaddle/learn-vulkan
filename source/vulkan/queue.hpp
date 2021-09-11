#pragma once

#include "external.hpp"
#include "noncopyable.hpp"
#include "physical.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "framebuffer.hpp"
#include "impl.hpp"

namespace vulkan
{

class Queue : NonCopyable
{
public:
    inline Queue(const Physical &physical, const Device &device, const Swapchain &swapchain);

    inline ~Queue();

    void present(const Framebuffer &framebuffer) const;

private:
    impl::Queue impl = VK_NULL_HANDLE;

    impl::Semaphore semaphore_image_available = VK_NULL_HANDLE;

    impl::Semaphore semaphore_image_rendered = VK_NULL_HANDLE;

    std::vector<impl::Fence> fences;

    impl::Device device;

    impl::Swapchain swapchain;
};

inline Queue::Queue(const Physical &physical, const Device &device, const Swapchain &swapchain)
: impl(impl::get_device_queue(*device, physical.get_queue_family_index()))
, semaphore_image_available(impl::create_semaphore(*device))
, semaphore_image_rendered(impl::create_semaphore(*device))
, device(*device)
, swapchain(*swapchain)
{
    ///////////////////////////////////////////////////////////////////////////
    /// Set up fences.

    fences.resize(swapchain.get_image_views().size());

    for (unsigned int i = 0; i < swapchain.get_image_views().size(); i++)
    {
        fences[i] = impl::create_fence(*device);
    }
}

inline Queue::~Queue()
{
    for (const auto &fence : fences)
    {
        impl::destroy_fence(device, fence);
    }

    impl::destroy_semaphore(device, semaphore_image_available);
    impl::destroy_semaphore(device, semaphore_image_rendered);
}

inline void Queue::present(const Framebuffer &framebuffer) const
{
    auto image_index = impl::next_image_index(device, swapchain, semaphore_image_available);

    impl::wait_and_reset_fence(device, fences[image_index]);
    impl::submit_queue(impl, framebuffer.get_command_bufers()[image_index], fences[image_index], semaphore_image_available, semaphore_image_rendered);
    impl::present_queue(impl, swapchain, semaphore_image_rendered, image_index);
}

}
