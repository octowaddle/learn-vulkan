#include <context.h>

#include <buffer.h>
#include <commandbuffer.h>
#include <device.h>
#include <framebuffer.h>
#include <instance.h>
#include <pipeline.h>
#include <queue.h>
#include <renderpass.h>
#include <shadermodule.h>
#include <swapchain.h>
#include <window.h>

#include <stdio.h>
#include <stdlib.h>

struct context *context_create(GLFWwindow *window) {
    struct context *context = malloc(sizeof *context);

    const uint32_t instance_layer_count = 1;
    const char *const instance_layer_names[] = {"VK_LAYER_KHRONOS_validation"};
    uint32_t instance_extension_count = 0;
    const char *const *const instance_extension_names = glfwGetRequiredInstanceExtensions(&instance_extension_count);
    context->instance = instance_create(instance_layer_count, instance_layer_names , instance_extension_count, instance_extension_names);

    context->physical_device = instance_choose_physical_device(context->instance);
    context->physical_device_properties = physical_device_get_properties(context->physical_device);
    context->physical_device_memory_properties = physical_device_get_memory_properties(context->physical_device);

    context->queue_family_index = physical_device_find_queue_family_index(context->physical_device, VK_QUEUE_GRAPHICS_BIT);

    context->device = device_create(context->physical_device, context->queue_family_index);

    context->surface = window_create_surface(window, context->instance);
    context->surface_format = surface_choose_format(context->surface, context->physical_device);
    context->surface_capabilities = surface_get_capabilities(context->surface, context->physical_device);

    const uint32_t swapchain_min_image_count = swapchain_choose_min_image_count(context->surface_capabilities);
    context->swapchain = swapchain_create(context->physical_device, context->device, context->surface, context->surface_format, context->surface_capabilities, VK_NULL_HANDLE, context->queue_family_index, swapchain_min_image_count);
    context->swapchain_image_count = swapchain_get_image_count(context->swapchain, context->device);

    context->image_views = swapchain_create_image_views(context->swapchain, context->device, context->surface_format, context->swapchain_image_count);

    const VkShaderModule vertex_shader_module = shader_module_create(context->device, "vert.spv");
    const VkShaderModule fragment_shader_module = shader_module_create(context->device, "frag.spv");

    context->render_pass = render_pass_create(context->device, context->surface_format);

    context->graphics_pipeline_layout = pipeline_layout_create(context->device);
    context->graphics_pipeline = graphics_pipeline_create(context->device, context->surface_capabilities, context->render_pass, vertex_shader_module, fragment_shader_module, context->graphics_pipeline_layout);

    context->framebuffers = framebuffers_create(context->device, context->surface_capabilities, context->image_views, context->render_pass, context->swapchain_image_count);

    context->command_pool = command_pool_create(context->device, context->queue_family_index);

    context->queue = device_get_queue(context->device, context->queue_family_index);

    context->semaphore_image_available = semaphore_create(context->device);
    context->semaphore_image_rendered = semaphore_create(context->device);

    context->fences = fences_create(context->device, context->swapchain_image_count);
    return context;
}

void context_record_command_buffers(struct context *context, const VkBuffer vertex_buffer, const uint32_t vertex_count) {
    context->command_buffers = command_buffer_create_draw(
        context->device,
        context->surface_capabilities,
        context->command_pool,
        context->render_pass,
        context->framebuffers,
        context->graphics_pipeline,
        vertex_buffer,
        context->swapchain_image_count,
        vertex_count);
}

void context_destroy(struct context *context) {
    fences_destroy(context->device, context->fences, context->swapchain_image_count);
    semaphore_destroy(context->device, context->semaphore_image_rendered);
    semaphore_destroy(context->device, context->semaphore_image_available);
    command_buffers_free(context->device, context->command_pool, context->command_buffers, context->swapchain_image_count);
    command_pool_destroy(context->device, context->command_pool);
    framebuffers_destroy(context->device, context->framebuffers, context->swapchain_image_count);
    pipeline_destroy(context->device, context->graphics_pipeline);
    pipeline_layout_destroy(context->graphics_pipeline_layout, context->device);
    render_pass_destroy(context->render_pass, context->device);
    swapchain_image_views_destroy(context->image_views, context->device, context->swapchain_image_count);
    swapchain_destroy(context->swapchain, context->device);
    surface_destroy(context->surface, context->instance);
    device_destroy(context->device);
    instance_destroy(context->instance);
}

void context_recreate_swapchain(struct context *context, const uint32_t width, const uint32_t height) {
    vkDeviceWaitIdle(context->device);

    VkSwapchainKHR old_swapchain = context->swapchain;

    command_buffers_free(context->device, context->command_pool, context->command_buffers, context->swapchain_image_count);
    command_pool_destroy(context->device, context->command_pool);
    framebuffers_destroy(context->device, context->framebuffers, context->swapchain_image_count);
    pipeline_destroy(context->device, context->graphics_pipeline);
    pipeline_layout_destroy(context->graphics_pipeline_layout, context->device);
    render_pass_destroy(context->render_pass, context->device);
    swapchain_image_views_destroy(context->image_views, context->device, context->swapchain_image_count);

    VkSurfaceCapabilitiesKHR surface_capabilities = surface_get_capabilities(context->surface, context->physical_device);
    context->surface_format = surface_choose_format(context->surface, context->physical_device);

    const uint32_t swapchain_min_image_count = swapchain_choose_min_image_count(context->surface_capabilities);
    context->swapchain = swapchain_create(context->physical_device, context->device, context->surface, context->surface_format, surface_capabilities, old_swapchain, context->queue_family_index, swapchain_min_image_count);

    swapchain_destroy(old_swapchain, context->device);

    context->image_views = swapchain_create_image_views(context->swapchain, context->device, context->surface_format, context->swapchain_image_count);

    context->render_pass = render_pass_create(context->device, context->surface_format);

    const VkShaderModule vertex_shader_module = shader_module_create(context->device, "vert.spv");
    const VkShaderModule fragment_shader_module = shader_module_create(context->device, "frag.spv");

    context->graphics_pipeline_layout = pipeline_layout_create(context->device);
    context->graphics_pipeline = graphics_pipeline_create(context->device, surface_capabilities, context->render_pass, vertex_shader_module, fragment_shader_module, context->graphics_pipeline_layout);

    context->framebuffers = framebuffers_create(context->device, surface_capabilities, context->image_views, context->render_pass, context->swapchain_image_count);
    context->command_pool = command_pool_create(context->device, context->queue_family_index);
}
