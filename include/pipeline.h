#ifndef PIPELINE_H
#define PIPELINE_H

#include <vulkan/vulkan.h>

VkPipelineLayout pipeline_layout_create(const VkDevice device);

void pipeline_layout_destroy(const VkPipelineLayout pipeline_layout, const VkDevice device);

VkPipeline graphics_pipeline_create(
    const VkDevice device,
    const VkSurfaceCapabilitiesKHR surface_capabilities,
    const VkRenderPass render_pass,
    const VkShaderModule vertex_shader_module,
    const VkShaderModule fragment_shader_module,
    const VkPipelineLayout pipeline_layout
);

void pipeline_destroy(const VkDevice device, const VkPipeline pipeline);

#endif
