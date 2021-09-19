#include <swapchain.h>

#include <stdio.h>
#include <stdlib.h>

// TODO Get surface format and capabilities directly from surface.

uint32_t swapchain_choose_min_image_count(const VkSurfaceCapabilitiesKHR surface_capabilities) {
    uint32_t swapchain_min_image_count = 3;

    while (swapchain_min_image_count > surface_capabilities.minImageCount) {
        swapchain_min_image_count--;
    }

    return swapchain_min_image_count;
}

VkSwapchainKHR swapchain_create(
    const VkPhysicalDevice physical_device,
    const VkDevice device,
    const VkSurfaceKHR surface,
    const VkSurfaceFormatKHR surface_format,
    const VkSurfaceCapabilitiesKHR surface_capabilities,
    const VkSwapchainKHR old_swapchain,
    const uint32_t queue_family_index,
    const uint32_t swapchain_min_image_count
) {
    VkBool32 physical_device_surface_support = VK_FALSE;

    VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, queue_family_index, surface, &physical_device_surface_support);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to check surface support\n");
        exit(1);
    }

    if (!physical_device_surface_support) {
        fprintf(stderr, "error: physical device does not support surfaces\n");
        exit(1);
    }

    // TODO Check for better presentation modes.

    const VkSwapchainCreateInfoKHR swapchain_create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .surface = surface,
        .minImageCount = swapchain_min_image_count,
        .imageFormat = surface_format.format,
        .imageColorSpace = surface_format.colorSpace,
        .imageExtent = surface_capabilities.currentExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR,
        .clipped = VK_TRUE,
        .oldSwapchain = old_swapchain
    };

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    result = vkCreateSwapchainKHR(device, &swapchain_create_info, NULL, &swapchain);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create surface\n");
        exit(1);
    }

    return swapchain;
}

void swapchain_destroy(const VkSwapchainKHR swapchain, const VkDevice device) {
    vkDestroySwapchainKHR(device, swapchain, NULL);
}

uint32_t swapchain_get_image_count(const VkSwapchainKHR swapchain, const VkDevice device) {
    uint32_t swapchain_image_count = 0;
    VkResult result = vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, NULL);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to get swapchain images\n");
        exit(1);
    }
    return swapchain_image_count;
}

VkImageView *swapchain_create_image_views(const VkSwapchainKHR swapchain, const VkDevice device, const VkSurfaceFormatKHR surface_format, uint32_t swapchain_image_count) {
    VkImage *swapchain_images = malloc(swapchain_image_count * (sizeof *swapchain_images));

    // TODO Research why swapchain_image_count is not const.

    VkResult result = vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, swapchain_images);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to get swapchain images\n");
        exit(1);
    }

    if (swapchain_image_count == 0) {
        fprintf(stderr, "error: swapchain has no images\n");
        exit(1);
    }

    VkImageView *image_views = malloc(swapchain_image_count * (sizeof *image_views));

    for (uint32_t swapchain_image_index = 0U; swapchain_image_index < swapchain_image_count; ++swapchain_image_index) {
        const VkImageViewCreateInfo image_view_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .image = swapchain_images[swapchain_image_index],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = surface_format.format,
            .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
            .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .subresourceRange.baseMipLevel = 0,
            .subresourceRange.levelCount = 1,
            .subresourceRange.baseArrayLayer = 0,
            .subresourceRange.layerCount = 1
        };

        result = vkCreateImageView(device, &image_view_create_info, NULL, &image_views[swapchain_image_index]);

        if (result != VK_SUCCESS) {
            fprintf(stderr, "error: failed to create image view\n");
            exit(1);
        }
    }

    free(swapchain_images);
    swapchain_images = NULL;

    return image_views;
}

void swapchain_image_views_destroy(VkImageView *image_views, const VkDevice device, const uint32_t image_view_count) {
        for (uint32_t image_view_index = 0U; image_view_index < image_view_count; ++image_view_index) {
        vkDestroyImageView(device, image_views[image_view_index], NULL);
    }
    free(image_views);
}
