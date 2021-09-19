#include <context.h>

#include <window.h>
#include <buffer.h>
#include <queue.h>

#include <stdlib.h>
#include <stdio.h>

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

struct context *context = NULL;
VkBuffer vertex_buffer = NULL;

void on_window_resize(GLFWwindow *window, int width, int height) {
    if (width == 0 || height == 0) return;

    context_recreate_swapchain(context, width, height);
    context_record_command_buffers(context, vertex_buffer, vertex_count);
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "error: failed to initialize GLFW.\n");
        exit(1);
    }

    GLFWwindow *window = window_create(1280, 720);

    context = context_create(window);

    const uint32_t buffer_size = vertex_count * (2 * sizeof (float) + 3 * sizeof (float));

    const VkBuffer staging_buffer = buffer_create(context->device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, buffer_size);

    const VkDeviceMemory staging_buffer_device_memory = buffer_allocate_device_memory(staging_buffer, context->device, context->physical_device_memory_properties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    buffer_upload_data(staging_buffer, context->device, staging_buffer_device_memory, buffer_size, vertex_data);

    //
    // Create a vertex buffer.

    vertex_buffer = buffer_create(context->device, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, buffer_size);
    const VkDeviceMemory vertex_buffer_device_memory = buffer_allocate_device_memory(vertex_buffer, context->device, context->physical_device_memory_properties, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    buffer_copy_data(staging_buffer, vertex_buffer, context->device, context->command_pool, context->queue, buffer_size);

    buffer_destroy(context->device, staging_buffer);
    buffer_free_memory(context->device, staging_buffer_device_memory);

    context_record_command_buffers(context, vertex_buffer, vertex_count);

    glfwSetWindowSizeCallback(window, on_window_resize);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        queue_draw(context->queue, context->device, context->swapchain, context->command_buffers, context->semaphore_image_available, context->semaphore_image_rendered, context->fences);
    }

    vkDeviceWaitIdle(context->device);

    buffer_free_memory(context->device, vertex_buffer_device_memory);
    buffer_destroy(context->device, vertex_buffer);
    context_destroy(context);
}
