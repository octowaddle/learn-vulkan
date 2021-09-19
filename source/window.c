#include <window.h>

#include <stdio.h>
#include <stdlib.h>

GLFWwindow *window_create(const uint32_t width, const uint32_t height) {
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "Vulkan Application", NULL, NULL);

    if (window == NULL) {
        fprintf(stderr, "error: failed to create window\n");
        exit(1);
    }

    return window;
}

VkSurfaceKHR window_create_surface(GLFWwindow *window, const VkInstance instance) {
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    glfwCreateWindowSurface(instance, window, NULL, &surface);

    return surface;
}

void surface_destroy(const VkSurfaceKHR surface, const VkInstance instance) {
    vkDestroySurfaceKHR(instance, surface, NULL);
}

VkSurfaceFormatKHR surface_choose_format(const VkSurfaceKHR surface, const VkPhysicalDevice physical_device) {
    uint32_t surface_format_count = 0;
    VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, NULL);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to get surface formats\n");
        exit(1);
    }


    VkSurfaceFormatKHR *surface_formats = malloc(surface_format_count * (sizeof *surface_formats));
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, surface_formats);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to get surface formats\n");
        exit(1);
    }

    if (surface_format_count == 0) {
        fprintf(stderr, "error: no surface formats are available\n");
        exit(1);
    }

    VkSurfaceFormatKHR surface_format = surface_formats[0];

    // TODO Improve filtering for surface formats.

    for (uint32_t surface_format_index = 0U; surface_format_index < surface_format_count; ++surface_format_index) {
        if (surface_formats[surface_format_index].format == VK_FORMAT_R8G8B8A8_UNORM || surface_formats[surface_format_index].format == VK_FORMAT_B8G8R8A8_UNORM) {
            surface_format = surface_formats[surface_format_index];
            break;
        }
    }

    free(surface_formats);

    if (surface_format.format == VK_FORMAT_UNDEFINED) {
        fprintf(stderr, "error: surface has an undefined format\n");
        exit(1);
    }

    return surface_format;
}

VkSurfaceCapabilitiesKHR surface_get_capabilities(const VkSurfaceKHR surface, const VkPhysicalDevice physical_device)
{
    VkSurfaceCapabilitiesKHR surface_capabilities;
    VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to get surface capabilities\n");
        exit(1);
    }

    return surface_capabilities;
}
