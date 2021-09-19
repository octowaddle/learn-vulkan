#include <queue.h>

#include <stdio.h>
#include <stdlib.h>

VkSemaphore semaphore_create(const VkDevice device) {
    const VkSemaphoreCreateInfo semaphore_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0
    };

    VkSemaphore semaphore;
    VkResult result = vkCreateSemaphore(device, &semaphore_create_info, NULL, &semaphore);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create semaphore\n");
        exit(1);
    }

    return semaphore;
}

void semaphore_destroy(const VkDevice device, const VkSemaphore semaphore) {
    vkDestroySemaphore(device, semaphore, NULL);
}

VkFence *fences_create(const VkDevice device, const uint32_t swapchain_image_count) {
    const VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    VkFence *fences = malloc(swapchain_image_count * (sizeof *fences));

    for (uint32_t fence_index = 0U; fence_index < swapchain_image_count; ++fence_index) {
        VkResult result = vkCreateFence(device, &fence_create_info, NULL, &fences[fence_index]);

        if (result != VK_SUCCESS) {
            fprintf(stderr, "error: failed to create fence\n");
            exit(1);
        }
    }

    return fences;
}

void fences_destroy(const VkDevice device, VkFence *fences, const uint32_t swapchain_image_count) {
    for (uint32_t fence_index = 0U; fence_index < swapchain_image_count; ++fence_index) {
        vkDestroyFence(device, fences[fence_index], NULL);
    }
    free(fences);
}

void queue_draw(
    const VkQueue queue,
    const VkDevice device,
    const VkSwapchainKHR swapchain,
    const VkCommandBuffer *command_buffers,
    const VkSemaphore semaphore_image_available,
    const VkSemaphore semaphore_image_rendered,
    const VkFence *fences
) {
    uint32_t image_index = 0;
    VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, semaphore_image_available, VK_NULL_HANDLE, &image_index);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to acquire next image\n");
        exit(1);
    }

    result = vkWaitForFences(device, 1, &fences[image_index], VK_TRUE, UINT64_MAX);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to wait for fence\n");
        exit(1);
    }

    result = vkResetFences(device, 1, &fences[image_index]);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to reset fence\n");
        exit(1);
    }

    const VkPipelineStageFlags pipeline_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    const VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &semaphore_image_available,
        .pWaitDstStageMask = &pipeline_stage_flags,
        .commandBufferCount = 1,
        .pCommandBuffers = &command_buffers[image_index],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &semaphore_image_rendered
    };

    result = vkQueueSubmit(queue, 1, &submit_info, fences[image_index]);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to submit to queue\n");
        exit(1);
    }

    const VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &semaphore_image_rendered,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &image_index,
        .pResults = NULL
    };

    result = vkQueuePresentKHR(queue, &present_info);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to present queue\n");
        exit(1);
    }
}
