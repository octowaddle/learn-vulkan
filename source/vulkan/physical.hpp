#pragma once

#include "external.hpp"
#include "instance.hpp"
#include "noncopyable.hpp"

namespace vulkan
{

class Physical : NonCopyable
{
public:
    inline Physical(const Instance &instance);

    inline ~Physical() = default;

    inline const VkPhysicalDevice &get_handle() const;

    inline uint32_t get_queue_family_index() const;

private:
    VkPhysicalDevice handle;

    uint32_t queue_family_index;
};

inline Physical::Physical(const Instance &instance)
{
    unsigned int physical_device_count = 0;
    if (vkEnumeratePhysicalDevices(instance.get_handle(), &physical_device_count, nullptr) != VK_SUCCESS)
    {
        std::cerr << "Failed to find physical device count." << std::endl;
        throw 1;
    }

    if (physical_device_count == 0)
    {
        std::cerr << "No physical device is available." << std::endl;
        throw 1;
    }

    VkPhysicalDevice *physical_devices = new VkPhysicalDevice[physical_device_count];

    if (vkEnumeratePhysicalDevices(instance.get_handle(), &physical_device_count, physical_devices) != VK_SUCCESS)
    {
        std::cerr << "Failed to find physical devices." << std::endl;
        throw 1;
    }

    // TODO: Choose a physical device properly instead of choosing the first.
    handle = physical_devices[0];
    delete[] physical_devices;

    unsigned int queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(handle, &queue_family_count, nullptr);
    if (queue_family_count == 0)
    {
        std::cerr << "No queue families are available." << std::endl;
        throw 1;
    }

    VkQueueFamilyProperties *queue_family_properties = new VkQueueFamilyProperties[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(handle, &queue_family_count, queue_family_properties);

    int queue_family_index_temp = -1;
    for (unsigned int i = 0; i < queue_family_count; i++)
    {
        // TODO: Choose a queue family index properly
        if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queue_family_index_temp = i;
            break;
        }
    }

    delete[] queue_family_properties;

    if (queue_family_index_temp < 0)
    {
        std::cerr << "No suitable queue family found." << std::endl;
        throw 0;
    }

    // TODO: Check presentation support

    queue_family_index = queue_family_index_temp;
}

inline const VkPhysicalDevice &Physical::get_handle() const
{
    return handle;
}

inline uint32_t Physical::get_queue_family_index() const
{
    return queue_family_index;
}

}
