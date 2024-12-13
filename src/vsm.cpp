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

#define VSM_API_BEGIN(NAME, ...)       \
    VsmResult NAME(__VA_ARGS__)        \
    {                                  \
        VsmResult result(VSM_SUCCESS); \
        try                            \
        {

#define VSM_API_END            \
    }                          \
    catch (vsm::exception & e) \
    {                          \
        result = e.result();   \
    }                          \
    return result;             \
    }

VSM_API_BEGIN(vsmCreateContext, const VsmContextCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VsmContext *pContext)
if (pContext == nullptr)
{
    throw vsm::exception(VSM_ERROR_NULL_HANDLE);
}
std::unique_ptr<VsmContext_T> context(new VsmContext_T);
std::unique_ptr<vsm::compiler> compiler = std::make_unique<vsm::compiler>(pCreateInfo->vulkanVersion, pCreateInfo->spvVersion);
std::unique_ptr<vsm::repository> repository = std::make_unique<vsm::repository>(vsm::utilities::make_string(pCreateInfo->repositoryPath), pCreateInfo->shared);
context->compiler = std::move(compiler);
context->repository = std::move(repository);
*pContext = context.release();
VSM_API_END

void vsmDestroyContext(VsmContext context, const VkAllocationCallbacks *pAllocator)
{
    if (context != VK_NULL_HANDLE)
    {
        // TODO: check if reset() is necessary
        context->compiler.reset();
        context->repository.reset();
        delete context;
    }
}

VSM_API_BEGIN(vsmCompileShader, VsmContext context, const VsmShaderCompileInfo *pCompileInfo)
if (pCompileInfo == nullptr)
{
    throw vsm::exception(VSM_ERROR_NULL_HANDLE);
}
std::vector<uint32_t> code;
vsm::utilities::get_compiler(context)->compile(vsm::utilities::make_string(pCompileInfo->shaderName), pCompileInfo->shaderStage, vsm::utilities::make_string(pCompileInfo->shaderSource), code);
vsm::utilities::get_repository(context)->store(vsm::utilities::make_string(pCompileInfo->shaderName), pCompileInfo->shaderStage, code);
VSM_API_END

VSM_API_BEGIN(vsmQueryShader, VsmContext context, const char *shaderName, VkBool32 *pFound, VsmShaderStage *pShaderStage)
const std::unique_ptr<vsm::repository> &repository = vsm::utilities::get_repository(context);
const std::pair<bool, VsmShaderStage> result = repository->query(vsm::utilities::make_string(shaderName));
if (pFound != nullptr)
{
    *pFound = result.first ? VK_TRUE : VK_FALSE;
}
if (pShaderStage != nullptr)
{
    *pShaderStage = result.second;
}
VSM_API_END

VSM_API_BEGIN(vsmRemoveShader, VsmContext context, const char *shaderName)
vsm::utilities::get_repository(context)->remove(vsm::utilities::make_string(shaderName));
VSM_API_END

VSM_API_BEGIN(vsmClearShaders, VsmContext context)
vsm::utilities::get_repository(context)->clear();
VSM_API_END

VSM_API_BEGIN(vsmCreateShaderModule, VsmContext context, const VsmShaderModuleCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkShaderModule *pShaderModule)
std::vector<uint32_t> code;
vsm::utilities::get_repository(context)->load(vsm::utilities::make_string(pCreateInfo->shaderName), code);
const VkShaderModuleCreateInfo createInfo = {
    VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    pCreateInfo->pNext,
    pCreateInfo->flags,
    code.size() * sizeof(uint32_t),
    code.data()};
if (vkCreateShaderModule(pCreateInfo->device, &createInfo, pAllocator, pShaderModule) != VK_SUCCESS)
{
    throw vsm::exception(VSM_ERROR_CREATE_MODULE);
}
VSM_API_END