#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <sstream>
#include <chrono>

// Notes
//
// -? Use std::vector<T> instead of heap-allocated C++ arrays.
//
// Shall be classes:
//
// - Instance
// - Device
// - Pipeline (with PipelineLayout)
// - Images
// - RenderPass
// - QueueFamily (for index)
// - Shader modules
// - SwapChain (for image count)
// - CommandPool
// - Queue
//

int main()
{
    const unsigned int width = 600;
    const unsigned int height = 500;

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Initialization
    ///

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return 1;
    }

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Create a Vulkan instance.
    ///

    unsigned int layer_count = 0;
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
        return 1;
    }

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Choose a physical device.
    ///

    unsigned int physical_device_count = 0;
    if (vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr) != VK_SUCCESS)
    {
        std::cerr << "Failed to find physical device count." << std::endl;
        return 1;
    }

    if (physical_device_count == 0)
    {
        std::cerr << "No physical device is available." << std::endl;
        return 1;
    }

    VkPhysicalDevice *physical_devices = new VkPhysicalDevice[physical_device_count];

    if (vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices) != VK_SUCCESS)
    {
        std::cerr << "Failed to find physical devices." << std::endl;
        return 1;
    }

    // TODO: Choose a physical device properly instead of choosing the first.
    VkPhysicalDevice physical_device = physical_devices[0];
    delete[] physical_devices;

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Choose a queue family.
    ///

    unsigned int queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);
    if (queue_family_count == 0)
    {
        std::cerr << "No queue families are available." << std::endl;
        return 1;
    }

    VkQueueFamilyProperties *queue_family_properties = new VkQueueFamilyProperties[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family_properties);

    int queue_family_index = -1;
    for (unsigned int i = 0; i < queue_family_count; i++)
    {
        // TODO: Choose a queue family index properly.
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
        return 0;
    }

    // TODO: Check presentation support? Or maybe use 2 queues, one for graphics and one for presentation

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Create a logical device.
    ///

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

    VkDevice device;
    if (vkCreateDevice(physical_device, &device_create_info, nullptr, &device) != VK_SUCCESS)
    {
        std::cerr << "Failed to create device." << std::endl;
        return 1;
    }

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Create queue.
    ///

    VkQueue queue;
    vkGetDeviceQueue(device, queue_family_index, 0, &queue);

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Create a GLFW window.
    ///

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(width, height, "learn-vulkan", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create window." << std::endl;
        return 1;
    }

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Create a window surface.
    ///

    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
    {
        std::cerr << "Failed to create window surface." << std::endl;
        return 1;
    }

    unsigned int surface_format_count = 0;

    if (vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, nullptr) != VK_SUCCESS)
    {
        std::cerr << "Failed to find surface formats." << std::endl;
        return 1;
    }

    if (surface_format_count == 0)
    {
        std::cerr << "No surface formats are found." << std::endl;
        return 1;
    }

    VkSurfaceFormatKHR *surface_formats = new VkSurfaceFormatKHR[surface_format_count];

    if (vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, surface_formats) != VK_SUCCESS)
    {
        std::cerr << "Failed to fetch surface formats." << std::endl;
        return 1;
    }

    VkSurfaceFormatKHR surface_format = {};
    for (unsigned int i = 0; i < surface_format_count; i++)
    {
        // TODO: Choose surface format properly.
        if (surface_formats[i].format == VK_FORMAT_R8G8B8A8_UNORM || surface_formats[i].format == VK_FORMAT_B8G8R8A8_UNORM)
        {
            surface_format = surface_formats[i];
            break;
        }
    }

    delete[] surface_formats;

    if (surface_format.format == VK_FORMAT_UNDEFINED)
    {
        std::cerr << "Surface does not support R8G8B8A8 format." << std::endl;
        return 1;
    }

    VkSurfaceCapabilitiesKHR surface_capabilities;
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities) != VK_SUCCESS)
    {
        std::cerr << "Failed to fetch surface capabilities." << std::endl;
        return 1;
    }

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Create a swapchain.
    ///

    unsigned int min_image_count = 2;

    if (surface_capabilities.maxImageCount >= 3)
    {
        min_image_count = 3;
    }
    else
    {
        min_image_count = surface_capabilities.minImageCount;
    }

    VkBool32 surface_supported = false;
    if (vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, queue_family_index, surface, &surface_supported) != VK_SUCCESS)
    {
        std::cerr << "Failed to check surface support of physical device." << std::endl;
        return 1;
    }

    if (!surface_supported)
    {
        std::cerr << "Physical device does not support surfaces." << std::endl;
        return 1;
    }

    VkSwapchainCreateInfoKHR swapchain_create_info;
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.pNext = nullptr;
    swapchain_create_info.flags = 0;
    swapchain_create_info.surface = surface;
    swapchain_create_info.minImageCount = min_image_count;
    swapchain_create_info.imageFormat = surface_format.format;
    swapchain_create_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; // TODO: Check if valid.
    swapchain_create_info.imageExtent = surface_capabilities.currentExtent;
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

    VkSwapchainKHR swapchain;
    if (vkCreateSwapchainKHR(device, &swapchain_create_info, nullptr, &swapchain) != VK_SUCCESS)
    {
        std::cerr << "Failed to create swapchain." << std::endl;
        return 1;
    }

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Swapchain image views.
    ///

    unsigned int swapchain_image_count = 0;
    if (vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, nullptr) != VK_SUCCESS)
    {
        std::cerr << "Failed to fetch swapchain images." << std::endl;
        return 1;
    }

    if (swapchain_image_count == 0)
    {
        std::cerr << "No swapchain images found." << std::endl;
        return 1;
    }

    VkImage *swapchain_images = new VkImage[swapchain_image_count];
    if (vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, swapchain_images) != VK_SUCCESS)
    {
        std::cerr << "Failed to fetch swapchain images." << std::endl;
        return 1;
    }

    VkImageViewCreateInfo image_view_create_info;
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.pNext = nullptr;
    image_view_create_info.flags = 0;
    image_view_create_info.image = swapchain_images[0];
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

    // Need Vulkan clean up, afterwards delete array (at end).
    VkImageView *image_views = new VkImageView[swapchain_image_count];
    for (unsigned int i = 0; i < swapchain_image_count; i++)
    {
        image_view_create_info.image = swapchain_images[i];

        if (vkCreateImageView(device, &image_view_create_info, nullptr, &image_views[i]) != VK_SUCCESS)
        {
            std::cerr << "Failed to create image view." << std::endl;
            return 1;
        }
    }

    delete[] swapchain_images;

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Read shader binaries.
    ///

    std::ifstream vertex_shader_file_stream("vert.spv", std::ios::binary | std::ios::ate);

    if (!vertex_shader_file_stream)
    {
        std::cerr << "Failed to load vertex shader file." << std::endl;
        return 1;
    }

    // Put the read head to the end of the file and check the pointer to find
    // out the file size.
    size_t vertex_shader_file_size = vertex_shader_file_stream.tellg();
    char *vertex_shader_file_buffer = new char[vertex_shader_file_size];
    // Move the read head back to the beginning so it can be read properly.
    vertex_shader_file_stream.seekg(0);
    vertex_shader_file_stream.read(vertex_shader_file_buffer, vertex_shader_file_size);
    vertex_shader_file_stream.close();

    std::ifstream fragment_shader_file_stream("frag.spv", std::ios::binary | std::ios::ate);

    if (!fragment_shader_file_stream)
    {
        std::cerr << "Failed to load fragment shader file." << std::endl;
        return 1;
    }

    // Put the read head to the end of the file and check the pointer to find
    // out the file size.
    size_t fragment_shader_file_size = fragment_shader_file_stream.tellg();
    char *fragment_shader_file_buffer = new char[fragment_shader_file_size];
    // Move the read head back to the beginning so it can be read properly.
    fragment_shader_file_stream.seekg(0);
    fragment_shader_file_stream.read(fragment_shader_file_buffer, fragment_shader_file_size);
    fragment_shader_file_stream.close();

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Shader modules creation.
    ///

    VkShaderModuleCreateInfo shader_module_create_info;
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.pNext = nullptr;
    shader_module_create_info.flags = 0;
    shader_module_create_info.codeSize = vertex_shader_file_size;
    shader_module_create_info.pCode = reinterpret_cast<uint32_t *>(vertex_shader_file_buffer);

    VkShaderModule vertex_shader_module;
    if (vkCreateShaderModule(device, &shader_module_create_info, nullptr, &vertex_shader_module) != VK_SUCCESS)
    {
        std::cerr << "Failed to create vertex shader module." << std::endl;
        return 1;
    }

    delete[] vertex_shader_file_buffer;

    shader_module_create_info.codeSize = fragment_shader_file_size;
    shader_module_create_info.pCode = reinterpret_cast<uint32_t *>(fragment_shader_file_buffer);

    VkShaderModule fragment_shader_module;
    if (vkCreateShaderModule(device, &shader_module_create_info, nullptr, &fragment_shader_module) != VK_SUCCESS)
    {
        std::cerr << "Failed to create fragment shader module." << std::endl;
        return 1;
    }

    delete[] fragment_shader_file_buffer;

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Create pipeline shader stages.
    ///

    VkPipelineShaderStageCreateInfo pipeline_vertex_shader_stage_create_info;
    pipeline_vertex_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipeline_vertex_shader_stage_create_info.pNext = nullptr;
    pipeline_vertex_shader_stage_create_info.flags = 0;
    pipeline_vertex_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    pipeline_vertex_shader_stage_create_info.module = vertex_shader_module;
    pipeline_vertex_shader_stage_create_info.pName = "main";
    pipeline_vertex_shader_stage_create_info.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo pipeline_fragment_shader_stage_create_info;
    pipeline_fragment_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipeline_fragment_shader_stage_create_info.pNext = nullptr;
    pipeline_fragment_shader_stage_create_info.flags = 0;
    pipeline_fragment_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    pipeline_fragment_shader_stage_create_info.module = fragment_shader_module;
    pipeline_fragment_shader_stage_create_info.pName = "main";
    pipeline_fragment_shader_stage_create_info.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_infos[] = {
        pipeline_vertex_shader_stage_create_info,
        pipeline_fragment_shader_stage_create_info};

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Create render pass.
    ///

    VkPipelineLayoutCreateInfo pipeline_layout_create_info;
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.pNext = nullptr;
    pipeline_layout_create_info.flags = 0;
    pipeline_layout_create_info.setLayoutCount = 0;
    pipeline_layout_create_info.pSetLayouts = nullptr;
    pipeline_layout_create_info.pushConstantRangeCount = 0;
    pipeline_layout_create_info.pPushConstantRanges = nullptr;

    VkPipelineLayout pipeline_layout;
    if (vkCreatePipelineLayout(device, &pipeline_layout_create_info, nullptr, &pipeline_layout) != VK_SUCCESS)
    {
        std::cerr << "Failed to create pipeline layout." << std::endl;
        return 1;
    }

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

    VkRenderPass render_pass;
    if (vkCreateRenderPass(device, &render_pass_create_info, nullptr, &render_pass) != VK_SUCCESS)
    {
        std::cerr << "Failed to create render pass.";
        return 1;
    }

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Create pipeline.
    ///

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
    viewport.width = surface_capabilities.currentExtent.width;
    viewport.height = surface_capabilities.currentExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = surface_capabilities.currentExtent.width;
    scissor.extent.height = surface_capabilities.currentExtent.width;

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

    VkPipeline pipeline;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &pipeline) != VK_SUCCESS)
    {
        std::cerr << "Failed to create pipeline." << std::endl;
        return 1;
    }

    // Not needed anymore because they are in the pipeline
    vkDestroyShaderModule(device, vertex_shader_module, nullptr);
    vkDestroyShaderModule(device, fragment_shader_module, nullptr);

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Create framebuffer.
    ///

    VkFramebuffer *framebuffers = new VkFramebuffer[swapchain_image_count];

    for (unsigned int i = 0; i < swapchain_image_count; i++)
    {
        VkFramebufferCreateInfo framebuffer_create_info;
        framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_create_info.pNext = nullptr;
        framebuffer_create_info.flags = 0;
        framebuffer_create_info.renderPass = render_pass;
        framebuffer_create_info.attachmentCount = 1;
        framebuffer_create_info.pAttachments = &image_views[i];
        framebuffer_create_info.width = surface_capabilities.currentExtent.width;
        framebuffer_create_info.height = surface_capabilities.currentExtent.height;
        framebuffer_create_info.layers = 1;

        // Need Vulkan clean up, afterwards delete array (at end).
        if (vkCreateFramebuffer(device, &framebuffer_create_info, nullptr, &framebuffers[i]) != VK_SUCCESS)
        {
            std::cerr << "Failed to create framebuffer." << std::endl;
            return 1;
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Set up command buffers.
    ///

    VkCommandPoolCreateInfo command_pool_create_info;
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.pNext = nullptr;
    command_pool_create_info.flags = 0;
    command_pool_create_info.queueFamilyIndex = queue_family_index;

    VkCommandPool command_pool;
    if (vkCreateCommandPool(device, &command_pool_create_info, nullptr, &command_pool) != VK_SUCCESS)
    {
        std::cerr << "Failed to create command pool." << std::endl;
        return 1;
    }

    VkCommandBufferAllocateInfo command_buffer_allocate_info;
    command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocate_info.pNext = nullptr;
    command_buffer_allocate_info.commandPool = command_pool;
    command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocate_info.commandBufferCount = swapchain_image_count; // Every frame buffer needs a command buffer

    VkCommandBuffer *command_buffers = new VkCommandBuffer[swapchain_image_count];

    if (vkAllocateCommandBuffers(device, &command_buffer_allocate_info, command_buffers) != VK_SUCCESS)
    {
        std::cerr << "Failed to allocate command buffers." << std::endl;
        return 1;
    }

    VkCommandBufferBeginInfo command_buffer_begin_info;
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.pNext = nullptr;
    command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    command_buffer_begin_info.pInheritanceInfo = nullptr; // Important if secondary command buffer is in use.

    VkClearValue clear_value = {0.0f, 0.0f, 0.0f, 1.0f}; // Set clear color

    // Record commands
    for (unsigned int i = 0; i < swapchain_image_count; i++)
    {
        if (vkBeginCommandBuffer(command_buffers[i], &command_buffer_begin_info) != VK_SUCCESS)
        {
            std::cerr << "Failed to begin command buffer recording." << std::endl;
            return 1;
        }

        VkRenderPassBeginInfo render_pass_begin_info;
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.pNext = nullptr;
        render_pass_begin_info.renderPass = render_pass;
        render_pass_begin_info.framebuffer = framebuffers[i];
        render_pass_begin_info.renderArea.offset.x = 0;
        render_pass_begin_info.renderArea.offset.y = 0;
        render_pass_begin_info.renderArea.extent.width = surface_capabilities.currentExtent.width;
        render_pass_begin_info.renderArea.extent.height = surface_capabilities.currentExtent.height;
        render_pass_begin_info.clearValueCount = 1;
        render_pass_begin_info.pClearValues = &clear_value;

        vkCmdBeginRenderPass(command_buffers[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE /* because only primary command buffers */);

        // Record: Bind pipeline and draw.
        vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdDraw(command_buffers[i], 3, 1, 0, 0); // Hard coded triangle (in vertex shader).

        vkCmdEndRenderPass(command_buffers[i]);

        if (vkEndCommandBuffer(command_buffers[i]) != VK_SUCCESS)
        {
            std::cerr << "Failed to stop command buffer recording." << std::endl;
            return 1;
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Create semaphores and fences for proper synchronization.
    ///

    VkSemaphoreCreateInfo semaphore_create_info;
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_create_info.pNext = nullptr;
    semaphore_create_info.flags = 0;

    VkSemaphore semaphore_image_available;
    VkSemaphore semaphore_image_rendered;

    if (vkCreateSemaphore(device, &semaphore_create_info, nullptr, &semaphore_image_available) != VK_SUCCESS)
    {
        std::cerr << "Failed to create semaphore." << std::endl;
        return 1;
    }

    if (vkCreateSemaphore(device, &semaphore_create_info, nullptr, &semaphore_image_rendered) != VK_SUCCESS)
    {
        std::cerr << "Failed to create semaphore." << std::endl;
        return 1;
    }

    VkFenceCreateInfo fence_create_info;
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.pNext = nullptr;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkFence *fences = new VkFence[swapchain_image_count];

    for (unsigned int i = 0; i < swapchain_image_count; i++)
    {
        if (vkCreateFence(device, &fence_create_info, nullptr, &fences[i]) != VK_SUCCESS)
        {
            std::cerr << "Failed to create fence." << std::endl;
            return 1;
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Main rendering and event loop.
    ///

    unsigned long frames_per_second = 0;
    auto last_frame_begin_time = std::chrono::steady_clock::now();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Draw frame:
        //   1.) Get image from swapchain
        //   2.) Render into this image
        //   3.) Give it back to the swapchain

        // Get next framebuffer image
        unsigned int image_index = 0;
        if (vkAcquireNextImageKHR(device, swapchain, std::numeric_limits<uint64_t>::max(), semaphore_image_available, VK_NULL_HANDLE, &image_index) != VK_SUCCESS)
        {
            std::cerr << "Failed to acquire next image.";
            return 1;
        }

        // Wait fence
        if (vkWaitForFences(device, 1, &fences[image_index], VK_TRUE, std::numeric_limits<uint64_t>::max()) != VK_SUCCESS)
        {
            std::cerr << "Failed to wait for fence." << std::endl;
            return 1;
        }

        // Reset fence
        if (vkResetFences(device, 1, &fences[image_index]) != VK_SUCCESS)
        {
            std::cerr << "Failed to wait for fence." << std::endl;
            return 1;
        }

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
        submit_info.pCommandBuffers = &command_buffers[image_index];
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &semaphore_image_rendered;

        if (vkQueueSubmit(queue, 1, &submit_info, fences[image_index]) != VK_SUCCESS)
        {
            std::cerr << "Failed submitting to queue." << std::endl;
            return 1;
        }

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
            return 1;
        }

        frames_per_second++;

        if (std::chrono::steady_clock::now() - last_frame_begin_time >= std::chrono::seconds(1))
        {
            std::stringstream title_stream;
            title_stream << "learn-vulkan - fps: " << frames_per_second;
            glfwSetWindowTitle(window, title_stream.str().c_str());
            frames_per_second = 0;
            last_frame_begin_time = std::chrono::steady_clock::now();
        }

        // TODO: Proper VSync
        //std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(16.6666666666667));
    }

    ///////////////////////////////////////////////////////////////////////////
    ///
    /// Clean up.
    ///

    glfwDestroyWindow(window);
    glfwTerminate();

    vkDeviceWaitIdle(device);

    for (unsigned int i = 0; i < swapchain_image_count; i++)
    {
        vkDestroyFence(device, fences[i], nullptr);
    }

    delete[] fences;

    vkDestroySemaphore(device, semaphore_image_available, nullptr);
    vkDestroySemaphore(device, semaphore_image_rendered, nullptr);

    // Not necessary because `vkDestroyCommandPool` does the same.
    vkFreeCommandBuffers(device, command_pool, swapchain_image_count, command_buffers);

    delete[] command_buffers;

    vkDestroyCommandPool(device, command_pool, nullptr);

    for (unsigned int i = 0; i < swapchain_image_count; i++)
    {
        vkDestroyFramebuffer(device, framebuffers[i], nullptr);
    }

    delete[] framebuffers;

    vkDestroyPipeline(device, pipeline, nullptr);

    vkDestroyRenderPass(device, render_pass, nullptr);

    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);

    for (unsigned int i = 0; i < swapchain_image_count; i++)
    {
        vkDestroyImageView(device, image_views[i], nullptr);
    }

    delete[] image_views;

    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}
