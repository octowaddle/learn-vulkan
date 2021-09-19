#include <shadermodule.h>

#include <stdio.h>
#include <stdlib.h>

VkShaderModule shader_module_create(const VkDevice device, const char *const path) {
    FILE *shader_module_file = fopen(path, "rb");

    if (!shader_module_file) {
        fprintf(stderr, "error: failed to open vertex shader module file\n");
        exit(1);
    }

    fseek(shader_module_file, 0L, SEEK_END);
    const uint64_t shader_module_file_size = ftell(shader_module_file);
    fseek(shader_module_file, 0L, SEEK_SET);
    char *shader_module_file_buffer = malloc(shader_module_file_size * (sizeof *shader_module_file_buffer));
    fread(shader_module_file_buffer, shader_module_file_size, 1, shader_module_file);

    const VkShaderModuleCreateInfo shader_module_create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .codeSize = shader_module_file_size,
        .pCode = (uint32_t *) shader_module_file_buffer,
    };

    VkShaderModule shader_module = VK_NULL_HANDLE;
    VkResult result = vkCreateShaderModule(device, &shader_module_create_info, NULL, &shader_module);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "error: failed to create shader module\n");
        exit(1);
    }

    fclose(shader_module_file);
    free(shader_module_file_buffer);

    return shader_module;
}
