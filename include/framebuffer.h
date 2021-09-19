#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <vulkan/vulkan.h>

VkFramebuffer *framebuffers_create(
    const VkDevice device,
    const VkSurfaceCapabilitiesKHR surface_capabilities,
    const VkImageView *const image_views,
    const VkRenderPass render_pass,
    const uint32_t swapchain_image_count
);

void framebuffers_destroy(
    const VkDevice device,
    VkFramebuffer *framebuffers,
    const uint32_t framebuffer_count
);

#endif
