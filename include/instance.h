#ifndef INSTANCE_H
#define INSTANCE_H

#include <vulkan/vulkan.h>

VkInstance instance_create(
    const uint32_t enabled_layer_count,
    const char *const *const enabled_layer_names,
    const uint32_t enabled_extension_count,
    const char *const *const enabled_extension_names
);

void instance_destroy(const VkInstance instance);

VkPhysicalDevice instance_choose_physical_device(const VkInstance instance);

VkPhysicalDeviceProperties physical_device_get_properties(const VkPhysicalDevice physical_device);

VkPhysicalDeviceMemoryProperties physical_device_get_memory_properties(const VkPhysicalDevice physical_device);

uint32_t physical_device_find_queue_family_index(const VkPhysicalDevice physical_device, const VkQueueFlagBits queue_flag_bits);

#endif
