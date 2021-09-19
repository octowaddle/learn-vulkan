#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <vulkan/vulkan.h>

VkRenderPass render_pass_create(const VkDevice device, const VkSurfaceFormatKHR surface_format);

void render_pass_destroy(const VkRenderPass render_pass, const VkDevice device);

#endif
