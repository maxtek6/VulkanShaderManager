add_library(vsm_a STATIC ${VSM_SOURCES})
target_include_directories(vsm_a PUBLIC ${VSM_INCLUDE_DIR})
target_link_libraries(vsm_a PUBLIC sqlite3 Vulkan::Vulkan Vulkan::glslang Vulkan::shaderc_combined Vulkan::SPIRV-Tools)

#TODO: there is an issue with building shared library on Windows
#add_library(vsm SHARED ${VSM_SOURCES})
#target_include_directories(vsm PUBLIC ${VSM_INCLUDE_DIR})
#target_link_libraries(vsm PRIVATE sqlite3 Vulkan::Vulkan Vulkan::glslang)

add_library(VulkanShaderManager::Static ALIAS vsm_a)
if(NOT(WIN32))
    #add_library(VulkanShaderManager::Shared ALIAS vsm)
endif()