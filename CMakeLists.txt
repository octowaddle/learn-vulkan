cmake_minimum_required(VERSION 3.20)
project(learn-vulkan LANGUAGES C VERSION 0.1.0)

add_custom_target(vertex-shader COMMAND glslc -fshader-stage=vert -o vert.spv "${CMAKE_CURRENT_SOURCE_DIR}/source/shaders/vert.glsl")
add_custom_target(fragment-shader COMMAND glslc -fshader-stage=frag -o frag.spv "${CMAKE_CURRENT_SOURCE_DIR}/source/shaders/frag.glsl")

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(learn-vulkan source/main.c)
add_dependencies(learn-vulkan vertex-shader fragment-shader)
target_link_libraries(learn-vulkan PRIVATE Vulkan::Vulkan glfw)
