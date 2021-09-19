#ifndef SHADERMODULE_H
#define SHADERMODULE_H

#include <vulkan/vulkan.h>

VkShaderModule shader_module_create(const VkDevice device, const char *const path);

#endif
