#pragma once

#include "external.hpp"
#include "noncopyable.hpp"
#include "physical.hpp"
#include "device.hpp"
#include "surface.hpp"

namespace vulkan
{

using VkImageViews = std::vector<VkImageView>;

class Swapchain : NonCopyable
{
public:
    inline Swapchain(const Physical &physical, const Device &device, const Surface &surface);

    inline ~Swapchain();

    inline const VkSwapchainKHR &get_handle() const;

    inline const VkImageViews &get_image_views() const;

private:
    VkSwapchainKHR handle = VK_NULL_HANDLE;

    VkImageViews image_views;

    VkDevice device; // Unsafe. Could be deleted too early.
};

inline Swapchain::Swapchain(const Physical &physical, const Device &device, const Surface &surface)
: device(device.get_handle())
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

    VkBool32 surface_supported = false;
    if (vkGetPhysicalDeviceSurfaceSupportKHR(physical.get_handle(), physical.get_queue_family_index(), surface.get_handle(), &surface_supported) != VK_SUCCESS)
    {
        std::cerr << "Failed to check surface support of physical device." << std::endl;
        throw 1;
    }

    if (!surface_supported)
    {
        std::cerr << "Physical device does not support surfaces." << std::endl;
        throw 1;
    }

    VkSwapchainCreateInfoKHR swapchain_create_info;
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.pNext = nullptr;
    swapchain_create_info.flags = 0;
    swapchain_create_info.surface = surface.get_handle();
    swapchain_create_info.minImageCount = min_image_count;
    swapchain_create_info.imageFormat = surface.get_format().format;
    swapchain_create_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; // TODO: Check if valid.
    swapchain_create_info.imageExtent = surface.get_capabilities().currentExtent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: Support for separate presentation and graphic queues (VK_SHARING_MODE_CONCURRENT).
    swapchain_create_info.queueFamilyIndexCount = 0;
    swapchain_create_info.pQueueFamilyIndices = nullptr;
    swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR; //VK_PRESENT_MODE_FIFO_KHR; // TODO: Check if there is a better mode?
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE; // TODO: Support for resizing windows (recreate swapchain).

    if (vkCreateSwapchainKHR(device.get_handle(), &swapchain_create_info, nullptr, &handle) != VK_SUCCESS)
    {
        std::cerr << "Failed to create swapchain." << std::endl;
        throw 1;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Create image views.

    unsigned int swapchain_image_count = 0;

    if (vkGetSwapchainImagesKHR(device.get_handle(), handle, &swapchain_image_count, nullptr) != VK_SUCCESS)
    {
        std::cerr << "Failed to fetch swapchain images." << std::endl;
        throw 1;
    }

    if (swapchain_image_count == 0)
    {
        std::cerr << "No swapchain images found." << std::endl;
        throw 1;
    }

    VkImage *swapchain_images = new VkImage[swapchain_image_count];
    if (vkGetSwapchainImagesKHR(device.get_handle(), handle, &swapchain_image_count, swapchain_images) != VK_SUCCESS)
    {
        std::cerr << "Failed to fetch swapchain images." << std::endl;
        throw 1;
    }

    VkImageViewCreateInfo image_view_create_info;
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.pNext = nullptr;
    image_view_create_info.flags = 0;
    image_view_create_info.image = swapchain_images[0];
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = surface.get_format().format;
    image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    // Needs Vulkan clean up.
    image_views.resize(swapchain_image_count);
    for (unsigned int i = 0; i < swapchain_image_count; i++)
    {
        image_view_create_info.image = swapchain_images[i];

        if (vkCreateImageView(device.get_handle(), &image_view_create_info, nullptr, &image_views[i]) != VK_SUCCESS)
        {
            std::cerr << "Failed to create image view." << std::endl;
            throw 1;
        }
    }

    delete[] swapchain_images;
}

inline Swapchain::~Swapchain()
{
    for (auto &image_view : image_views)
    {
        vkDestroyImageView(device, image_view, nullptr);
    }

    vkDestroySwapchainKHR(device, handle, nullptr);
}

inline const VkSwapchainKHR &Swapchain::get_handle() const
{
    return handle;
}

inline const VkImageViews &Swapchain::get_image_views() const
{
    return image_views;
}

}
