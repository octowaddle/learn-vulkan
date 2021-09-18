#pragma once

#include "external.hpp"

namespace vulkan
{

namespace impl
{

using Device = VkDevice;
using Framebuffer = VkFramebuffer;
using Instance = VkInstance;
using Queue = VkQueue;
using RenderPass = VkRenderPass;
using ShaderModule = VkShaderModule;
using Surface = VkSurfaceKHR;
using PhysicalDevice = VkPhysicalDevice;
using ImageView = VkImageView;
using CommandPool = VkCommandPool;
using CommandBuffer = VkCommandBuffer;
using ClearValue = VkClearValue;
using Pipeline = VkPipeline;
using PipelineShaderStageCreateInfo = VkPipelineShaderStageCreateInfo;
using ShaderStageFlagBits = VkShaderStageFlagBits;
using PipelineLayout = VkPipelineLayout;
using PipelineVertexInputStateCreateInfo = VkPipelineVertexInputStateCreateInfo;
using PipelineInputAssemblyStateCreateInfo = VkPipelineInputAssemblyStateCreateInfo;
using Viewport = VkViewport;
using Scissor = VkRect2D;
using Fence = VkFence;
using Swapchain = VkSwapchainKHR;
using Semaphore = VkSemaphore;
using SurfaceFormat = VkSurfaceFormatKHR;
using Image = VkImage;
using SurfaceCapabilities = VkSurfaceCapabilitiesKHR;

enum class ShaderStage
{
    Vertex,
    Fragment
};

inline Device create_device(const uint32_t queue_family_index, const PhysicalDevice &physical_device)
{
    float queue_priorities[] = {1.0f};

    VkDeviceQueueCreateInfo device_queue_create_info;
    device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    device_queue_create_info.pNext = nullptr;
    device_queue_create_info.flags = 0;
    device_queue_create_info.queueFamilyIndex = queue_family_index;
    device_queue_create_info.queueCount = 1;
    device_queue_create_info.pQueuePriorities = queue_priorities;

    unsigned int device_extension_count = 1;
    const char *device_extension_names[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkDeviceCreateInfo device_create_info;
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pNext = nullptr;
    device_create_info.flags = 0;
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pQueueCreateInfos = &device_queue_create_info;
    device_create_info.enabledLayerCount = 0;
    device_create_info.ppEnabledLayerNames = nullptr;
    device_create_info.enabledExtensionCount = device_extension_count;
    device_create_info.ppEnabledExtensionNames = device_extension_names;
    device_create_info.pEnabledFeatures = nullptr;

    Device device;
    if (vkCreateDevice(physical_device, &device_create_info, nullptr, &device) != VK_SUCCESS)
    {
        std::cerr << "Failed to create device." << std::endl;
        throw 1;
    }

    return device;
}

inline Framebuffer create_framebuffer(const RenderPass &render_pass, const ImageView &image_view, const uint32_t width, const uint32_t height, const Device &device)
{
    VkFramebufferCreateInfo framebuffer_create_info;
    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_create_info.pNext = nullptr;
    framebuffer_create_info.flags = 0;
    framebuffer_create_info.renderPass = render_pass;
    framebuffer_create_info.attachmentCount = 1;
    framebuffer_create_info.pAttachments = &image_view;
    framebuffer_create_info.width = width;
    framebuffer_create_info.height = height;
    framebuffer_create_info.layers = 1;

    VkFramebuffer framebuffer;
    if (vkCreateFramebuffer(device, &framebuffer_create_info, nullptr, &framebuffer) != VK_SUCCESS)
    {
        std::cerr << "Failed to create framebuffer." << std::endl;
        throw 1;
    }

    return framebuffer;
}

inline VkCommandPool create_command_pool(const uint32_t queue_family_index, const Device &device)
{
    VkCommandPoolCreateInfo command_pool_create_info;
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.pNext = nullptr;
    command_pool_create_info.flags = 0;
    command_pool_create_info.queueFamilyIndex = queue_family_index;

    CommandPool command_pool;
    if (vkCreateCommandPool(device, &command_pool_create_info, nullptr, &command_pool) != VK_SUCCESS)
    {
        std::cerr << "Failed to create command pool." << std::endl;
        throw 1;
    }

    return command_pool;
}

inline std::vector<CommandBuffer> allocate_command_buffers(const CommandPool &command_pool, uint32_t buffer_count, const Device &device)
{
    VkCommandBufferAllocateInfo command_buffer_allocate_info;
    command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocate_info.pNext = nullptr;
    command_buffer_allocate_info.commandPool = command_pool;
    command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocate_info.commandBufferCount = buffer_count; // Every frame buffer needs a command buffer

    std::vector<CommandBuffer> command_buffers(buffer_count);

    if (vkAllocateCommandBuffers(device, &command_buffer_allocate_info, command_buffers.data()) != VK_SUCCESS)
    {
        std::cerr << "Failed to allocate command buffers." << std::endl;
        throw 1;
    }

    return command_buffers;
}

inline void record_command_buffer(const CommandBuffer &command_buffer, const RenderPass &render_pass, const Framebuffer &framebuffer, const uint32_t width, const uint32_t height, const ClearValue &clear_value, const Pipeline &pipeline)
{
    VkCommandBufferBeginInfo command_buffer_begin_info;
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.pNext = nullptr;
    command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    command_buffer_begin_info.pInheritanceInfo = nullptr; // Important if secondary command buffer is in use.

    if (vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info) != VK_SUCCESS)
    {
        std::cerr << "Failed to begin command buffer recording." << std::endl;
        throw 1;
    }

    VkRenderPassBeginInfo render_pass_begin_info;
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.pNext = nullptr;
    render_pass_begin_info.renderPass = render_pass;
    render_pass_begin_info.framebuffer = framebuffer;
    render_pass_begin_info.renderArea.offset.x = 0;
    render_pass_begin_info.renderArea.offset.y = 0;
    render_pass_begin_info.renderArea.extent.width = width;
    render_pass_begin_info.renderArea.extent.height = height;
    render_pass_begin_info.clearValueCount = 1;
    render_pass_begin_info.pClearValues = &clear_value;

    vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE /* because only primary command buffers */);

