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

#define VSM_API_DEF(FUNC, ENTRYPOINT, ...) \
    VsmResult FUNC(__VA_ARGS__)             \
    {                                       \
        VsmResult result(VSM_SUCCESS);      \
        try                                 \
        {                                   \
            ENTRYPOINT;                     \
        }                                   \
        catch (vsm::exception & e)          \
        {                                   \
            result = e.result();            \
        }                                   \
        return result;                      \
    }

VSM_API_DEF(vsmCreateContext, vsm::create_context(pCreateInfo, pAllocator, pContext), const VsmContextCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VsmContext *pContext);

void vsmDestroyContext(VsmContext context, const VkAllocationCallbacks *pAllocator)
{
    if (context != VK_NULL_HANDLE)
    {
        vsm::destroy_context(context, pAllocator);
    }
}

VSM_API_DEF(vsmCompileShader, vsm::compile_shader(context, pCompileInfo), VsmContext context, const VsmShaderCompileInfo *pCompileInfo);
VSM_API_DEF(vsmQueryShader, vsm::query_shader(context, shaderName, pFound, pShaderStage), VsmContext context, const char *shaderName, VkBool32 *pFound, VsmShaderStage *pShaderStage);
VSM_API_DEF(vsmRemoveShader, vsm::remove_shader(context, shaderName), VsmContext context, const char *shaderName);
VSM_API_DEF(vsmClearShaders, vsm::clear_shaders(context), VsmContext context);
VSM_API_DEF(vsmCreateShaderModule, vsm::create_shader_module(context, pCreateInfo, pAllocator, pShaderModule), VsmContext context, const VsmShaderModuleCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkShaderModule *pShaderModule);

void vsm::create_context(const VsmContextCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VsmContext *pContext)
{
    if (pContext == nullptr)
    {
        throw vsm::exception(VSM_ERROR_NULL_HANDLE);
    }
    std::unique_ptr<VsmContext_T> context(new VsmContext_T);
    std::unique_ptr<vsm::compiler> compiler = std::make_unique<vsm::compiler>(pCreateInfo->vulkanVersion, pCreateInfo->spvVersion);
    std::unique_ptr<vsm::repository> repository = std::make_unique<vsm::repository>(pCreateInfo->repositoryPath, pCreateInfo->shared);
    context->compiler = std::move(compiler);
    context->repository = std::move(repository);
    *pContext = context.release();
}

void vsm::destroy_context(VsmContext context, const VkAllocationCallbacks *pAllocator)
{
    if (context != VK_NULL_HANDLE)
    {
        // TODO: check if reset() is necessary
        context->compiler.reset();
        context->repository.reset();
        delete context;
    }
}

void vsm::compile_shader(VsmContext context, const VsmShaderCompileInfo *pCompileInfo)
{
    if (context == VK_NULL_HANDLE)
    {
        throw vsm::exception(VSM_ERROR_INVALID_CONTEXT);
    }
    const std::unique_ptr<vsm::compiler> &compiler = context->compiler;
    const std::unique_ptr<vsm::repository> &repository = context->repository;
    std::vector<uint32_t> code;

    compiler->compile(pCompileInfo->shaderName, pCompileInfo->shaderStage, pCompileInfo->shaderSource, code);
    repository->store(pCompileInfo->shaderName, pCompileInfo->shaderStage, code);
}

void vsm::query_shader(VsmContext context, const char *shaderName, VkBool32 *pFound, VsmShaderStage *pShaderStage)
{
    if (context == VK_NULL_HANDLE)
    {
        throw vsm::exception(VSM_ERROR_INVALID_CONTEXT);
    }

    const std::unique_ptr<vsm::repository> &repository = context->repository;
    const std::pair<bool, VsmShaderStage> result = repository->query(shaderName);
    
    if (pFound != nullptr)
    {
        *pFound = result.first ? VK_TRUE : VK_FALSE;
    }
    
    if (pShaderStage != nullptr)
    {
        *pShaderStage = result.second;
    }
}

void vsm::remove_shader(VsmContext context, const char *shaderName)
{
    if (context == VK_NULL_HANDLE)
    {
        throw vsm::exception(VSM_ERROR_INVALID_CONTEXT);
    }
    const std::unique_ptr<vsm::repository> &repository = context->repository;
    repository->remove(shaderName);
}

void vsm::clear_shaders(VsmContext context)
{
    if (context == VK_NULL_HANDLE)
    {
        throw vsm::exception(VSM_ERROR_INVALID_CONTEXT);
    }
    const std::unique_ptr<vsm::repository> &repository = context->repository;
    repository->clear();
}

void vsm::create_shader_module(VsmContext context, const VsmShaderModuleCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkShaderModule *pShaderModule)
{
    if (context == VK_NULL_HANDLE)
    {
        throw vsm::exception(VSM_ERROR_INVALID_CONTEXT);
    }
    const std::unique_ptr<vsm::repository> &repository = context->repository;
    std::vector<uint32_t> code;

    repository->load(pCreateInfo->shaderName, code);
    
    const VkShaderModuleCreateInfo createInfo = {
        VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        pCreateInfo->pNext,
        pCreateInfo->flags,
        code.size() * sizeof(uint32_t),
        code.data()
    };

    if (vkCreateShaderModule(pCreateInfo->device, &createInfo, pAllocator, pShaderModule) != VK_SUCCESS)
    {
        throw vsm::exception(VSM_ERROR_CREATE_MODULE);
    }
}