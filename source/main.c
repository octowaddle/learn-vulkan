#define GLFW_INCLUDE_VULKAN

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

int main() {

    //
    //
    // Initialize external libraries.
    //

    if (!glfwInit()) {
        fprintf(stderr, "error: failed to initialize GLFW.\n");
        exit(1);
    }

    //
    //
    // Create an instance.
    //

    const VkApplicationInfo application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = "Vulkan Application",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Vulkan Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0
    };

    const uint32_t instance_layer_count = 1;
    const char *const instance_layer_names[] = {"VK_LAYER_KHRONOS_validation"};

    uint32_t instance_extension_count = 0;
    const char *const *const instance_extension_names = glfwGetRequiredInstanceExtensions(&instance_extension_count);

    const VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .pApplicationInfo = &application_info,
        .enabledLayerCount = instance_layer_count,
        .ppEnabledLayerNames = instance_layer_names,
        .enabledExtensionCount = instance_extension_count,
        .ppEnabledExtensionNames = instance_extension_names
    };

    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vkCreateInstance(&instance_create_info, NULL, &instance);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create instance\n");
        exit(1);
    }

    //
    //
    // Choose a physical device.
    //

    uint32_t physical_device_count = 0;
    vkEnumeratePhysicalDevices(instance, &physical_device_count, NULL);
    VkPhysicalDevice *physical_devices = malloc(physical_device_count * (sizeof *physical_devices));
    vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices);

    if (physical_device_count == 0) {
        fprintf(stderr, "error: no physical devices available\n");
        exit(1);
    }

    VkPhysicalDevice physical_device = physical_devices[0];

    for (uint32_t physical_device_index = 0U; physical_device_index < physical_device_count; ++physical_device_index) {
        VkPhysicalDeviceProperties physical_device_properties;
        vkGetPhysicalDeviceProperties(physical_devices[physical_device_index], &physical_device_properties);

        if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            physical_device = physical_devices[physical_device_index];
            break;
        }
    }

    free(physical_devices);

    //
    //
    // Choose a queue family.
    //

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, NULL);
    VkQueueFamilyProperties *queue_family_properties = malloc(queue_family_count * (sizeof *queue_family_properties));
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family_properties);

    if (queue_family_count == 0) {
        fprintf(stderr, "error: no queue families are available\n");
        exit(1);
    }

    uint32_t queue_family_index = 0;
    uint32_t queue_family_found = 0;

    for (uint32_t queue_family_properties_index = 0U; queue_family_properties_index < queue_family_count; ++queue_family_properties_index) {
        if (queue_family_properties[queue_family_properties_index].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queue_family_index = queue_family_properties_index;
            queue_family_found = 1;
            break;
        }
    }

    free(queue_family_properties);

    if (!queue_family_found) {
        fprintf(stderr, "error: no graphics queue family is available\n");
        exit(1);
    }

    //
    //
    // Create a device.
    //

    const float queue_priorities[1] = {1.0f};

    const uint32_t device_extension_count = 1;
    const char *const device_extension_names[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    // TODO Create multiple queues from multiple queue families.

    const VkDeviceQueueCreateInfo device_queue_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueFamilyIndex = queue_family_index,
        .queueCount = 1,
        .pQueuePriorities = queue_priorities
    };

    const VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &device_queue_create_info,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = device_extension_count,
        .ppEnabledExtensionNames = device_extension_names,
        .pEnabledFeatures = NULL
    };

    VkDevice device = VK_NULL_HANDLE;
    result = vkCreateDevice(physical_device, &device_create_info, NULL, &device);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create device\n");
        exit(1);
    }

    //
    //
    // Create a window.
    //

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "Vulkan Application", NULL, NULL);

    if (window == NULL) {
        fprintf(stderr, "error: failed to create window\n");
        exit(1);
    }

    //
    //
    // Create a surface.
    //

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    glfwCreateWindowSurface(instance, window, NULL, &surface);

    uint32_t surface_format_count = 0;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, NULL);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to get surface formats\n");
        exit(1);
    }

    VkSurfaceFormatKHR *surface_formats = malloc(surface_format_count * (sizeof *surface_formats));
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, surface_formats);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to get surface formats\n");
        exit(1);
    }

    if (surface_format_count == 0) {
        fprintf(stderr, "error: no surface formats are available\n");
        exit(1);
    }

    VkSurfaceFormatKHR surface_format = surface_formats[0];

    // TODO Improve filtering for surface formats.

    for (uint32_t surface_format_index = 0U; surface_format_index < surface_format_count; ++surface_format_index) {
        if (surface_formats[surface_format_index].format == VK_FORMAT_R8G8B8A8_UNORM || surface_formats[surface_format_index].format == VK_FORMAT_B8G8R8A8_UNORM) {
            surface_format = surface_formats[surface_format_index];
            break;
        }
    }

    free(surface_formats);

    if (surface_format.format == VK_FORMAT_UNDEFINED) {
        fprintf(stderr, "error: surface has an undefined format\n");
        exit(1);
    }

    VkSurfaceCapabilitiesKHR surface_capabilities;
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to get surface capabilities\n");
        exit(1);
    }

    //
    //
    // Create a swapchain.
    //

    uint32_t swapchain_min_image_count = 3;

    while (swapchain_min_image_count > surface_capabilities.minImageCount) {
        swapchain_min_image_count--;
    }

    VkBool32 physical_device_surface_support = VK_FALSE;

    result = vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, queue_family_index, surface, &physical_device_surface_support);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to check surface support\n");
        exit(1);
    }

    if (!physical_device_surface_support) {
        fprintf(stderr, "error: physical device does not support surfaces\n");
        exit(1);
    }

    // TODO Check for better presentation modes.

    const VkSwapchainCreateInfoKHR swapchain_create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .surface = surface,
        .minImageCount = swapchain_min_image_count,
        .imageFormat = surface_format.format,
        .imageColorSpace = surface_format.colorSpace,
        .imageExtent = surface_capabilities.currentExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    result = vkCreateSwapchainKHR(device, &swapchain_create_info, NULL, &swapchain);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create surface\n");
        exit(1);
    }

    //
    //
    // Create image views.
    //

    uint32_t swapchain_image_count = 0;
    result = vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, NULL);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to get swapchain images\n");
        exit(1);
    }

    VkImage *swapchain_images = malloc(swapchain_image_count * (sizeof *swapchain_images));
    result = vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, swapchain_images);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to get swapchain images\n");
        exit(1);
    }

    if (swapchain_image_count == 0) {
        fprintf(stderr, "error: swapchain has no images\n");
        exit(1);
    }

    VkImageView *image_views = malloc(swapchain_image_count * (sizeof *image_views));

    for (uint32_t swapchain_image_index = 0U; swapchain_image_index < swapchain_min_image_count; ++swapchain_image_index) {
        const VkImageViewCreateInfo image_view_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .image = swapchain_images[swapchain_image_index],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = surface_format.format,
            .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
            .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .subresourceRange.baseMipLevel = 0,
            .subresourceRange.levelCount = 1,
            .subresourceRange.baseArrayLayer = 0,
            .subresourceRange.layerCount = 1
        };

        result = vkCreateImageView(device, &image_view_create_info, NULL, &image_views[swapchain_image_index]);

        if (result != VK_SUCCESS) {
            fprintf(stderr, "error: failed to create image view\n");
            exit(1);
        }
    }

    free(swapchain_images);

    //
    //
    // Create shader modules.
    //

    FILE *vertex_shader_module_file = fopen("vert.spv", "rb");

    if (!vertex_shader_module_file) {
        fprintf(stderr, "error: failed to open vertex shader module file\n");
        exit(1);
    }

    fseek(vertex_shader_module_file, 0L, SEEK_END);
    const uint64_t vertex_shader_module_file_size = ftell(vertex_shader_module_file);
    fseek(vertex_shader_module_file, 0L, SEEK_SET);
    char *vertex_shader_module_file_buffer = malloc(vertex_shader_module_file_size * (sizeof *vertex_shader_module_file_buffer));
    fread(vertex_shader_module_file_buffer, vertex_shader_module_file_size, 1, vertex_shader_module_file);

    const VkShaderModuleCreateInfo vertex_shader_module_create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .codeSize = vertex_shader_module_file_size,
        .pCode = (uint32_t *) vertex_shader_module_file_buffer,
    };

    VkShaderModule vertex_shader_module = VK_NULL_HANDLE;
    result = vkCreateShaderModule(device, &vertex_shader_module_create_info, NULL, &vertex_shader_module);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create vertex shader module\n");
        exit(1);
    }

    fclose(vertex_shader_module_file);
    free(vertex_shader_module_file_buffer);

    FILE *fragment_shader_module_file = fopen("frag.spv", "rb");

    if (!fragment_shader_module_file) {
        fprintf(stderr, "error: failed to open fragment shader module file\n");
        exit(1);
    }

    fseek(fragment_shader_module_file, 0L, SEEK_END);
    const uint64_t fragment_shader_module_file_size = ftell(fragment_shader_module_file);
    fseek(fragment_shader_module_file, 0L, SEEK_SET);
    char *fragment_shader_module_file_buffer = malloc(fragment_shader_module_file_size * (sizeof *fragment_shader_module_file_buffer));
    fread(fragment_shader_module_file_buffer, fragment_shader_module_file_size, 1, fragment_shader_module_file);

    const VkShaderModuleCreateInfo fragment_shader_module_create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .codeSize = fragment_shader_module_file_size,
        .pCode = (uint32_t *) fragment_shader_module_file_buffer,
    };

    VkShaderModule fragment_shader_module = VK_NULL_HANDLE;
    result = vkCreateShaderModule(device, &fragment_shader_module_create_info, NULL, &fragment_shader_module);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create fragment shader module\n");
        exit(1);
    }

    fclose(fragment_shader_module_file);
    free(fragment_shader_module_file_buffer);

    //
    //
    // Create a render pass.
    //

    const VkAttachmentDescription attachment_description = {
        .flags = 0,
        .format = surface_format.format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    const VkAttachmentReference attachment_reference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    const VkSubpassDescription subpass_description = {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = NULL,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachment_reference,
        .pResolveAttachments = NULL,
        .pDepthStencilAttachment = NULL,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = NULL
    };

    const VkSubpassDependency subpass_dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = 0
    };

    const VkRenderPassCreateInfo render_pass_create_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .attachmentCount = 1,
        .pAttachments = &attachment_description,
        .subpassCount = 1,
        .pSubpasses = &subpass_description,
        .dependencyCount = 1,
        .pDependencies = &subpass_dependency
    };

    VkRenderPass render_pass;
    result = vkCreateRenderPass(device, &render_pass_create_info, NULL, &render_pass);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create render pass\n");
        exit(1);
    }

    //
    //
    // Create a pipeline.
    //

    VkPipelineLayoutCreateInfo graphics_pipeline_layout_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = 0,
        .pSetLayouts = NULL,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL
    };

    VkPipelineLayout graphics_pipeline_layout;
    result = vkCreatePipelineLayout(device, &graphics_pipeline_layout_create_info, NULL, &graphics_pipeline_layout);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create graphics pipeline layout\n");
        exit(1);
    }

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
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = NULL,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = NULL
    };

    const VkPipelineInputAssemblyStateCreateInfo graphics_pipeline_input_assembly_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
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
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
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
        .pDynamicState = NULL,
        .layout = graphics_pipeline_layout,
        .renderPass = render_pass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };

    VkPipeline graphics_pipeline = VK_NULL_HANDLE;
    result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, NULL, &graphics_pipeline);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create graphics pipeline\n");
        exit(1);
    }

    vkDestroyShaderModule(device, fragment_shader_module, NULL);
    vkDestroyShaderModule(device, vertex_shader_module, NULL);

    fragment_shader_module = VK_NULL_HANDLE;
    vertex_shader_module = VK_NULL_HANDLE;

    //
    //
    // Create framebuffers.
    //

    VkFramebuffer *framebuffers = malloc(swapchain_image_count * (sizeof *framebuffers));

    for (uint32_t framebuffer_index = 0U; framebuffer_index < swapchain_image_count; ++framebuffer_index) {
        const VkFramebufferCreateInfo framebuffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .renderPass = render_pass,
            .attachmentCount = 1,
            .pAttachments = &image_views[framebuffer_index],
            .width = surface_capabilities.currentExtent.width,
            .height = surface_capabilities.currentExtent.height,
            .layers = 1
        };

        result = vkCreateFramebuffer(device, &framebuffer_create_info, NULL, &framebuffers[framebuffer_index]);

        if (result != VK_SUCCESS) {
            fprintf(stderr, "error: failed to create framebuffer\n");
            exit(1);
        }
    }

    //
    //
    // Create command pool.
    //

    const VkCommandPoolCreateInfo command_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueFamilyIndex = queue_family_index
    };

    VkCommandPool command_pool = VK_NULL_HANDLE;
    result = vkCreateCommandPool(device, &command_pool_create_info, NULL, &command_pool);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create command pool\n");
        exit(1);
    }

    //
    //
    // Create command buffers.
    //

    const VkCommandBufferAllocateInfo command_buffer_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = swapchain_image_count
    };

    VkCommandBuffer *command_buffers = malloc(swapchain_image_count * (sizeof *command_buffers));
    result = vkAllocateCommandBuffers(device, &command_buffer_allocate_info, command_buffers);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to allocate command buffers\n");
        exit(1);
    }

    const VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
        .pInheritanceInfo = NULL
    };

    const VkClearColorValue clear_color_value = {
        .float32 = {0.0f, 0.0f, 0.0f, 1.0f},
        .int32 = {0, 0, 0, 255},
        .uint32 = {0U, 0U, 0U, 255U}
    };

    const VkClearDepthStencilValue clear_depth_stencil_value = {
        .depth = 0.0f,
        .stencil = 0U
    };

    const VkClearValue clear_value = {
        .color = clear_color_value,
        .depthStencil = clear_depth_stencil_value
    };

    for (uint32_t command_buffer_index = 0U; command_buffer_index < swapchain_image_count; ++command_buffer_index) {
        result = vkBeginCommandBuffer(command_buffers[command_buffer_index], &command_buffer_begin_info);

        if (result != VK_SUCCESS) {
            fprintf(stderr, "error: failed to begin command buffer recording\n");
            exit(1);
        }

        const VkRenderPassBeginInfo render_pass_begin_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = NULL,
            .renderPass = render_pass,
            .framebuffer = framebuffers[command_buffer_index],
            .renderArea.offset.x = 0,
            .renderArea.offset.y = 0,
            .renderArea.extent.width = surface_capabilities.currentExtent.width,
            .renderArea.extent.height = surface_capabilities.currentExtent.height,
            .clearValueCount = 1,
            .pClearValues = &clear_value
        };

        vkCmdBeginRenderPass(command_buffers[command_buffer_index], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(command_buffers[command_buffer_index], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);

        // TODO Not only 3 vertices for a hard-coded triangle.

        vkCmdDraw(command_buffers[command_buffer_index], 3, 1, 0, 0);

        vkCmdEndRenderPass(command_buffers[command_buffer_index]);

        result = vkEndCommandBuffer(command_buffers[command_buffer_index]);

        if (result != VK_SUCCESS) {
            fprintf(stderr, "error: failed to end command buffer recording\n");
            exit(1);
        }
    };

    //
    //
    // Create semaphores.
    //

    const VkSemaphoreCreateInfo semaphore_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0
    };

    VkSemaphore semaphore_image_available;
    VkSemaphore semaphore_image_rendered;

    result = vkCreateSemaphore(device, &semaphore_create_info, NULL, &semaphore_image_available);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create image available semaphore\n");
        exit(1);
    }

    result = vkCreateSemaphore(device, &semaphore_create_info, NULL, &semaphore_image_rendered);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create image rendered semaphore\n");
        exit(1);
    }

    //
    //
    // Create fences.
    //

    const VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    VkFence *fences = malloc(swapchain_image_count * (sizeof *fences));

    for (uint32_t fence_index = 0U; fence_index < swapchain_image_count; ++fence_index) {
        result = vkCreateFence(device, &fence_create_info, NULL, &fences[fence_index]);

        if (result != VK_SUCCESS) {
            fprintf(stderr, "error: failed to create fence\n");
            exit(1);
        }
    }

    //
    //
    // Get device queue.
    //

    VkQueue queue = VK_NULL_HANDLE;
    vkGetDeviceQueue(device, queue_family_index, 0, &queue);

    //
    //
    // Main loop.
    //

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        uint32_t image_index = 0;
        result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, semaphore_image_available, VK_NULL_HANDLE, &image_index);

        if (result != VK_SUCCESS) {
            fprintf(stderr, "error: failed to acquire next image\n");
            exit(1);
        }

        result = vkWaitForFences(device, 1, &fences[image_index], VK_TRUE, UINT64_MAX);

        if (result != VK_SUCCESS) {
            fprintf(stderr, "error: failed to wait for fence\n");
            exit(1);
        }

        result = vkResetFences(device, 1, &fences[image_index]);

        if (result != VK_SUCCESS) {
            fprintf(stderr, "error: failed to reset fence\n");
            exit(1);
        }

        const VkPipelineStageFlags pipeline_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        const VkSubmitInfo submit_info = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = NULL,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &semaphore_image_available,
            .pWaitDstStageMask = &pipeline_stage_flags,
            .commandBufferCount = 1,
            .pCommandBuffers = &command_buffers[image_index],
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &semaphore_image_rendered
        };

        result = vkQueueSubmit(queue, 1, &submit_info, fences[image_index]);

        if (result != VK_SUCCESS) {
            fprintf(stderr, "error: failed to submit to queue\n");
            exit(1);
        }

        const VkPresentInfoKHR present_info = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = NULL,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &semaphore_image_rendered,
            .swapchainCount = 1,
            .pSwapchains = &swapchain,
            .pImageIndices = &image_index,
            .pResults = NULL
        };

        result = vkQueuePresentKHR(queue, &present_info);

        if (result != VK_SUCCESS) {
            fprintf(stderr, "error: failed to present queue\n");
            exit(1);
        }
    }

    vkDeviceWaitIdle(device);

    //
    //
    // Clean up.
    //

    for (uint32_t fence_index = 0U; fence_index < swapchain_image_count; ++fence_index) {
        vkDestroyFence(device, fences[fence_index], NULL);
    }
    free(fences);

    vkDestroySemaphore(device, semaphore_image_rendered, NULL);
    vkDestroySemaphore(device, semaphore_image_available, NULL);

    vkFreeCommandBuffers(device, command_pool, swapchain_image_count, command_buffers);

    free(command_buffers);

    vkDestroyCommandPool(device, command_pool, NULL);

    for (uint32_t framebuffer_index = 0U; framebuffer_index < swapchain_image_count; ++framebuffer_index) {
        vkDestroyFramebuffer(device, framebuffers[framebuffer_index], NULL);
    }
    free(framebuffers);

    vkDestroyPipeline(device, graphics_pipeline, NULL);

    vkDestroyPipelineLayout(device, graphics_pipeline_layout, NULL);

    vkDestroyRenderPass(device, render_pass, NULL);

    if (fragment_shader_module != VK_NULL_HANDLE) {
        vkDestroyShaderModule(device, fragment_shader_module, NULL);
    }

    if (vertex_shader_module != VK_NULL_HANDLE) {
        vkDestroyShaderModule(device, vertex_shader_module, NULL);
    }

    for (uint32_t image_view_index = 0U; image_view_index < swapchain_image_count; ++image_view_index) {
        vkDestroyImageView(device, image_views[image_view_index], NULL);
    }
    free(image_views);

    vkDestroySwapchainKHR(device, swapchain, NULL);

    vkDestroySurfaceKHR(instance, surface, NULL);

    glfwDestroyWindow(window);
    glfwTerminate();

    vkDestroyDevice(device, NULL);

    vkDestroyInstance(instance, NULL);
}
