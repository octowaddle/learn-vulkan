#ifndef QUEUE_H
#define QUEUE_H

#include <vulkan/vulkan.h>

VkSemaphore semaphore_create(const VkDevice device);

void semaphore_destroy(const VkDevice device, const VkSemaphore semaphore);

VkFence *fences_create(const VkDevice device, const uint32_t swapchain_image_count);

void fences_destroy(const VkDevice device, VkFence *fences, const uint32_t swapchain_image_count);

void queue_draw(
    const VkQueue queue,
    const VkDevice device,
    const VkSwapchainKHR swapchain,
    const VkCommandBuffer *command_buffers,
    const VkSemaphore semaphore_image_available,
    const VkSemaphore semaphore_image_rendered,
    const VkFence *fences
);

#endif
