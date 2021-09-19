#include <buffer.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

VkBuffer buffer_create(
    const VkDevice device,
    const VkBufferUsageFlags buffer_usage_flags,
    const uint32_t buffer_size
) {
    const VkBufferCreateInfo buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .size = buffer_size,
        .usage = buffer_usage_flags,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
    };

    VkBuffer buffer = VK_NULL_HANDLE;
    VkResult result = vkCreateBuffer(device, &buffer_create_info, NULL, &buffer);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create buffer\n");
        exit(1);
    }

    return buffer;
}

VkDeviceMemory buffer_allocate_device_memory(
    const VkBuffer buffer,
    const VkDevice device,
    const VkPhysicalDeviceMemoryProperties physical_device_memory_properties,
    const VkMemoryPropertyFlags memory_property_flags
) {
    VkMemoryRequirements buffer_memory_requirements;
    vkGetBufferMemoryRequirements(device, buffer, &buffer_memory_requirements);

    uint32_t buffer_memory_type_index = 0;
    uint8_t  buffer_memory_type_found = 0;

    for (uint32_t memory_properties_type_index = 0U; memory_properties_type_index < physical_device_memory_properties.memoryTypeCount; ++memory_properties_type_index) {
        if ((buffer_memory_requirements.memoryTypeBits & (1 << memory_properties_type_index)) && (physical_device_memory_properties.memoryTypes[memory_properties_type_index].propertyFlags & memory_property_flags) == memory_property_flags) {
            buffer_memory_type_index = memory_properties_type_index;
            buffer_memory_type_found = 1;
            break;
        }
    }

    if (!buffer_memory_type_found) {
        fprintf(stderr, "error: failed to find required buffer memory type\n");
        exit(1);
    }

    const VkMemoryAllocateInfo buffer_memory_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = buffer_memory_requirements.size,
        .memoryTypeIndex = buffer_memory_type_index,
    };

    VkDeviceMemory buffer_device_memory;
    VkResult result = vkAllocateMemory(device, &buffer_memory_allocate_info, NULL, &buffer_device_memory);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to allocate buffer device memory\n");
        exit(1);
    }

    vkBindBufferMemory(device, buffer, buffer_device_memory, 0);

    return buffer_device_memory;
}

void buffer_upload_data(
    const VkBuffer buffer,
    const VkDevice device,
    const VkDeviceMemory buffer_device_memory,
    const uint32_t buffer_size,
    const void *buffer_data
) {
    void *staging_buffer_memory_map_address = NULL;
    vkMapMemory(device, buffer_device_memory, 0, buffer_size, 0, &staging_buffer_memory_map_address);
    memcpy(staging_buffer_memory_map_address, buffer_data, buffer_size);
    vkUnmapMemory(device, buffer_device_memory);
}

void buffer_copy_data(
    const VkBuffer source_buffer,
    const VkBuffer destination_buffer,
    const VkDevice device,
    const VkCommandPool command_pool,
    const VkQueue queue,
    const uint32_t buffer_size
) {
    const VkCommandBufferAllocateInfo buffer_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };

    VkCommandBuffer command_buffer;
    VkResult result = vkAllocateCommandBuffers(device, &buffer_allocate_info, &command_buffer);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to allocate staging to vertex command buffer\n");
        exit(1);
    }

    VkCommandBufferBeginInfo command_buffer_beging_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = NULL
    };

    result = vkBeginCommandBuffer(command_buffer, &command_buffer_beging_info);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to begin staging to vertex command buffer recording\n");
        exit(1);
    }

    const VkBufferCopy buffer_copy = {
        .srcOffset = 0,
        .dstOffset = 0,
        .size = buffer_size,
    };

    vkCmdCopyBuffer(command_buffer, source_buffer, destination_buffer, 1, &buffer_copy);

    result = vkEndCommandBuffer(command_buffer);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to end staging to vertex command buffer recording\n");
        exit(1);
    }

    const VkSubmitInfo staging_to_vertex_submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores =  NULL,
        .pWaitDstStageMask = NULL,
        .commandBufferCount = 1,
        .pCommandBuffers = &command_buffer,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = NULL
    };

    // TODO Add fences for multiple buffers and wait for them instaed of `vkQueueWaitIdle`.

    result = vkQueueSubmit(queue, 1, &staging_to_vertex_submit_info, VK_NULL_HANDLE);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to submit staging to vertex command buffer recording to queue\n");
        exit(1);
    }

    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}

void buffer_destroy(
    const VkDevice device,
    const VkBuffer buffer
) {
    vkDestroyBuffer(device, buffer, NULL);
}

void buffer_free_memory(
    const VkDevice device,
    const VkDeviceMemory buffer_device_memory
) {
    vkFreeMemory(device, buffer_device_memory, NULL);
}