    // Record: Bind pipeline and draw.
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdDraw(command_buffer, 3, 1, 0, 0); // TODO: Hard coded triangle (in vertex shader).

    vkCmdEndRenderPass(command_buffer);

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
    {
        std::cerr << "Failed to stop command buffer recording." << std::endl;
        throw 1;
    }
}

inline void destroy_framebuffer(const Device &device, const Framebuffer &framebuffer)
{
    vkDestroyFramebuffer(device, framebuffer, nullptr);
}

inline void destroy_command_pool(const Device &device, const CommandPool &command_pool)
{
    vkDestroyCommandPool(device, command_pool, nullptr);
}

inline Instance create_instance()
{
    unsigned int layer_count = 1;
    const char *layer_names[] = {"VK_LAYER_KHRONOS_validation"};

    unsigned int extension_count = 0;
    auto extension_names = glfwGetRequiredInstanceExtensions(&extension_count);

    VkApplicationInfo application_info;
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pNext = nullptr;
    application_info.pApplicationName = "My Application";
    application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.pEngineName = "No Engine";
    application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instance_create_info;
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pNext = nullptr;
    instance_create_info.flags = 0;
    instance_create_info.pApplicationInfo = &application_info;
    instance_create_info.enabledLayerCount = layer_count;
    instance_create_info.ppEnabledLayerNames = layer_names;
    instance_create_info.enabledExtensionCount = extension_count;
    instance_create_info.ppEnabledExtensionNames = extension_names;

    // TODO: Select layers and extensions properly and check if they are available.

    VkInstance instance;
    if (vkCreateInstance(&instance_create_info, nullptr, &instance) != VK_SUCCESS)
    {
        std::cerr << "Failed to create instance." << std::endl;
        throw 1;
    }

    return instance;
}

inline void destroy_instance(const Instance &instance)
{
    vkDestroyInstance(instance, nullptr);
}

inline PhysicalDevice choose_physical_device(const Instance &instance)
{
    unsigned int physical_device_count = 0;
    if (vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr) != VK_SUCCESS)
    {
        std::cerr << "Failed to find physical device count." << std::endl;
        throw 1;
    }

