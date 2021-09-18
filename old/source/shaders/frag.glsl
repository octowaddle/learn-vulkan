#version 460
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 pass_color;

layout (location = 0) out vec4 out_color;

void main()
{
    out_color = vec4(pass_color, 1.0);
}
