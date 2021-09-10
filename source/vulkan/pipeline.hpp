#pragma once

#include "external.hpp"
#include "noncopyable.hpp"
#include "device.hpp"
#include "renderpass.hpp"
#include "shader.hpp"
#include "surface.hpp"

namespace vulkan
{

class Pipeline : NonCopyable
{
public:
    inline Pipeline(const Device &device, const Surface &surface, const RenderPass &render_pass, const Shader &vertex_shader, const Shader &fragment_shader);

    inline ~Pipeline();

    inline const VkPipeline &get_handle() const;

private:
    VkPipeline handle = VK_NULL_HANDLE;

    VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;

    VkDevice device;
};

inline Pipeline::Pipeline(const Device &device, const Surface &surface, const RenderPass &render_pass, const Shader &vertex_shader, const Shader &fragment_shader)
: device(device.get_handle())
{
    VkPipelineShaderStageCreateInfo pipeline_vertex_shader_stage_create_info;
    pipeline_vertex_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipeline_vertex_shader_stage_create_info.pNext = nullptr;
    pipeline_vertex_shader_stage_create_info.flags = 0;
    pipeline_vertex_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    pipeline_vertex_shader_stage_create_info.module = vertex_shader.get_handle();
    pipeline_vertex_shader_stage_create_info.pName = "main";
    pipeline_vertex_shader_stage_create_info.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo pipeline_fragment_shader_stage_create_info;
    pipeline_fragment_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipeline_fragment_shader_stage_create_info.pNext = nullptr;
    pipeline_fragment_shader_stage_create_info.flags = 0;
    pipeline_fragment_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    pipeline_fragment_shader_stage_create_info.module = fragment_shader.get_handle();
    pipeline_fragment_shader_stage_create_info.pName = "main";
    pipeline_fragment_shader_stage_create_info.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_infos[] = {
        pipeline_vertex_shader_stage_create_info,
        pipeline_fragment_shader_stage_create_info};

    VkPipelineLayoutCreateInfo pipeline_layout_create_info;
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.pNext = nullptr;
    pipeline_layout_create_info.flags = 0;
    pipeline_layout_create_info.setLayoutCount = 0;
    pipeline_layout_create_info.pSetLayouts = nullptr;
    pipeline_layout_create_info.pushConstantRangeCount = 0;
    pipeline_layout_create_info.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(device.get_handle(), &pipeline_layout_create_info, nullptr, &pipeline_layout) != VK_SUCCESS)
    {
        std::cerr << "Failed to create pipeline layout." << std::endl;
        throw 1;
    }

    VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info;
    pipeline_vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipeline_vertex_input_state_create_info.pNext = nullptr;
    pipeline_vertex_input_state_create_info.flags = 0;
    pipeline_vertex_input_state_create_info.vertexBindingDescriptionCount = 0;
    pipeline_vertex_input_state_create_info.pVertexBindingDescriptions = nullptr;
    pipeline_vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;
    pipeline_vertex_input_state_create_info.pVertexAttributeDescriptions = nullptr;

    VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info;
    pipeline_input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipeline_input_assembly_state_create_info.pNext = nullptr;
    pipeline_input_assembly_state_create_info.flags = 0;
    pipeline_input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipeline_input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport;
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = surface.get_capabilities().currentExtent.width;
    viewport.height = surface.get_capabilities().currentExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = surface.get_capabilities().currentExtent.width;
    scissor.extent.height = surface.get_capabilities().currentExtent.width;

    VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info;
    pipeline_viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pipeline_viewport_state_create_info.pNext = nullptr;
    pipeline_viewport_state_create_info.flags = 0;
    pipeline_viewport_state_create_info.viewportCount = 1;
    pipeline_viewport_state_create_info.pViewports = &viewport;
    pipeline_viewport_state_create_info.scissorCount = 1;
    pipeline_viewport_state_create_info.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo pipeline_restirazion_state_create_info;
    pipeline_restirazion_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pipeline_restirazion_state_create_info.pNext = nullptr;
    pipeline_restirazion_state_create_info.flags = 0;
    pipeline_restirazion_state_create_info.depthClampEnable = VK_FALSE;
    pipeline_restirazion_state_create_info.rasterizerDiscardEnable = VK_FALSE;
    pipeline_restirazion_state_create_info.polygonMode = VK_POLYGON_MODE_FILL; // Change this for wireframe (requires extension?)
    pipeline_restirazion_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
    pipeline_restirazion_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    pipeline_restirazion_state_create_info.depthBiasEnable = VK_FALSE;
    pipeline_restirazion_state_create_info.depthBiasConstantFactor = 0.0f;
    pipeline_restirazion_state_create_info.depthBiasClamp = 0.0f;
    pipeline_restirazion_state_create_info.depthBiasSlopeFactor = 0.0f;
    pipeline_restirazion_state_create_info.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info;
    pipeline_multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pipeline_multisample_state_create_info.pNext = nullptr;
    pipeline_multisample_state_create_info.flags = 0;
    pipeline_multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // Can be higher for better image/worse performance.
    pipeline_multisample_state_create_info.sampleShadingEnable = VK_FALSE;
    pipeline_multisample_state_create_info.minSampleShading = 1.0f;
    pipeline_multisample_state_create_info.pSampleMask = nullptr;
    pipeline_multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
    pipeline_multisample_state_create_info.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state;
    pipeline_color_blend_attachment_state.blendEnable = VK_TRUE;
    pipeline_color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    pipeline_color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    pipeline_color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
    pipeline_color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    pipeline_color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    pipeline_color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
    pipeline_color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info;
    pipeline_color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pipeline_color_blend_state_create_info.pNext = nullptr;
    pipeline_color_blend_state_create_info.flags = 0;
    pipeline_color_blend_state_create_info.logicOpEnable = VK_FALSE;
    pipeline_color_blend_state_create_info.logicOp = VK_LOGIC_OP_NO_OP;
    pipeline_color_blend_state_create_info.attachmentCount = 1;
    pipeline_color_blend_state_create_info.pAttachments = &pipeline_color_blend_attachment_state;
    pipeline_color_blend_state_create_info.blendConstants[0] = 0.0f;
    pipeline_color_blend_state_create_info.blendConstants[1] = 0.0f;
    pipeline_color_blend_state_create_info.blendConstants[2] = 0.0f;
    pipeline_color_blend_state_create_info.blendConstants[3] = 0.0f;

    VkGraphicsPipelineCreateInfo graphics_pipeline_create_info;
    graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_create_info.pNext = nullptr;
    graphics_pipeline_create_info.flags = 0;
    graphics_pipeline_create_info.stageCount = 2;
    graphics_pipeline_create_info.pStages = pipeline_shader_stage_create_infos;
    graphics_pipeline_create_info.pVertexInputState = &pipeline_vertex_input_state_create_info;
    graphics_pipeline_create_info.pInputAssemblyState = &pipeline_input_assembly_state_create_info;
    graphics_pipeline_create_info.pTessellationState = nullptr;
    graphics_pipeline_create_info.pViewportState = &pipeline_viewport_state_create_info;
    graphics_pipeline_create_info.pRasterizationState = &pipeline_restirazion_state_create_info;
    graphics_pipeline_create_info.pMultisampleState = &pipeline_multisample_state_create_info;
    graphics_pipeline_create_info.pDepthStencilState = nullptr;
    graphics_pipeline_create_info.pColorBlendState = &pipeline_color_blend_state_create_info;
    graphics_pipeline_create_info.pDynamicState = nullptr;
    graphics_pipeline_create_info.layout = pipeline_layout;
    graphics_pipeline_create_info.renderPass = render_pass.get_handle();
    graphics_pipeline_create_info.subpass = 0; // Index of subpass in render pass
    graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    graphics_pipeline_create_info.basePipelineIndex = -1; // -1 is an invalid index (in contrast to 0).

    if (vkCreateGraphicsPipelines(device.get_handle(), VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &handle) != VK_SUCCESS)
    {
        std::cerr << "Failed to create pipeline." << std::endl;
        throw 1;
    }

    // TODO: Delete shaders early.
};

inline Pipeline::~Pipeline()
{
    vkDestroyPipeline(device, handle , nullptr);
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
}

inline const VkPipeline &Pipeline::get_handle() const
{
    return handle;
}

}
