#pragma once

#include "external.hpp"
#include "noncopyable.hpp"
#include "physical.hpp"

namespace vulkan
{

class Device : NonCopyable
{
public:
    inline Device(const Physical &physical);

    inline ~Device();

    inline void wait_idle() const;

    inline const VkDevice &get_handle() const;

private:
    VkDevice handle;
};

inline Device::Device(const Physical &physical)
{
    float queue_priorities[] = {1.0f};

    VkDeviceQueueCreateInfo device_queue_create_info;
    device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    device_queue_create_info.pNext = nullptr;
    device_queue_create_info.flags = 0;
    device_queue_create_info.queueFamilyIndex = physical.get_queue_family_index();
    device_queue_create_info.queueCount = 1;
    device_queue_create_info.pQueuePriorities = queue_priorities;

    unsigned int device_extension_count = 1;
    const char *device_extension_names[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};


    VkDeviceCreateInfo device_create_info;
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pNext = nullptr;
    device_create_info.flags = 0;
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pQueueCreateInfos = &device_queue_create_info;
    device_create_info.enabledLayerCount = 0;
    device_create_info.ppEnabledLayerNames = nullptr;
    device_create_info.enabledExtensionCount = device_extension_count;
    device_create_info.ppEnabledExtensionNames = device_extension_names;
    device_create_info.pEnabledFeatures = nullptr;


    if (vkCreateDevice(physical.get_handle(), &device_create_info, nullptr, &handle) != VK_SUCCESS)
    {
        std::cerr << "Failed to create device." << std::endl;
        throw 1;
    }
}

inline Device::~Device()
{
    vkDestroyDevice(handle, nullptr);
}

inline void Device::wait_idle() const
{
    vkDeviceWaitIdle(handle);
}

inline const VkDevice &Device::get_handle() const
{
    return handle;
}

}
