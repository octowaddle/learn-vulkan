#include <pipeline.h>

#include <stdio.h>
#include <stdlib.h>

VkPipelineLayout pipeline_layout_create(const VkDevice device) {
    const VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = 0,
        .pSetLayouts = NULL,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL
    };

    VkPipelineLayout pipeline_layout;
    VkResult result = vkCreatePipelineLayout(device, &pipeline_layout_create_info, NULL, &pipeline_layout);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create pipeline layout\n");
        exit(1);
    }

    return pipeline_layout;
}

void pipeline_layout_destroy(const VkPipelineLayout pipeline_layout, const VkDevice device) {
    vkDestroyPipelineLayout(device, pipeline_layout, NULL);
}

VkPipeline graphics_pipeline_create(
    const VkDevice device,
    const VkSurfaceCapabilitiesKHR surface_capabilities,
    const VkRenderPass render_pass,
    const VkShaderModule vertex_shader_module,
    const VkShaderModule fragment_shader_module,
    const VkPipelineLayout graphics_pipeline_layout
) {
    // Refernce structure of vertex data array. It is actually created in the
    // vertex buffer section.

    // const float vertex_data[] = {
    //     0.0f, -0.5f,       // Position #1 // Vertex #1
    //     1.0f,  0.0f, 0.0f, // Color #1    //

    //    -0.5f,  0.5f,       // Position #2 // Vertex #2
    //     0.0f,  1.0f, 0.0f, // Color #2    //

    //     0.5f,  0.5f,       // Position #3 // Vertex #3
    //     0.0f,  0.0f, 1.0f  // Color #3    //
    // };

    // TODO Not a hard-coded vertex buffer layout (offset and stride).

    const VkVertexInputBindingDescription vertex_input_bindings_description = {
        .binding = 0,
        .stride = 2 * sizeof (float) + 3 * sizeof (float),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    const VkVertexInputAttributeDescription vertex_input_attribute_description[] = {
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = 0 * sizeof (float),
        },
        {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = 2 * sizeof (float),
        },
    };

    const VkPipelineShaderStageCreateInfo graphics_pipeline_vertex_shader_stage_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertex_shader_module,
        .pName = "main",
        .pSpecializationInfo = NULL
    };

    const VkPipelineShaderStageCreateInfo graphics_pipeline_fragment_shader_stage_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragment_shader_module,
        .pName = "main",
        .pSpecializationInfo = NULL
    };

    const VkPipelineShaderStageCreateInfo graphics_pipeline_shader_stage_create_infos[] = {
        graphics_pipeline_vertex_shader_stage_create_info,
        graphics_pipeline_fragment_shader_stage_create_info
    };

    const VkPipelineVertexInputStateCreateInfo graphics_pipeline_vertex_input_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &vertex_input_bindings_description,
        .vertexAttributeDescriptionCount = 2,
        .pVertexAttributeDescriptions = vertex_input_attribute_description
    };

    const VkPipelineInputAssemblyStateCreateInfo graphics_pipeline_input_assembly_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    const VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    const VkPipelineDynamicStateCreateInfo graphics_pipeline_dynamic_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .dynamicStateCount = 2,
        .pDynamicStates = dynamic_states,
    };

    const VkViewport graphics_pipeline_viewport = {
        .x = 0,
        .y = 0,
        .width = surface_capabilities.currentExtent.width,
        .height = surface_capabilities.currentExtent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    const VkRect2D graphics_pipeline_scissor = {
        .offset.x = 0,
        .offset.y = 0,
        .extent.width = surface_capabilities.currentExtent.width,
        .extent.height = surface_capabilities.currentExtent.height
    };

    const VkPipelineViewportStateCreateInfo graphics_pipeline_viewport_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = &graphics_pipeline_viewport,
        .scissorCount = 1,
        .pScissors = &graphics_pipeline_scissor
    };

    const VkPipelineRasterizationStateCreateInfo graphics_pipeline_restirazion_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f
    };

    const VkPipelineMultisampleStateCreateInfo graphics_pipeline_multisample_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,
        .pSampleMask = NULL,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE
    };

    const VkPipelineColorBlendAttachmentState graphics_pipeline_color_blend_attachment_state = {
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };

    const VkPipelineColorBlendStateCreateInfo graphics_pipeline_color_blend_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_NO_OP,
        .attachmentCount = 1,
        .pAttachments = &graphics_pipeline_color_blend_attachment_state,
        .blendConstants[0] = 0.0f,
        .blendConstants[1] = 0.0f,
        .blendConstants[2] = 0.0f,
        .blendConstants[3] = 0.0f
    };

    const VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stageCount = (sizeof graphics_pipeline_shader_stage_create_infos) / (sizeof *graphics_pipeline_shader_stage_create_infos),
        .pStages = graphics_pipeline_shader_stage_create_infos,
        .pVertexInputState = &graphics_pipeline_vertex_input_state_create_info,
        .pInputAssemblyState = &graphics_pipeline_input_assembly_state_create_info,
        .pTessellationState = NULL,
        .pViewportState = &graphics_pipeline_viewport_state_create_info,
        .pRasterizationState = &graphics_pipeline_restirazion_state_create_info,
        .pMultisampleState = &graphics_pipeline_multisample_state_create_info,
        .pDepthStencilState = NULL,
        .pColorBlendState = &graphics_pipeline_color_blend_state_create_info,
        .pDynamicState = &graphics_pipeline_dynamic_state_create_info,
        .layout = graphics_pipeline_layout,
        .renderPass = render_pass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };

    VkPipeline graphics_pipeline = VK_NULL_HANDLE;
    VkResult result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, NULL, &graphics_pipeline);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create graphics pipeline\n");
        exit(1);
    }

    vkDestroyShaderModule(device, fragment_shader_module, NULL);
    vkDestroyShaderModule(device, vertex_shader_module, NULL);

    return graphics_pipeline;
}

void pipeline_destroy(const VkDevice device, const VkPipeline pipeline) {
    vkDestroyPipeline(device, pipeline, NULL);
}
