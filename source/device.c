#include <device.h>

#include <stdio.h>
#include <stdlib.h>

VkDevice device_create(const VkPhysicalDevice physical_device, const uint32_t queue_family_index) {
    const float queue_priorities[1] = {1.0f};

    const uint32_t device_extension_count = 1;
    const char *const device_extension_names[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    // TODO Create multiple queues from multiple queue families.

    const VkDeviceQueueCreateInfo device_queue_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueFamilyIndex = queue_family_index,
        .queueCount = 1,
        .pQueuePriorities = queue_priorities
    };

    const VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &device_queue_create_info,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = device_extension_count,
        .ppEnabledExtensionNames = device_extension_names,
        .pEnabledFeatures = NULL
    };

    VkDevice device = VK_NULL_HANDLE;
    VkResult result = vkCreateDevice(physical_device, &device_create_info, NULL, &device);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create device\n");
        exit(1);
    }

    return device;
}

void device_destroy(const VkDevice device) {
    vkDestroyDevice(device, NULL);
}

VkQueue device_get_queue(const VkDevice device, const uint32_t queue_family_index) {
    VkQueue queue = VK_NULL_HANDLE;
    // TODO: Select queue index (create multiple queues in queue creation process)
    vkGetDeviceQueue(device, queue_family_index, 0, &queue);
    return queue;
}
