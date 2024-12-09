add_executable(unit
    unit.cpp)

target_link_libraries(unit PRIVATE VulkanShaderManager::Static)

add_test(NAME vsmCreateContext COMMAND unit api::create_context)
add_test(NAME vsmDestroyContext COMMAND unit api::destroy_context)
add_test(NAME vsmCompileShader COMMAND unit api::compile_shader)