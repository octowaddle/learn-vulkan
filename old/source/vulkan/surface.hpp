#pragma once

#include "external.hpp"
#include "noncopyable.hpp"
#include "instance.hpp"
#include "physical.hpp"

namespace vulkan
{

class Surface : NonCopyable
{
public:
    inline Surface(const Instance &instance, const Physical &physical, GLFWwindow *window);

    inline ~Surface();

    inline const impl::Surface &operator*() const;

    inline const impl::SurfaceFormat &get_format() const;

    inline const impl::SurfaceCapabilities &get_capabilities() const;

private:
    impl::Surface impl;

    impl::SurfaceFormat format;

    impl::SurfaceCapabilities capabilities;

    impl::Instance instance;
};

inline Surface::Surface(const Instance &instance, const Physical &physical, GLFWwindow *window)
: instance(*instance)
{
    ///////////////////////////////////////////////////////////////////////////
    /// Create surface using GLFW.

    if (glfwCreateWindowSurface(*instance, window, nullptr, &impl) != VK_SUCCESS)
    {
        std::cerr << "Failed to create window surface." << std::endl;
        throw 1;
    }

    format = impl::get_surface_format(impl, *physical);
    capabilities = impl::get_surface_capabilities(impl, *physical);
}

inline Surface::~Surface()
{
    impl::destroy_surface(instance, impl);
}

inline const VkSurfaceCapabilitiesKHR &Surface::get_capabilities() const
{
    return capabilities;
}

inline const VkSurfaceFormatKHR &Surface::get_format() const
{
    return format;
}

inline const VkSurfaceKHR &Surface::operator*() const
{
    return impl;
}

}
