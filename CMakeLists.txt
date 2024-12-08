cmake_minimum_required(VERSION 3.10)

project(VulkanShaderManager)

find_package(Vulkan REQUIRED COMPONENTS glslang)

set(VSM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(sqlite)
add_subdirectory(src)

