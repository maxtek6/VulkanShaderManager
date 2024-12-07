#include "internal.hpp"


void vsm::create_context(const char *cachePath, VsmContext *pContext)
{
    if (pContext == nullptr)
    {
        throw vsm::exception(VSM_ERROR_NULL_HANDLE);
    }
    *pContext = new VsmContext_T();
    (*pContext)->id = vsm::context::create(cachePath);
}

VsmResult vsmCreateContext(const char *cachePath, VsmContext *pContext)
{
    VsmResult result = VSM_SUCCESS;
    try
    {
        vsm::create_context(cachePath, pContext);
    }
    catch (vsm::exception &e)
    {
        result = e.result();
    }
    return result;
}

void vsmDestroyContext(VsmContext context)
{
    
}

VsmResult vsmCompileShader(VsmContext context, const VsmShaderCompileInfo *pCompileInfo)
{

}

VsmResult vsmCreateShaderModule(VsmContext context, const VsmShaderModuleCreateInfo *pCreateInfo, VkShaderModule *pShaderModule)
{

}
