#ifndef BUFFER_H
#define BUFFER_H

#include <vulkan/vulkan.h>

VkBuffer buffer_create(
    const VkDevice device,
    const VkBufferUsageFlags buffer_usage_flags,
    const uint32_t buffer_size
);

VkDeviceMemory buffer_allocate_device_memory(
    const VkBuffer buffer,
    const VkDevice device,
    const VkPhysicalDeviceMemoryProperties physical_device_memory_properties,
    const VkMemoryPropertyFlags memory_property_flags
);

void buffer_upload_data(
    const VkBuffer buffer,
    const VkDevice device,
    const VkDeviceMemory buffer_device_memory,
    const uint32_t buffer_size,
    const void *buffer_data
);

void buffer_copy_data(
    const VkBuffer source_buffer,
    const VkBuffer destination_buffer,
    const VkDevice device,
    const VkCommandPool command_pool,
    const VkQueue queue,
    const uint32_t buffer_size
);

void buffer_destroy(
    const VkDevice device,
    const VkBuffer buffer
);

void buffer_free_memory(
    const VkDevice device,
    const VkDeviceMemory buffer_device_memory
);

#endif
