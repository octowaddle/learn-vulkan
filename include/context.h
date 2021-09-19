#ifndef CONTEXT_H
#define CONTEXT_H

#include <vulkan/vulkan.h>

#include <window.h>

struct context {
    VkInstance instance;
    VkPhysicalDevice physical_device;
    VkPhysicalDeviceProperties physical_device_properties;
    VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
    VkDevice device;
    VkSurfaceKHR surface;
    VkSurfaceFormatKHR surface_format;
    VkSurfaceCapabilitiesKHR surface_capabilities;
    VkSwapchainKHR swapchain;
    VkImageView *image_views;
    VkRenderPass render_pass;
    VkPipelineLayout graphics_pipeline_layout;
    VkPipeline graphics_pipeline;
    VkFramebuffer *framebuffers;
    VkCommandPool command_pool;
    VkQueue queue;
    VkCommandBuffer *command_buffers;
    VkSemaphore semaphore_image_available;
    VkSemaphore semaphore_image_rendered;
    VkFence *fences;
    uint32_t swapchain_image_count;
    uint32_t queue_family_index;
};

struct context *context_create(GLFWwindow *window);

void context_record_command_buffers(struct context *context, const VkBuffer vertex_buffer, const uint32_t vertex_count);

void context_destroy(struct context *context);

void context_recreate_swapchain(struct context *context, const uint32_t width, const uint32_t height);

#endif
