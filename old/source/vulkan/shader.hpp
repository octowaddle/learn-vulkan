#pragma once

#include "external.hpp"
#include "noncopyable.hpp"
#include "device.hpp"

namespace vulkan
{

class Shader : NonCopyable
{
public:
    inline Shader(const std::string &binary_path, const Device &device);

    inline ~Shader();

    inline const impl::ShaderModule &operator*() const;

private:
    impl::ShaderModule impl = VK_NULL_HANDLE;

    impl::Device device;
};

inline Shader::Shader(const std::string &binary_path, const Device &device)
: device(*device)
{
    std::ifstream file_stream(binary_path, std::ios::binary | std::ios::ate);

    if (!file_stream)
    {
        std::cerr << "Failed to load shader file." << std::endl;
        throw 1;
    }

    // Put the read head to the end of the file and check the pointer to find
    // out the file size.
    size_t file_size = file_stream.tellg();
    char *file_buffer = new char[file_size];
    // Move the read head back to the beginning so it can be read properly.
    file_stream.seekg(0);
    file_stream.read(file_buffer, file_size);
    file_stream.close();

    impl = impl::create_shader_module(*device, file_buffer, file_size);

    delete[] file_buffer;
}

inline Shader::~Shader()
{
    impl::destroy_shader_module(device, impl);
}

inline const VkShaderModule &Shader::operator*() const
{
    return impl;
}

}
