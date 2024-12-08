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

#ifndef INTERNAL_HPP
#define INTERNAL_HPP

#include "vk_shader_manager.h"

#include <glslang/Include/glslang_c_interface.h>
#include <sqlite3.h>

#include <atomic>
#include <filesystem>
#include <fstream>
#include <memory>
#include <unordered_map>

namespace vsm
{
    class exception : public std::exception
    {
    private:
        VsmResult _result;
    public:
        exception(VsmResult result) : _result(result) {}
        const char *what() const noexcept override { return nullptr; }
        VsmResult result() const { return _result; }
    };

    void glsl_preprocess(std::unique_ptr<glslang_shader_t, decltype(&glslang_shader_delete)>& shader, const glslang_input_t *input);
    void glsl_parse(std::unique_ptr<glslang_shader_t, decltype(&glslang_shader_delete)>& shader, const glslang_input_t *input);
    void glsl_link(std::unique_ptr<glslang_program_t, decltype(&glslang_program_delete)>& program, int messages);

    class compiler
    {
    private:
        glslang_target_client_version_t _vk_version;
        glslang_target_language_version_t _spv_version;
    public:
        compiler(VsmVulkanVersion vk_version, VsmSPVVersion spv_version);
        ~compiler() = default;
        void compile(const std::string &name, VsmShaderStage stage, const std::string &source, std::vector<uint32_t> &code);
    };

    class repository
    {
    private:
        static std::unique_ptr<sqlite3, decltype(&sqlite3_close)> &&open_db(const std::string &path, bool shared);
        static void init_db(std::unique_ptr<sqlite3, decltype(&sqlite3_close)> &db);
        std::unique_ptr<sqlite3, decltype(&sqlite3_close)> _db;

    public:
        repository(const std::string &path, bool shared);
        ~repository() = default;
        void store(const std::string &name, VsmShaderStage stage, const std::vector<uint32_t> &code);
        void load(const std::string &name, std::vector<uint32_t> &code);
        void remove(const std::string &name);
        std::pair<bool, VsmShaderStage> query(const std::string &name);
        void clear();
    };

    void create_context(const VsmContextCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VsmContext *pContext);

    void destroy_context(VsmContext context, const VkAllocationCallbacks *pAllocator);

    void compile_shader(VsmContext context, const VsmShaderCompileInfo *pCompileInfo);

    void query_shader(VsmContext context, const char *shaderName, VkBool32 *pFound, VsmShaderStage *pShaderStage);

    void remove_shader(VsmContext context, const char *shaderName);

    void clear_shaders(VsmContext context);

    void create_shader_module(VsmContext context, const VsmShaderModuleCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkShaderModule *pShaderModule);
}

struct VsmContext_T
{
    std::unique_ptr<vsm::compiler> compiler;
    std::unique_ptr<vsm::repository> repository;
};

#endif