    if (physical_device_count == 0)
    {
        std::cerr << "No physical device is available." << std::endl;
        throw 1;
    }

    VkPhysicalDevice *physical_devices = new VkPhysicalDevice[physical_device_count];

    if (vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices) != VK_SUCCESS)
    {
        std::cerr << "Failed to find physical devices." << std::endl;
        throw 1;
    }

    // TODO: Choose a physical device properly instead of choosing the first.
    auto physical_device = physical_devices[0];
    delete[] physical_devices;

    return physical_device;
}

inline uint32_t select_queue_family_index(const PhysicalDevice &physical_device)
{
    unsigned int queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);
    if (queue_family_count == 0)
    {
        std::cerr << "No queue families are available." << std::endl;
        throw 1;
    }

    VkQueueFamilyProperties *queue_family_properties = new VkQueueFamilyProperties[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family_properties);

    int queue_family_index = -1;
    for (unsigned int i = 0; i < queue_family_count; i++)
    {
        // TODO: Choose a queue family index properly
        if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queue_family_index = i;
            break;
        }
    }

    delete[] queue_family_properties;

    if (queue_family_index < 0)
    {
        std::cerr << "No suitable queue family found." << std::endl;
        throw 0;
    }

    // TODO: Check presentation support

    return queue_family_index;
}

inline PipelineShaderStageCreateInfo create_pipeline_shader_stage_create_info(const ShaderModule &shader_module, const ShaderStage &shader_stage)
{
    ShaderStageFlagBits shader_stage_flag_bits;

    switch (shader_stage)
    {
    case ShaderStage::Vertex: shader_stage_flag_bits = VK_SHADER_STAGE_VERTEX_BIT; break;
    case ShaderStage::Fragment: shader_stage_flag_bits = VK_SHADER_STAGE_FRAGMENT_BIT; break;
    };

    VkPipelineShaderStageCreateInfo shader_stage_create_info;
    shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage_create_info.pNext = nullptr;
    shader_stage_create_info.flags = 0;
    shader_stage_create_info.stage = shader_stage_flag_bits;
    shader_stage_create_info.module = shader_module;
    shader_stage_create_info.pName = "main";
    shader_stage_create_info.pSpecializationInfo = nullptr;

    return shader_stage_create_info;
}

inline PipelineLayout create_pipeline_layout(const Device &device)
{
    VkPipelineLayoutCreateInfo pipeline_layout_create_info;
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.pNext = nullptr;
    pipeline_layout_create_info.flags = 0;
    pipeline_layout_create_info.setLayoutCount = 0;
    pipeline_layout_create_info.pSetLayouts = nullptr;
    pipeline_layout_create_info.pushConstantRangeCount = 0;
    pipeline_layout_create_info.pPushConstantRanges = nullptr;

    PipelineLayout pipeline_layout;
    if (vkCreatePipelineLayout(device, &pipeline_layout_create_info, nullptr, &pipeline_layout) != VK_SUCCESS)
    {
        std::cerr << "Failed to create pipeline layout." << std::endl;
        throw 1;
    }

    return pipeline_layout;
}

inline Viewport create_viewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
{
    VkViewport viewport;
    viewport.x = x;
    viewport.y = y;
    viewport.width = width;
    viewport.height = height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    return viewport;
}

inline Scissor create_scissor(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
{
    VkRect2D scissor;
    scissor.offset.x = x;
    scissor.offset.y = y;
    scissor.extent.width = width;
    scissor.extent.height = height;

    return scissor;
}

inline Pipeline create_pipeline(const Viewport &viewport, const Scissor &scissor, const PipelineShaderStageCreateInfo *pipeline_shader_stage_create_infos, const PipelineLayout &pipeline_layout, const RenderPass &render_pass, const Device &device)
{
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
    graphics_pipeline_create_info.renderPass = render_pass;
    graphics_pipeline_create_info.subpass = 0; // Index of subpass in render pass
    graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    graphics_pipeline_create_info.basePipelineIndex = -1; // -1 is an invalid index (in contrast to 0).

    Pipeline pipeline;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &pipeline) != VK_SUCCESS)
    {
        std::cerr << "Failed to create pipeline." << std::endl;
        throw 1;
    }

    return pipeline;
}

