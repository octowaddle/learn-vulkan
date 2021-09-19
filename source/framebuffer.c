#include <framebuffer.h>

#include <stdio.h>
#include <stdlib.h>

VkFramebuffer *framebuffers_create(
    const VkDevice device,
    const VkSurfaceCapabilitiesKHR surface_capabilities,
    const VkImageView *const image_views,
    const VkRenderPass render_pass,
    const uint32_t swapchain_image_count
) {
    VkFramebuffer *framebuffers = malloc(swapchain_image_count * (sizeof *framebuffers));

    for (uint32_t framebuffer_index = 0U; framebuffer_index < swapchain_image_count; ++framebuffer_index) {
        const VkFramebufferCreateInfo framebuffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .renderPass = render_pass,
            .attachmentCount = 1,
            .pAttachments = &image_views[framebuffer_index],
            .width = surface_capabilities.currentExtent.width,
            .height = surface_capabilities.currentExtent.height,
            .layers = 1
        };

        VkResult result = vkCreateFramebuffer(device, &framebuffer_create_info, NULL, &framebuffers[framebuffer_index]);

        if (result != VK_SUCCESS) {
            fprintf(stderr, "error: failed to create framebuffer\n");
            exit(1);
        }
    }

    return framebuffers;
}

void framebuffers_destroy(
    const VkDevice device,
    VkFramebuffer *framebuffers,
    const uint32_t framebuffer_count
) {
    for (uint32_t framebuffer_index = 0U; framebuffer_index < framebuffer_count; ++framebuffer_index) {
        vkDestroyFramebuffer(device, framebuffers[framebuffer_index], NULL);
    }
    free(framebuffers);
}
