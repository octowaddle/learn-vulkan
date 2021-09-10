#pragma once

#include "external.hpp"
#include "noncopyable.hpp"
#include "instance.hpp"
#include "physical.hpp"

namespace vulkan
{

class Surface : NonCopyable
{
public:
    inline Surface(const Instance &instance, const Physical &physical, GLFWwindow *window);

    inline ~Surface();

    inline const VkSurfaceKHR &get_handle() const;

    inline const VkSurfaceFormatKHR &get_format() const;

    inline const VkSurfaceCapabilitiesKHR &get_capabilities() const;

private:
    VkSurfaceKHR handle;

    VkSurfaceFormatKHR format;

    VkSurfaceCapabilitiesKHR capabilities;

    VkInstance instance;
};

inline Surface::Surface(const Instance &instance, const Physical &physical, GLFWwindow *window)
: instance(instance.get_handle())
{
    ///////////////////////////////////////////////////////////////////////////
    /// Create surface using GLFW.

    if (glfwCreateWindowSurface(instance.get_handle(), window, nullptr, &handle) != VK_SUCCESS)
    {
        std::cerr << "Failed to create window surface." << std::endl;
        throw 1;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Fetch format and check if it is supported.

    unsigned int surface_format_count = 0;

    if (vkGetPhysicalDeviceSurfaceFormatsKHR(physical.get_handle(), handle, &surface_format_count, nullptr) != VK_SUCCESS)
    {
        std::cerr << "Failed to find surface formats." << std::endl;
        throw 1;
    }

    if (surface_format_count == 0)
    {
        std::cerr << "No surface formats are found." << std::endl;
        throw 1;
    }

    VkSurfaceFormatKHR *formats = new VkSurfaceFormatKHR[surface_format_count];

    if (vkGetPhysicalDeviceSurfaceFormatsKHR(physical.get_handle(), handle, &surface_format_count, formats) != VK_SUCCESS)
    {
        std::cerr << "Failed to fetch surface formats." << std::endl;
        throw 1;
    }

    for (unsigned int i = 0; i < surface_format_count; i++)
    {
        // TODO: Choose surface format properly.
        if (formats[i].format == VK_FORMAT_R8G8B8A8_UNORM || formats[i].format == VK_FORMAT_B8G8R8A8_UNORM)
        {
            format = formats[i];
            break;
        }
    }

    delete[] formats;

    if (format.format == VK_FORMAT_UNDEFINED)
    {
        std::cerr << "Surface does not support R8G8B8A8 format." << std::endl;
        throw 1;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Fetch capabilities.

    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical.get_handle(), handle, &capabilities) != VK_SUCCESS)
    {
        std::cerr << "Failed to fetch surface capabilities." << std::endl;
        throw 1;
    }
}

inline Surface::~Surface()
{
    vkDestroySurfaceKHR(instance, handle, nullptr);
}

inline const VkSurfaceCapabilitiesKHR &Surface::get_capabilities() const
{
    return capabilities;
}

inline const VkSurfaceFormatKHR &Surface::get_format() const
{
    return format;
}

inline const VkSurfaceKHR &Surface::get_handle() const
{
    return handle;
}

}