inline Queue get_device_queue(const Device &device, const uint32_t queue_family_index)
{
    Queue queue;
    vkGetDeviceQueue(device, queue_family_index, 0, &queue);

    return queue;
}

inline Semaphore create_semaphore(const Device &device)
{
    VkSemaphoreCreateInfo semaphore_create_info;
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_create_info.pNext = nullptr;
    semaphore_create_info.flags = 0;

    VkSemaphore semaphore;
    if (vkCreateSemaphore(device, &semaphore_create_info, nullptr, &semaphore) != VK_SUCCESS)
    {
        std::cerr << "Failed to create semaphore." << std::endl;
        throw 1;
    }

    return semaphore;
}

inline Fence create_fence(const Device &device)
{
    VkFenceCreateInfo fence_create_info;
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.pNext = nullptr;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    Fence fence;
    if (vkCreateFence(device, &fence_create_info, nullptr, &fence) != VK_SUCCESS)
    {
        std::cerr << "Failed to create fence." << std::endl;
        throw 1;
    }

    return fence;
}

inline uint32_t next_image_index(const Device &device, const Swapchain &swapchain, const Semaphore &semaphore_image_available)
{
    unsigned int image_index = 0;
    if (vkAcquireNextImageKHR(device, swapchain, std::numeric_limits<uint64_t>::max(), semaphore_image_available, VK_NULL_HANDLE, &image_index) != VK_SUCCESS)
    {
        std::cerr << "Failed to acquire next image.";
        throw 1;
    }

    return image_index;
}

inline void wait_and_reset_fence(const Device &device, const Fence &fence)
{
    // Wait fence
    if (vkWaitForFences(device, 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max()) != VK_SUCCESS)
    {
        std::cerr << "Failed to wait for fence." << std::endl;
        throw 1;
    }

    // Reset fence
    if (vkResetFences(device, 1, &fence) != VK_SUCCESS)
    {
        std::cerr << "Failed to wait for fence." << std::endl;
        throw 1;
    }
}

inline void submit_queue(const Queue &queue, const CommandBuffer &command_buffer, const Fence &fence, const Semaphore &semaphore_image_available, const Semaphore &semaphore_image_rendered)
{
    // When should we wait for the semaphore?
    VkPipelineStageFlags pipeline_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    // Set which semaphore to wait for and which semaphore to set
    VkSubmitInfo submit_info;
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pNext = nullptr;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &semaphore_image_available;
    submit_info.pWaitDstStageMask = &pipeline_stage_flags;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &semaphore_image_rendered;

    if (vkQueueSubmit(queue, 1, &submit_info, fence) != VK_SUCCESS)
    {
        std::cerr << "Failed submitting to queue." << std::endl;
        throw 1;
    }
}

inline void present_queue(const Queue &queue, const Swapchain &swapchain, const Semaphore &semaphore_image_rendered, const uint32_t image_index)
{
    VkPresentInfoKHR present_info;
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pNext = nullptr;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &semaphore_image_rendered;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain;
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr;

    if (vkQueuePresentKHR(queue, &present_info) != VK_SUCCESS)
    {
        std::cerr << "Failed to present queue." << std::endl;
        throw 1;
    }
}

