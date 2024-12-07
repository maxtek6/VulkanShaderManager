cmake_minimum_required(VERSION 3.0)

project(VulkanShaderManager)

find_package(Vulkan REQUIRED)

add_subdirectory(sqlite)

add_library(VulkanShaderManager SHARED
    VulkanShaderManager.cpp
    VulkanShaderManager.h
)

target_link_libraries(VulkanShaderManager PRIVATE sqlite3 Vulkan::Vulkan Vulkan::glslang)