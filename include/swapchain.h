#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan/vulkan.h>

uint32_t swapchain_choose_min_image_count(const VkSurfaceCapabilitiesKHR surface_capabilities);

VkSwapchainKHR swapchain_create(
    const VkPhysicalDevice physical_device,
    const VkDevice device,
    const VkSurfaceKHR surface,
    const VkSurfaceFormatKHR surface_format,
    const VkSurfaceCapabilitiesKHR surface_capabilities,
    const VkSwapchainKHR old_swapchain,
    const uint32_t queue_family_index,
    const uint32_t swapchain_min_image_count
);

void swapchain_destroy(const VkSwapchainKHR swapchain, const VkDevice device);

uint32_t swapchain_get_image_count(const VkSwapchainKHR swapchain, const VkDevice device);

VkImageView *swapchain_create_image_views(const VkSwapchainKHR swapchain, const VkDevice device, const VkSurfaceFormatKHR surface_format, uint32_t swapchain_image_count);

void swapchain_image_views_destroy(VkImageView *image_views, const VkDevice device, const uint32_t image_view_count);

#endif
