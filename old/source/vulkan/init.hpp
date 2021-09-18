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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(width, height, "learn-vulkan", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create window." << std::endl;
        throw 1;
    }

    Instance instance;
    Physical physical(instance);
    Device device(physical);
    Surface surface(instance, physical, window);
    Swapchain swapchain(physical, device, surface);
    RenderPass render_pass(device, surface);
    Shader vertex_shader("vert.spv", device);
    Shader fragment_shader("frag.spv", device);
    Pipeline pipeline(device, surface, render_pass, vertex_shader, fragment_shader);
    Framebuffer framebuffer(physical, device, swapchain, render_pass, surface, pipeline);
    Queue queue(physical, device, swapchain);


    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        queue.present(framebuffer);
    }

    device.wait_idle();
}

}
