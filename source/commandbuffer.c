#include <commandbuffer.h>

#include <stdio.h>
#include <stdlib.h>

VkCommandPool command_pool_create(const VkDevice device, const uint32_t queue_family_index) {
    const VkCommandPoolCreateInfo command_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueFamilyIndex = queue_family_index
    };

    VkCommandPool command_pool = VK_NULL_HANDLE;
    VkResult result = vkCreateCommandPool(device, &command_pool_create_info, NULL, &command_pool);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create command pool\n");
        exit(1);
    }

    return command_pool;
}

void command_pool_destroy(const VkDevice device, const VkCommandPool command_pool) {
    vkDestroyCommandPool(device, command_pool, NULL);
}

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
) {
    const VkCommandBufferAllocateInfo draw_command_buffer_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = swapchain_image_count
    };

    VkCommandBuffer *draw_command_buffers = malloc(swapchain_image_count * (sizeof *draw_command_buffers));
    VkResult result = vkAllocateCommandBuffers(device, &draw_command_buffer_allocate_info, draw_command_buffers);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to allocate command buffers\n");
        exit(1);
    }

    const VkCommandBufferBeginInfo draw_command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
        .pInheritanceInfo = NULL
    };

    const VkClearColorValue clear_color_value = {
        .float32 = {0.0f, 0.0f, 0.0f, 1.0f},
        .int32 = {0, 0, 0, 255},
        .uint32 = {0U, 0U, 0U, 255U}
    };

    const VkClearDepthStencilValue clear_depth_stencil_value = {
        .depth = 0.0f,
        .stencil = 0U
    };

    const VkClearValue clear_value = {
        .color = clear_color_value,
        .depthStencil = clear_depth_stencil_value
    };

    for (uint32_t draw_command_buffer_index = 0U; draw_command_buffer_index < swapchain_image_count; ++draw_command_buffer_index) {
        result = vkBeginCommandBuffer(draw_command_buffers[draw_command_buffer_index], &draw_command_buffer_begin_info);

        if (result != VK_SUCCESS) {
            fprintf(stderr, "error: failed to begin command buffer recording\n");
            exit(1);
        }


        const VkRenderPassBeginInfo render_pass_begin_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = NULL,
            .renderPass = render_pass,
            .framebuffer = framebuffers[draw_command_buffer_index],
            .renderArea.offset.x = 0,
            .renderArea.offset.y = 0,
            .renderArea.extent.width = surface_capabilities.currentExtent.width,
            .renderArea.extent.height = surface_capabilities.currentExtent.height,
            .clearValueCount = 1,
            .pClearValues = &clear_value
        };

        vkCmdBeginRenderPass(draw_command_buffers[draw_command_buffer_index], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(draw_command_buffers[draw_command_buffer_index], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);

        const VkDeviceSize vertex_buffer_offsets[] = {0};
        vkCmdBindVertexBuffers(draw_command_buffers[draw_command_buffer_index], 0, 1, &vertex_buffer, vertex_buffer_offsets);

        vkCmdDraw(draw_command_buffers[draw_command_buffer_index], vertex_count, 1, 0, 0);

        vkCmdEndRenderPass(draw_command_buffers[draw_command_buffer_index]);

        result = vkEndCommandBuffer(draw_command_buffers[draw_command_buffer_index]);

        if (result != VK_SUCCESS) {
            fprintf(stderr, "error: failed to end command buffer recording\n");
            exit(1);
        }
    };

    return draw_command_buffers;
}

void command_buffers_free(const VkDevice device, const VkCommandPool command_pool, VkCommandBuffer *command_buffers, const uint32_t swapchain_image_count) {
    vkFreeCommandBuffers(device, command_pool, swapchain_image_count, command_buffers);
    free(command_buffers);
}
