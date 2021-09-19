#define GLFW_INCLUDE_VULKAN

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <device.h>
#include <instance.h>
#include <renderpass.h>
#include <shadermodule.h>
#include <swapchain.h>
#include <window.h>
#include <pipeline.h>
#include <framebuffer.h>
#include <commandbuffer.h>
#include <buffer.h>
#include <queue.h>
#include <semaphore.h>

#ifdef DEBUG

uint64_t mallocd_count = 0;
uint64_t freed_count = 0;

void *debug__malloc(const size_t size, const char *const what, const char *const file, const uint64_t line) {
    mallocd_count++;
    void *ptr = malloc(size);
    printf("debug: malloc(%s): allocated %lu bytes at %p in file %s:%lu with a total allocation count of %lu\n", what, size, ptr, file, line, mallocd_count);
    return ptr;
}

void debug__free(void *ptr, const char *const what, const char *const file, const uint64_t line) {
    freed_count++;
    free(ptr);
    printf("debug: free(%s): freed memory at %p in file %s:%lu with a total free count of %lu\n", what, ptr, file, line, freed_count);
}

#define malloc(size) debug__malloc(size, #size, __FILE__, __LINE__)
#define free(ptr) debug__free(ptr, #ptr, __FILE__, __LINE__)

#endif

