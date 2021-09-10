#pragma once

#include "external.hpp"
#include "device.hpp"
#include "instance.hpp"
#include "physical.hpp"
#include "shader.hpp"
#include "surface.hpp"
#include "swapchain.hpp"
#include "renderpass.hpp"
#include "framebuffer.hpp"
#include "pipeline.hpp"
#include "queue.hpp"

namespace vulkan
{

inline void init()
{
    const unsigned int width = 1280;
    const unsigned int height = 720;

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        throw 1;
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(width, height, "learn-vulkan", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create window." << std::endl;
        throw 1;
    }

    Instance instance;
    std::cout << "Passed instance stage." << std::endl;
    Physical physical(instance);
    std::cout << "Passed physical stage." << std::endl;
    Device device(physical);
    std::cout << "Passed device stage." << std::endl;
    Surface surface(instance, physical, window);
    std::cout << "Passed surfaec stage." << std::endl;
    Swapchain swapchain(physical, device, surface);
    std::cout << "Passed swapchain stage." << std::endl;
    RenderPass render_pass(device, surface);
    std::cout << "Passed render pass stage." << std::endl;
    Shader vertex_shader("vert.spv", device);
    std::cout << "Passed vertex shader stage." << std::endl;
    Shader fragment_shader("frag.spv", device);
    std::cout << "Passed fragment shader stage." << std::endl;
    Pipeline pipeline(device, surface, render_pass, vertex_shader, fragment_shader);
    std::cout << "Passed pipeline stage." << std::endl;
    Framebuffer framebuffer(physical, device, swapchain, render_pass, surface, pipeline);
    std::cout << "Passed framebuffer stage." << std::endl;
    Queue queue(physical, device, swapchain);
    std::cout << "Passed queue stage." << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        queue.present(framebuffer);
    }

    device.wait_idle();
}

}
