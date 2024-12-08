#ifndef VK_SHADER_MANAGER_H
#define VK_SHADER_MANAGER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <vulkan/vulkan.h>

    typedef enum VsmResult
    {
        VSM_SUCCESS = 0,
        VSM_ERROR_NULL_HANDLE,
        VSM_ERROR_INVALID_CONTEXT,
        VSM_ERROR_CORRUPT_CACHE,
        VSM_ERROR_SHADER_DNE,
        VSM_ERROR_VULKAN_VERSION,
        VSM_ERROR_SPV_VERSION,
    };

    typedef enum VsmVulkanVersion
    {
        VSM_VULKAN_1_0,
        VSM_VULKAN_1_1,
        VSM_VULKAN_1_2,
        VSM_VULKAN_1_3,
        VSM_VULKAN_MAX_ENUM,
    };

    typedef enum VsmSPVVersion
    {
        VSM_SPV_1_0,
        VSM_SPV_1_1,
        VSM_SPV_1_2,
        VSM_SPV_1_3,
        VSM_SPV_1_4,
        VSM_SPV_1_5,
        VSM_SPV_1_6,
        VSM_SPV_MAX_ENUM,
    };

    typedef struct VsmContextCreateInfo
    {
        const char *cachePath;
        uint32_t maxCacheSize;
    } VsmContextCreateInfo;

    typedef struct VsmShaderCompileInfo
    {
        const char *shaderName;
        const char *shaderPath;
        VsmVulkanVersion vulkanVersion;
        VsmSPVVersion spvVersion;
    } VsmShaderCompileInfo;

    typedef struct VsmShaderModuleCreateInfo
    {
        const char *shaderName;
        void *pNext;
        // VkShaderCreateFlagsEXT flags;
    } VsmShaderModuleCreateInfo;

    VK_DEFINE_HANDLE(VsmContext);

    VsmResult vsmCreateContext(const VsmContextCreateInfo *pCreateInfo, VkAllocationCallbacks *pAllocator, VsmContext *pContext);

    void vsmDestroyContext(VsmContext context, const VkAllocationCallbacks *pAllocator);

    VsmResult vsmCompileShader(VsmContext context, const VsmShaderCompileInfo *pCompileInfo);

    VsmResult vsmFindShader(VsmContext context, const char *shaderName, VkBool32 *pFound);

    VsmResult vsmRemoveShader(VsmContext context, const char *shaderName);

    VsmResult vsmClearShaderCache(VsmContext context);

    VsmResult vsmCreateShaderModule(VsmContext context, const VsmShaderModuleCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkShaderModule *pShaderModule);

#ifdef __cplusplus
}
#endif

#endif