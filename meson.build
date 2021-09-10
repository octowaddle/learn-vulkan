project('learn-vulkan', 'cpp', default_options: ['cpp_std=c++17'])

###############################################################################
#
# Add shader compilation targets that use `glslc`.
#

glsl_compiler = find_program('glslc', required : true)

vertex_shader = custom_target('vertex-shader',
  output : 'shader.vert.spv',
  input : 'source/shader.vert.glsl',
  command : [glsl_compiler, '-fshader-stage=vert', '-o', '@OUTPUT@','@INPUT@'],
  build_always : true
)

fragment_shader = custom_target('fragment-shader',
  output : 'shader.frag.spv',
  input : 'source/shader.frag.glsl',
  command : [glsl_compiler, '-fshader-stage=frag', '-o', '@OUTPUT@','@INPUT@'],
  build_always : true
)

###############################################################################
#
# Find dependencies.
#

compiler = meson.get_compiler('cpp')

vulkan_dependency = dependency('vulkan', required: false)
if not vulkan_dependency.found()
    vulkan_dependency = compiler.find_library('vulkan', required: false)
endif

glfw3_dependency = dependency('glfw3', required: false)
if not glfw3_dependency.found()
    glfw3_dependency = compiler.find_library('glfw3', required: false)
endif

dependencies = [
    vulkan_dependency,
    glfw3_dependency,
]

###############################################################################
#
# Add executable compilation target as a test.
#

source_files = files('source/main.cpp')

executable = executable(
    'learn-vulkan',
    [source_files, vertex_shader, fragment_shader],
    dependencies : dependencies,
    install : true
)

test('learn-vulkan', executable)
