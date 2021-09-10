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

    inline const VkShaderModule &get_handle() const;

private:
    VkShaderModule handle = VK_NULL_HANDLE;

    VkDevice device;
};

inline Shader::Shader(const std::string &binary_path, const Device &device)
: device(device.get_handle())
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

    VkShaderModuleCreateInfo shader_module_create_info;
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.pNext = nullptr;
    shader_module_create_info.flags = 0;
    shader_module_create_info.codeSize = file_size;
    shader_module_create_info.pCode = reinterpret_cast<uint32_t *>(file_buffer);

    if (vkCreateShaderModule(device.get_handle(), &shader_module_create_info, nullptr, &handle) != VK_SUCCESS)
    {
        std::cerr << "Failed to create vertex shader module." << std::endl;
        throw 1;
    }

    delete[] file_buffer;
}

inline Shader::~Shader()
{
    vkDestroyShaderModule(device, handle, nullptr);
}

inline const VkShaderModule &Shader::get_handle() const
{
    return handle;
}

}
