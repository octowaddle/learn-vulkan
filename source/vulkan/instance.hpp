#pragma once

#include "external.hpp"
#include "noncopyable.hpp"

namespace vulkan
{

class Instance : NonCopyable
{
public:
    inline Instance();

    inline ~Instance();

    inline const VkInstance &get_handle() const;

private:
    VkInstance handle;
};

inline Instance::Instance()
{
    unsigned int layer_count = 1;
    const char *layer_names[] = {"VK_LAYER_KHRONOS_validation"};

    unsigned int extension_count = 0;
    auto extension_names = glfwGetRequiredInstanceExtensions(&extension_count);

    VkApplicationInfo application_info;
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pNext = nullptr;
    application_info.pApplicationName = "My Application";
    application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.pEngineName = "No Engine";
    application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instance_create_info;
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pNext = nullptr;
    instance_create_info.flags = 0;
    instance_create_info.pApplicationInfo = &application_info;
    instance_create_info.enabledLayerCount = layer_count;
    instance_create_info.ppEnabledLayerNames = layer_names;
    instance_create_info.enabledExtensionCount = extension_count;
    instance_create_info.ppEnabledExtensionNames = extension_names;

    // TODO: Select layers and extensions properly and check if they are available.

    if (vkCreateInstance(&instance_create_info, nullptr, &handle) != VK_SUCCESS)
    {
        std::cerr << "Failed to create instance." << std::endl;
        throw 1;
    }
}

inline Instance::~Instance()
{
    vkDestroyInstance(handle, nullptr);
}

inline const VkInstance &Instance::get_handle() const
{
    return handle;
}

}