int main() {
    VkResult result;

    //
    // Initialize external libraries.

    if (!glfwInit()) {
        fprintf(stderr, "error: failed to initialize GLFW.\n");
        exit(1);
    }

    //
    // Create an instance.

    const uint32_t instance_layer_count = 1;
    const char *const instance_layer_names[] = {"VK_LAYER_KHRONOS_validation"};

    uint32_t instance_extension_count = 0;
    const char *const *const instance_extension_names = glfwGetRequiredInstanceExtensions(&instance_extension_count);

    const VkInstance instance = instance_create(instance_layer_count, instance_layer_names , instance_extension_count, instance_extension_names);

    //
    // Choose a physical device.

    const VkPhysicalDevice physical_device = instance_choose_physical_device(instance);
    const VkPhysicalDeviceProperties physical_device_properties = physical_device_get_properties(physical_device);
    const VkPhysicalDeviceMemoryProperties physical_device_memory_properties = physical_device_get_memory_properties(physical_device);

    //
    // Choose a queue family.

    const uint32_t queue_family_index = physical_device_find_queue_family_index(physical_device, VK_QUEUE_GRAPHICS_BIT);

    //
    // Create a device.

    const VkDevice device = device_create(physical_device, queue_family_index);

    //
    // Create a window.

    GLFWwindow *window = window_create(1280, 720);

    //
    // Create a surface.

    const VkSurfaceKHR surface = window_create_surface(window, instance);
    const VkSurfaceFormatKHR surface_format = surface_choose_format(surface, physical_device);
    const VkSurfaceCapabilitiesKHR surface_capabilities = surface_get_capabilities(surface, physical_device);

    //
    // Create a swapchain.

    const uint32_t swapchain_min_image_count = swapchain_choose_min_image_count(surface_capabilities);
    const VkSwapchainKHR swapchain = swapchain_create(physical_device, device, surface, surface_format, surface_capabilities, queue_family_index, swapchain_min_image_count);
    const uint32_t swapchain_image_count = swapchain_get_image_count(swapchain, device);

    //
    // Create image views.

    VkImageView *image_views = swapchain_create_image_views(swapchain, device, surface_format, swapchain_image_count);

    //
    // Create shader modules.

    const VkShaderModule vertex_shader_module = shader_module_create(device, "vert.spv");
    const VkShaderModule fragment_shader_module = shader_module_create(device, "frag.spv");

    //
    // Create a render pass.

    const VkRenderPass render_pass = render_pass_create(device, surface_format);

    //
    // Create a pipeline.

    const VkPipelineLayout graphics_pipeline_layout = pipeline_layout_create(device);
    const VkPipeline graphics_pipeline = graphics_pipeline_create(device, surface_capabilities, render_pass, vertex_shader_module, fragment_shader_module, graphics_pipeline_layout);

    //
    // Create framebuffers.

    VkFramebuffer *framebuffers = framebuffers_create(device, surface_capabilities, image_views, render_pass, swapchain_image_count);


    //
    // Create a command pool.

    const VkCommandPool command_pool = command_pool_create(device, queue_family_index);

    //
    // Get device queue.

    const VkQueue queue = device_get_queue(device, queue_family_index);

    //
    // Create a staging buffer

    // TODO Not a hard-coded vertex count (size).

    const float vertex_data[] = {
       -0.5f, -0.5f,       // Position #1 // Vertex #1
        1.0f,  0.0f, 0.0f, // Color #1    //

       -0.5f,  0.5f,       // Position #2 // Vertex #2
        0.0f,  1.0f, 0.0f, // Color #2    //

        0.5f,  0.5f,       // Position #3 // Vertex #3
        0.0f,  0.0f, 1.0f, // Color #3    //

       -0.5f, -0.5f,       // Position #1 // Vertex #1
        1.0f,  0.0f, 0.0f, // Color #1    //

        0.5f,  0.5f,       // Position #3 // Vertex #3
        0.0f,  0.0f, 1.0f, // Color #3    //

        0.5f, -0.5f,       // Position #4 // Vertex #4
        1.0f,  1.0f, 1.0f  // Color #4    //
    };

    const uint32_t vertex_count = 6;

    const uint32_t buffer_size = vertex_count * (2 * sizeof (float) + 3 * sizeof (float));

    const VkBuffer staging_buffer = buffer_create(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, buffer_size);

    const VkDeviceMemory staging_buffer_device_memory = buffer_allocate_device_memory(staging_buffer, device, physical_device_memory_properties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    buffer_upload_data(staging_buffer, device, staging_buffer_device_memory, buffer_size, vertex_data);

    //
    // Create a vertex buffer.

    const VkBuffer vertex_buffer = buffer_create(device, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, buffer_size);
    const VkDeviceMemory vertex_buffer_device_memory = buffer_allocate_device_memory(vertex_buffer, device, physical_device_memory_properties, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    buffer_copy_data(staging_buffer, vertex_buffer, device, command_pool, queue, buffer_size);

    buffer_destroy(device, staging_buffer);
    buffer_free_memory(device, staging_buffer_device_memory);

    //
    // Create command buffers.
    VkCommandBuffer *command_buffers = command_buffer_create_draw(device, surface_capabilities, command_pool, render_pass, framebuffers, graphics_pipeline, vertex_buffer, swapchain_image_count, vertex_count);

    //
    // Create semaphores.

    const VkSemaphore semaphore_image_available = semaphore_create(device);
    const VkSemaphore semaphore_image_rendered = semaphore_create(device);

    //
    // Create fences.

    VkFence *fences = fences_create(device, swapchain_image_count);

    //
    // Main loop.

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        queue_draw(queue, device, swapchain, command_buffers, semaphore_image_available, semaphore_image_rendered, fences);
    }

    vkDeviceWaitIdle(device);

    //
    // Clean up.

    buffer_destroy(device, vertex_buffer);
    buffer_free_memory(device, vertex_buffer_device_memory);

    fences_destroy(device, fences, swapchain_image_count);

    semaphore_destroy(device, semaphore_image_rendered);
    semaphore_destroy(device, semaphore_image_available);

    command_buffers_free(device, command_pool, command_buffers, swapchain_image_count);

    command_pool_destroy(device, command_pool);

    framebuffers_destroy(device, framebuffers, swapchain_image_count);

    pipeline_destroy(device, graphics_pipeline);

    pipeline_layout_destroy(graphics_pipeline_layout, device);

    render_pass_destroy(render_pass, device);

    // if (fragment_shader_module != VK_NULL_HANDLE) {
    //     vkDestroyShaderModule(device, fragment_shader_module, NULL);
    // }

    // if (vertex_shader_module != VK_NULL_HANDLE) {
    //     vkDestroyShaderModule(device, vertex_shader_module, NULL);
    // }

    swapchain_image_views_destroy(image_views, device, swapchain_image_count);

    swapchain_destroy(swapchain, device);

    surface_destroy(surface, instance);

    glfwDestroyWindow(window);
    glfwTerminate();

    device_destroy(device);

    instance_destroy(instance);
}
