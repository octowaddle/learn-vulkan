#ifndef COMMANDBUFFER_H
#define COMMANDBUFFER_H

#include <vulkan/vulkan.h>

VkCommandPool command_pool_create(const VkDevice device, const uint32_t queue_family_index);

void command_pool_destroy(const VkDevice device, const VkCommandPool command_pool);

VkCommandBuffer *command_buffer_create_draw(
    const VkDevice device,
    const VkSurfaceCapabilitiesKHR surface_capabilities,
    const VkCommandPool command_pool,
    const VkRenderPass render_pass,
    const VkFramebuffer *const framebuffers,
    const VkPipeline graphics_pipeline,
    const VkBuffer vertex_buffer,
    const uint32_t swapchain_image_count,
    const uint32_t vertex_count
);

void command_buffers_free(const VkDevice device, const VkCommandPool command_pool, VkCommandBuffer *command_buffers, const uint32_t swapchain_image_count);

#endif
