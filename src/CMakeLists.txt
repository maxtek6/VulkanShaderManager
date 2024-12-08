file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

add_library(vsm_a STATIC ${SOURCES})
target_include_directories(vsm_a PUBLIC ${VSM_INCLUDE_DIR})
target_link_libraries(vsm_a PRIVATE sqlite3 Vulkan::Vulkan Vulkan::glslang)

add_library(vsm SHARED ${SOURCES})
target_include_directories(vsm PUBLIC ${VSM_INCLUDE_DIR})
target_link_libraries(vsm PRIVATE sqlite3 Vulkan::Vulkan Vulkan::glslang)

add_library(VulkanShaderManager::Static ALIAS vsm_a)
if(NOT(WIN32))
    add_library(VulkanShaderManager::Shared ALIAS vsm)
endif()