inline RenderPass create_render_pass(const SurfaceFormat &surface_format, const Device &device)
{
    VkAttachmentDescription attachment_description;
    attachment_description.flags = 0;
    attachment_description.format = surface_format.format;
    attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Adds a subpass!

    VkAttachmentReference attachment_reference;
    attachment_reference.attachment = 0;
    attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // Adds a subpass!

    VkSubpassDescription subpass_description;
    subpass_description.flags = 0;
    subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_description.inputAttachmentCount = 0;
    subpass_description.pInputAttachments = nullptr;
    subpass_description.colorAttachmentCount = 1;
    subpass_description.pColorAttachments = &attachment_reference;
    subpass_description.pResolveAttachments = nullptr;
    subpass_description.pDepthStencilAttachment = nullptr;
    subpass_description.preserveAttachmentCount = 0;
    subpass_description.pPreserveAttachments = nullptr;

    VkSubpassDependency subpass_dependency;
    subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpass_dependency.dstSubpass = 0;
    subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.srcAccessMask = 0;
    subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpass_dependency.dependencyFlags = 0;

    VkRenderPassCreateInfo render_pass_create_info;
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.pNext = nullptr;
    render_pass_create_info.flags = 0;
    render_pass_create_info.attachmentCount = 1;
    render_pass_create_info.pAttachments = &attachment_description;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass_description;
    render_pass_create_info.dependencyCount = 1;
    render_pass_create_info.pDependencies = &subpass_dependency;

    RenderPass render_pass;
    if (vkCreateRenderPass(device, &render_pass_create_info, nullptr, &render_pass) != VK_SUCCESS)
    {
        std::cerr << "Failed to create render pass.";
        throw 1;
    }

    return render_pass;
}

inline ShaderModule create_shader_module(const Device &device, const char *file_buffer, const size_t file_size)
{
    VkShaderModuleCreateInfo shader_module_create_info;
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.pNext = nullptr;
    shader_module_create_info.flags = 0;
    shader_module_create_info.codeSize = file_size;
    shader_module_create_info.pCode = reinterpret_cast<const uint32_t *>(file_buffer);

    ShaderModule shader_module;
    if (vkCreateShaderModule(device, &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS)
    {
        std::cerr << "Failed to create vertex shader module." << std::endl;
        throw 1;
    }

    return shader_module;
}

inline SurfaceFormat get_surface_format(const Surface &surface, const PhysicalDevice &physical_device)
{
    unsigned int surface_format_count = 0;

    if (vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, nullptr) != VK_SUCCESS)
    {
        std::cerr << "Failed to find surface formats." << std::endl;
        throw 1;
    }

    if (surface_format_count == 0)
    {
        std::cerr << "No surface formats are found." << std::endl;
        throw 1;
    }

    VkSurfaceFormatKHR *formats = new VkSurfaceFormatKHR[surface_format_count];

    if (vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, formats) != VK_SUCCESS)
    {
        std::cerr << "Failed to fetch surface formats." << std::endl;
        throw 1;
    }

    SurfaceFormat surface_format;
    for (unsigned int i = 0; i < surface_format_count; i++)
    {
        // TODO: Choose surface format properly.
        if (formats[i].format == VK_FORMAT_R8G8B8A8_UNORM || formats[i].format == VK_FORMAT_B8G8R8A8_UNORM)
        {
            surface_format = formats[i];
            break;
        }
    }

    delete[] formats;

    if (surface_format.format == VK_FORMAT_UNDEFINED)
    {
        std::cerr << "Surface does not support R8G8B8A8 format." << std::endl;
        throw 1;
    }

    return surface_format;
}

inline SurfaceCapabilities get_surface_capabilities(const Surface &surface, const PhysicalDevice &physical_device)
{
    SurfaceCapabilities surface_capabilities;
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities) != VK_SUCCESS)
    {
        std::cerr << "Failed to fetch surface capabilities." << std::endl;
        throw 1;
    }

    return surface_capabilities;
}

