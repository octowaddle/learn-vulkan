#pragma once

#include "external.hpp"
#include "noncopyable.hpp"
#include "device.hpp"
#include "surface.hpp"

namespace vulkan
{

class RenderPass : NonCopyable
{
public:
    inline RenderPass(const Device &device, const Surface &surface);

    inline ~RenderPass();

    inline const impl::RenderPass &operator*() const;

private:
    impl::RenderPass impl = VK_NULL_HANDLE;

    impl::Device device;
};

inline RenderPass::RenderPass(const Device &device, const Surface &surface)
: impl(impl::create_render_pass(surface.get_format(), *device))
, device(*device)
{

}

inline RenderPass::~RenderPass()
{
    impl::destroy_render_pass(device, impl);
}

inline const VkRenderPass &RenderPass::operator*() const
{
    return impl;
}

}
