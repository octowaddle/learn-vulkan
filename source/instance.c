#include <instance.h>

#include <stdio.h>
#include <stdlib.h>

VkInstance instance_create(
    const uint32_t enabled_layer_count,
    const char *const *const enabled_layer_names,
    const uint32_t enabled_extension_count,
    const char *const *const enabled_extension_names
) {
    const VkApplicationInfo application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = "Vulkan Application",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Vulkan Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0
    };

    const VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .pApplicationInfo = &application_info,
        .enabledLayerCount = enabled_layer_count,
        .ppEnabledLayerNames = enabled_layer_names,
        .enabledExtensionCount = enabled_extension_count,
        .ppEnabledExtensionNames = enabled_extension_names
    };

    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vkCreateInstance(&instance_create_info, NULL, &instance);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create instance\n");
        exit(1);
    }

    return instance;
}

void instance_destroy(const VkInstance instance) {
    vkDestroyInstance(instance, NULL);
}

VkPhysicalDevice instance_choose_physical_device(const VkInstance instance) {
    uint32_t physical_device_count = 0;
    vkEnumeratePhysicalDevices(instance, &physical_device_count, NULL);
    VkPhysicalDevice *physical_devices = malloc(physical_device_count * (sizeof *physical_devices));
    vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices);

    if (physical_device_count == 0) {
        fprintf(stderr, "error: no physical devices available\n");
        exit(1);
    }

    VkPhysicalDevice physical_device = physical_devices[0];

    for (uint32_t physical_device_index = 0U; physical_device_index < physical_device_count; ++physical_device_index) {
        VkPhysicalDeviceProperties physical_device_properties;
        vkGetPhysicalDeviceProperties(physical_devices[physical_device_index], &physical_device_properties);

        if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            physical_device = physical_devices[physical_device_index];
            break;
        }
    }

    free(physical_devices);

    return physical_device;
}

VkPhysicalDeviceProperties physical_device_get_properties(const VkPhysicalDevice physical_device) {
    VkPhysicalDeviceProperties physical_device_properties;
    vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);
    return physical_device_properties;
}

VkPhysicalDeviceMemoryProperties physical_device_get_memory_properties(const VkPhysicalDevice physical_device) {
    VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);
    return physical_device_memory_properties;
}

uint32_t physical_device_find_queue_family_index(const VkPhysicalDevice physical_device, const VkQueueFlagBits queue_flag_bits) {
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, NULL);
    VkQueueFamilyProperties *queue_family_properties = malloc(queue_family_count * (sizeof *queue_family_properties));
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family_properties);

    if (queue_family_count == 0) {
        fprintf(stderr, "error: no queue families are available\n");
        exit(1);
    }

    uint32_t queue_family_index = 0;
    uint32_t queue_family_found = 0;

    for (uint32_t queue_family_properties_index = 0U; queue_family_properties_index < queue_family_count; ++queue_family_properties_index) {
        if (queue_family_properties[queue_family_properties_index].queueFlags & queue_flag_bits) {
            queue_family_index = queue_family_properties_index;
            queue_family_found = 1;
            break;
        }
    }

    free(queue_family_properties);

    if (!queue_family_found) {
        fprintf(stderr, "error: no graphics queue family is available\n");
        exit(1);
    }
}
