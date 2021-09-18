#pragma once

#include "external.hpp"
#include "noncopyable.hpp"
#include "physical.hpp"
#include "device.hpp"
#include "surface.hpp"

namespace vulkan
{

class Swapchain : NonCopyable
{
public:
    inline Swapchain(const Physical &physical, const Device &device, const Surface &surface);

    inline ~Swapchain();

    inline const impl::Swapchain &operator*() const;

    inline const std::vector<impl::ImageView> &get_image_views() const;

private:
    impl::Swapchain impl = VK_NULL_HANDLE;

    std::vector<impl::ImageView> image_views;

    impl::Device device; // Unsafe. Could be deleted too early.
};

inline Swapchain::Swapchain(const Physical &physical, const Device &device, const Surface &surface)
: device(*device)
{
    ///////////////////////////////////////////////////////////////////////////
    /// Create swap chain.

    unsigned int min_image_count = 2;

    if (surface.get_capabilities().maxImageCount >= 3)
    {
        min_image_count = 3;
    }
    else
    {
        min_image_count = surface.get_capabilities().minImageCount;
    }

    impl = impl::create_swapchain(*surface, *device, *physical, physical.get_queue_family_index(), min_image_count,
    surface.get_format(), surface.get_capabilities().currentExtent.width, surface.get_capabilities().currentExtent.height);

    auto swapchain_images = impl::get_swapchain_images(impl, *device);

    image_views.resize(swapchain_images.size());
    for (unsigned int i = 0; i < swapchain_images.size(); i++)
    {
        image_views[i] = impl::create_image_view(*device, swapchain_images[i], surface.get_format());
    }
}

inline Swapchain::~Swapchain()
{
    for (auto &image_view : image_views)
    {
        impl::destroy_image_view(device, image_view);
    }

    impl::destroy_swapchain(device, impl);
}

inline const VkSwapchainKHR &Swapchain::operator*() const
{
    return impl;
}

inline const
std::vector<impl::ImageView> &Swapchain::get_image_views() const
{
    return image_views;
}

}
