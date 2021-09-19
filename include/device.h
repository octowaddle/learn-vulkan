#ifndef DEVICE_H
#define DEVICE_H

#include <vulkan/vulkan.h>

VkDevice device_create(const VkPhysicalDevice physical_device, const uint32_t queue_family_index);

void device_destroy(const VkDevice device);

VkQueue device_get_queue(const VkDevice device, const uint32_t queue_family_index);

#endif
