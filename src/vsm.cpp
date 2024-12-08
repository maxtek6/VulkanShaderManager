#include "internal.hpp"

VsmResult vsmCreateContext(const VsmContextCreateInfo *pCreateInfo, VkAllocationCallbacks *pAllocator, VsmContext *pContext)
{
    VsmResult result(VSM_SUCCESS);
    try
    {
        vsm::create_context(pCreateInfo, pAllocator, pContext);
    }
    catch (vsm::exception &e)
    {
        result = e.result();
    }
    return result;
}

void vsmDestroyContext(VsmContext context, const VkAllocationCallbacks *pAllocator)
{
    vsm::destroy_context(context, pAllocator);
}

VsmResult vsmCompileShader(VsmContext context, const VsmShaderCompileInfo *pCompileInfo)
{
    VsmResult result(VSM_SUCCESS);
    try
    {
        vsm::compile_shader(context, pCompileInfo);
    }
    catch (vsm::exception &e)
    {
        result = e.result();
    }
    return result;
}

VsmResult vsmFindShader(VsmContext context, const char *shaderName, VkBool32 *pFound)
{
    VsmResult result(VSM_SUCCESS);
    try
    {
        vsm::find_shader(context, shaderName, pFound);
    }
    catch (vsm::exception &e)
    {
        result = e.result();
    }
    return result;
}

VsmResult vsmRemoveShader(VsmContext context, const char *shaderName)
{
    VsmResult result(VSM_SUCCESS);
    try
    {
        vsm::remove_shader(context, shaderName);
    }
    catch (vsm::exception &e)
    {
        result = e.result();
    }
    return result;
}

VsmResult vsmClearShaderCache(VsmContext context)
{
    VsmResult result(VSM_SUCCESS);
    try
    {
        vsm::clear_shader_cache(context);
    }
    catch (vsm::exception &e)
    {
        result = e.result();
    }
    return result;
}

VsmResult vsmCreateShaderModule(VsmContext context, const VsmShaderModuleCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkShaderModule *pShaderModule)
{
    VsmResult result(VSM_SUCCESS);
    try
    {
        vsm::create_shader_module(context, pCreateInfo, pAllocator, pShaderModule);
    }
    catch (vsm::exception &e)
    {
        result = e.result();
    }
    return result;
}

void vsm::create_context(const VsmContextCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VsmContext *pContext)
{
    if (!pCreateInfo || !pContext)
    {
        throw vsm::exception(VsmResult::VSM_ERROR_NULL_HANDLE);
    }
    *pContext = reinterpret_cast<VsmContext_T*>(malloc(sizeof(VsmContext_T)));
    (*pContext)->id = vsm::context::create(pCreateInfo->cachePath);
}

void vsm::destroy_context(VsmContext context, const VkAllocationCallbacks *pAllocator)
{
    if (context)
    {
        vsm::context::destroy(context->id);
        free(context);
    }
}