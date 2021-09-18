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

    inline const impl::Pipeline &operator*() const;

private:
    impl::Pipeline impl = VK_NULL_HANDLE;

    impl::PipelineLayout pipeline_layout = VK_NULL_HANDLE;

    impl::Device device;
};

inline Pipeline::Pipeline(const Device &device, const Surface &surface, const RenderPass &render_pass, const Shader &vertex_shader, const Shader &fragment_shader)
: device(*device)
{
    impl::PipelineShaderStageCreateInfo pipeline_shader_stage_create_infos[] =
    {
        impl::create_pipeline_shader_stage_create_info(*vertex_shader, impl::ShaderStage::Vertex),
        impl::create_pipeline_shader_stage_create_info(*fragment_shader, impl::ShaderStage::Fragment)
    };

    pipeline_layout = impl::create_pipeline_layout(*device);

    auto viewport = impl::create_viewport(0, 0, surface.get_capabilities().currentExtent.width, surface.get_capabilities().currentExtent.height);
    auto scissor = impl::create_scissor(0, 0, surface.get_capabilities().currentExtent.width, surface.get_capabilities().currentExtent.height);

    impl = impl::create_pipeline(viewport, scissor, pipeline_shader_stage_create_infos, pipeline_layout, *render_pass, *device);

    // TODO: Delete shaders early.
};

inline Pipeline::~Pipeline()
{
    impl::destroy_pipeline(device, impl);
    impl::destroy_pipeline_layout(device, pipeline_layout);
}

inline const VkPipeline &Pipeline::operator*() const
{
    return impl;
}

}