inline Swapchain create_swapchain(const Surface &surface, const Device &device, const PhysicalDevice &physical_device, const uint32_t queue_family_index, const uint32_t min_image_count, const SurfaceFormat &surface_format, const uint32_t width, const uint32_t height)
{
    VkBool32 surface_supported = false;
    if (vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, queue_family_index, surface, &surface_supported) != VK_SUCCESS)
    {
        std::cerr << "Failed to check surface support of physical device." << std::endl;
        throw 1;
    }

    if (!surface_supported)
    {
        std::cerr << "Physical device does not support surfaces." << std::endl;
        throw 1;
    }

    VkSwapchainCreateInfoKHR swapchain_create_info;
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.pNext = nullptr;
    swapchain_create_info.flags = 0;
    swapchain_create_info.surface = surface;
    swapchain_create_info.minImageCount = min_image_count;
    swapchain_create_info.imageFormat = surface_format.format;
    swapchain_create_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; // TODO: Check if valid.
    swapchain_create_info.imageExtent.width = width;
    swapchain_create_info.imageExtent.height = height;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: Support for separate presentation and graphic queues (VK_SHARING_MODE_CONCURRENT).
    swapchain_create_info.queueFamilyIndexCount = 0;
    swapchain_create_info.pQueueFamilyIndices = nullptr;
    swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR; //VK_PRESENT_MODE_FIFO_KHR; // TODO: Check if there is a better mode?
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE; // TODO: Support for resizing windows (recreate swapchain).

    Swapchain swapchain;
    if (vkCreateSwapchainKHR(device, &swapchain_create_info, nullptr, &swapchain) != VK_SUCCESS)
    {
        std::cerr << "Failed to create swapchain." << std::endl;
        throw 1;
    }

    return swapchain;
}

inline std::vector<Image> get_swapchain_images(const Swapchain &swapchain, const Device &device)
{
    unsigned int swapchain_image_count = 0;

    if (vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, nullptr) != VK_SUCCESS)
    {
        std::cerr << "Failed to fetch swapchain images." << std::endl;
        throw 1;
    }

    if (swapchain_image_count == 0)
    {
        std::cerr << "No swapchain images found." << std::endl;
        throw 1;
    }

    std::vector<VkImage> swapchain_images(swapchain_image_count);
    if (vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, swapchain_images.data()) != VK_SUCCESS)
    {
        std::cerr << "Failed to fetch swapchain images." << std::endl;
        throw 1;
    }

    return swapchain_images;
}

inline ImageView create_image_view(const Device &device, const Image &image, const SurfaceFormat &surface_format)
{
    VkImageViewCreateInfo image_view_create_info;
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.pNext = nullptr;
    image_view_create_info.flags = 0;
    image_view_create_info.image = image;
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = surface_format.format;
    image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    VkImageView image_view;
    if (vkCreateImageView(device, &image_view_create_info, nullptr, &image_view) != VK_SUCCESS)
    {
        std::cerr << "Failed to create image view." << std::endl;
        throw 1;
    }

    return image_view;
}

inline void destroy_device(const Device &device)
{
    vkDestroyDevice(device, nullptr);
}

inline void device_wait_idle(const Device &device)
{
    vkDeviceWaitIdle(device);
}

inline void destroy_pipeline(const Device &device, const Pipeline &pipeline)
{
    vkDestroyPipeline(device, pipeline, nullptr);
}

inline void destroy_pipeline_layout(const Device &device, const PipelineLayout &pipeline_layout)
{
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
}

inline void destroy_fence(const Device &device, const Fence &fence)
{
    vkDestroyFence(device, fence, nullptr);
}

inline void destroy_semaphore(const Device &device, const Semaphore &semaphore)
{
    vkDestroySemaphore(device, semaphore, nullptr);
}

inline void destroy_render_pass(const Device &device, const RenderPass &render_pass)
{
    vkDestroyRenderPass(device, render_pass, nullptr);
}

inline void destroy_shader_module(const Device &device, const ShaderModule &shader_module)
{
    vkDestroyShaderModule(device, shader_module, nullptr);
}

inline void destroy_surface(const Instance &instance, const Surface &surface)
{
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

inline void destroy_image_view(const Device &device, const ImageView &image_view)
{
    vkDestroyImageView(device, image_view, nullptr);
}

inline void destroy_swapchain(const Device &device, const Swapchain &swapchian)
{
    vkDestroySwapchainKHR(device, swapchian, nullptr);
}

}

}
