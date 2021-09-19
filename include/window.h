#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

GLFWwindow *window_create(const uint32_t width, const uint32_t height);

VkSurfaceKHR window_create_surface(GLFWwindow *window, const VkInstance instance);

void surface_destroy(const VkSurfaceKHR surface, const VkInstance instance);

VkSurfaceFormatKHR surface_choose_format(const VkSurfaceKHR surface, const VkPhysicalDevice physical_device);

VkSurfaceCapabilitiesKHR surface_get_capabilities(const VkSurfaceKHR surface, const VkPhysicalDevice physical_device);

#endif
