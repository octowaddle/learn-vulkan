project('learn-vulkan', 'cpp', default_options: ['cpp_std=c++17'])

source_files = files('source/main.cpp')

dependencies = [
    dependency('vulkan'),
    dependency('glfw3')
]

executable = executable('learn-vulkan', source_files, dependencies : dependencies, install : true)

test('learn-vulkan', executable)
