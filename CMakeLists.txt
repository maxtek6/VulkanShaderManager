cmake_minimum_required(VERSION 3.10)

project(VulkanShaderManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    find_package(Vulkan REQUIRED COMPONENTS glslang shaderc_combined SPIRV-Tools)
    set(VSM_LIBS sqlite3 Vulkan::Vulkan Vulkan::glslang Vulkan::shaderc_combined Vulkan::SPIRV-Tools)
else()
    find_package(PkgConfig REQUIRED)
    pkg_search_module(VULKAN REQUIRED IMPORTED_TARGET vulkan)
    pkg_search_module(SHADERC REQUIRED IMPORTED_TARGET shaderc_combined)
    pkg_search_module(SPIRV_TOOLS REQUIRED IMPORTED_TARGET SPIRV-Tools)
    set(VSM_LIBS sqlite3 glslang glslang-default-resource-limits PkgConfig::VULKAN PkgConfig::SHADERC PkgConfig::SPIRV_TOOLS)
endif()

option(VSM_TESTS "Build tests" ON)
option(VSM_EXAMPLE "Build example" ON)

add_subdirectory(sqlite)

set(VSM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE VSM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_subdirectory(src)

if(VSM_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(VSM_EXAMPLE)
    add_subdirectory(example)
endif()