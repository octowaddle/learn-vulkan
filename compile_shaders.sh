#!/usr/bin/env bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

glslangValidator -V "${SCRIPT_DIR}/source/shader.vert.glsl"
glslangValidator -V "${SCRIPT_DIR}/source/shader.frag.glsl"
