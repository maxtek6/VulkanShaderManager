/*
 * Copyright 2024 Maxtek Consulting
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "internal.hpp"

void vsm::glsl_preprocess(std::unique_ptr<glslang_shader_t, decltype(&glslang_shader_delete)> &shader, const glslang_input_t *input)
{
    if (!glslang_shader_preprocess(shader.get(), input))
    {
        throw exception(VSM_ERROR_COMPILE_PREPROCESS);
    }
}

void vsm::glsl_parse(std::unique_ptr<glslang_shader_t, decltype(&glslang_shader_delete)> &shader, const glslang_input_t *input)
{
    if (!glslang_shader_parse(shader.get(), input))
    {
        throw exception(VSM_ERROR_COMPILE_PARSE);
    }
}

void vsm::glsl_link(std::unique_ptr<glslang_program_t, decltype(&glslang_program_delete)> &program, int messages)
{
    if (!glslang_program_link(program.get(), messages))
    {
        throw exception(VSM_ERROR_COMPILE_LINK);
    }
}

vsm::compiler::compiler(VsmVulkanVersion vk_version, VsmSPVVersion spv_version)
{
    static const std::unordered_map<VsmVulkanVersion, glslang_target_client_version_t> vk_version_map = {
        {VSM_VULKAN_1_0, GLSLANG_TARGET_VULKAN_1_0},
        {VSM_VULKAN_1_1, GLSLANG_TARGET_VULKAN_1_1},
        {VSM_VULKAN_1_2, GLSLANG_TARGET_VULKAN_1_2},
        {VSM_VULKAN_1_3, GLSLANG_TARGET_VULKAN_1_3},
    };
    static const std::unordered_map<VsmSPVVersion, glslang_target_language_version_t> spv_version_map = {
        {VSM_SPV_1_0, GLSLANG_TARGET_SPV_1_0},
        {VSM_SPV_1_1, GLSLANG_TARGET_SPV_1_1},
        {VSM_SPV_1_2, GLSLANG_TARGET_SPV_1_2},
        {VSM_SPV_1_3, GLSLANG_TARGET_SPV_1_3},
        {VSM_SPV_1_4, GLSLANG_TARGET_SPV_1_4},
        {VSM_SPV_1_5, GLSLANG_TARGET_SPV_1_5},
        {VSM_SPV_1_6, GLSLANG_TARGET_SPV_1_6},
    };

    if (vk_version_map.find(vk_version) == vk_version_map.end())
    {
        throw exception(VSM_ERROR_VULKAN_VERSION);
    }

    if (spv_version_map.find(spv_version) == spv_version_map.end())
    {
        throw exception(VSM_ERROR_SPV_VERSION);
    }

    _vk_version = vk_version_map.at(vk_version);
    _spv_version = spv_version_map.at(spv_version);
}

void vsm::compiler::compile(const std::string &name, VsmShaderStage stage, const std::string &source, std::vector<uint32_t> &code)
{
    static const std::unordered_map<VsmShaderStage, glslang_stage_t> stage_map = {
        {VSM_SHADER_VERTEX, GLSLANG_STAGE_VERTEX},
        {VSM_SHADER_TESS_CONTROL, GLSLANG_STAGE_TESSCONTROL},
        {VSM_SHADER_TESS_EVALUATION, GLSLANG_STAGE_TESSEVALUATION},
        {VSM_SHADER_GEOMETRY, GLSLANG_STAGE_GEOMETRY},
        {VSM_SHADER_FRAGMENT, GLSLANG_STAGE_FRAGMENT},
        {VSM_SHADER_COMPUTE, GLSLANG_STAGE_COMPUTE},
        {VSM_SHADER_RAYGEN, GLSLANG_STAGE_RAYGEN},
        {VSM_SHADER_INTERSECT, GLSLANG_STAGE_INTERSECT},
        {VSM_SHADER_ANYHIT, GLSLANG_STAGE_ANYHIT},
        {VSM_SHADER_CLOSESTHIT, GLSLANG_STAGE_CLOSESTHIT},
        {VSM_SHADER_MISS, GLSLANG_STAGE_MISS},
        {VSM_SHADER_CALLABLE, GLSLANG_STAGE_CALLABLE},
        {VSM_SHADER_TASK, GLSLANG_STAGE_TASK},
        {VSM_SHADER_MESH, GLSLANG_STAGE_MESH},
    };

    if (stage_map.find(stage) == stage_map.end())
    {
        throw exception(VSM_ERROR_SHADER_STAGE);
    }

    const glslang_input_t input = {
        GLSLANG_SOURCE_GLSL,
        stage_map.at(stage),
        GLSLANG_CLIENT_VULKAN,
        _vk_version,
        GLSLANG_TARGET_SPV,
        _spv_version,
        source.c_str(),
        100,
        GLSLANG_NO_PROFILE,
        false,
        false,
        GLSLANG_MSG_DEFAULT_BIT,
        glslang_default_resource(),
    };

    std::unique_ptr<glslang_shader_t, decltype(&glslang_shader_delete)> shader(glslang_shader_create(&input), glslang_shader_delete);
    std::unique_ptr<glslang_program_t, decltype(&glslang_program_delete)> program(glslang_program_create(), glslang_program_delete);

    //glslang_initialize_process();
    vsm::glsl_preprocess(shader, &input);
    vsm::glsl_parse(shader, &input);
    glslang_program_add_shader(program.get(), shader.get());
    vsm::glsl_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT);
    glslang_program_SPIRV_generate(program.get(), stage_map.at(stage));
    code.resize(glslang_program_SPIRV_get_size(program.get()));
    glslang_program_SPIRV_get(program.get(), code.data());
}