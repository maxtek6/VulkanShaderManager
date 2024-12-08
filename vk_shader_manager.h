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

#ifndef VK_SHADER_MANAGER_H
#define VK_SHADER_MANAGER_H

/**
 * @file vk_shader_manager.h
 * @brief Vulkan Shader Manager API
 * @author John R. Patek Sr.
 */

#include <vulkan/vulkan.h>

#ifdef _WIN32
#define VSM_API_CALL __declspec(dllexport)
#else
#define VSM_API_CALL
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief VSM status codes
     */
    typedef enum VsmResult
    {
        VSM_SUCCESS = 0,
        VSM_ERROR_NULL_HANDLE,
        VSM_ERROR_INVALID_CONTEXT,
        VSM_ERROR_CORRUPT_REPOSITORY,
        VSM_ERROR_SHADER_DNE,
        VSM_ERROR_VULKAN_VERSION,
        VSM_ERROR_SPV_VERSION,
        VSM_ERROR_SHADER_STAGE,
        VSM_ERROR_COMPILE_PREPROCESS,
        VSM_ERROR_COMPILE_PARSE,
        VSM_ERROR_COMPILE_LINK,
        VSM_ERROR_CREATE_MODULE,
    };

    /**
     * @brief VSM Vulkan versions
     */
    typedef enum VsmVulkanVersion
    {
        VSM_VULKAN_1_0,
        VSM_VULKAN_1_1,
        VSM_VULKAN_1_2,
        VSM_VULKAN_1_3,
        VSM_VULKAN_MAX_ENUM,
    };

    /**
     * @brief VSM SPIR-V versions
     */
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

    /**
     * @brief VSM shader stages
     */
    typedef enum VsmShaderStage
    {
        VSM_SHADER_VERTEX,
        VSM_SHADER_TESS_CONTROL,
        VSM_SHADER_TESS_EVALUATION,
        VSM_SHADER_GEOMETRY,
        VSM_SHADER_FRAGMENT,
        VSM_SHADER_COMPUTE,
        VSM_SHADER_RAYGEN,
        VSM_SHADER_INTERSECT,
        VSM_SHADER_ANYHIT,
        VSM_SHADER_CLOSESTHIT,
        VSM_SHADER_MISS,
        VSM_SHADER_CALLABLE,
        VSM_SHADER_TASK,
        VSM_SHADER_MESH,
        VSM_SHADER_MAX_ENUM,
    };

    typedef struct VsmContextCreateInfo
    {
        const char *repositoryPath;
        bool shared;
        VsmVulkanVersion vulkanVersion;
        VsmSPVVersion spvVersion;
    } VsmContextCreateInfo;

    typedef struct VsmShaderCompileInfo
    {
        const char *shaderName;
        const char *shaderSource;
        VsmShaderStage shaderStage;
    } VsmShaderCompileInfo;

    typedef struct VsmShaderModuleCreateInfo
    {
        VkDevice device;
        const char *shaderName;
        void *pNext;
        VkShaderModuleCreateFlags flags;
    } VsmShaderModuleCreateInfo;

    VK_DEFINE_HANDLE(VsmContext);

    VSM_API_CALL VsmResult vsmCreateContext(const VsmContextCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VsmContext *pContext);

    VSM_API_CALL void vsmDestroyContext(VsmContext context, const VkAllocationCallbacks *pAllocator);

    VSM_API_CALL VsmResult vsmCompileShader(VsmContext context, const VsmShaderCompileInfo *pCompileInfo);

    VSM_API_CALL VsmResult vsmQueryShader(VsmContext context, const char *shaderName, VkBool32 *pFound, VsmShaderStage *pShaderStage);

    VSM_API_CALL VsmResult vsmRemoveShader(VsmContext context, const char *shaderName);

    VSM_API_CALL VsmResult vsmClearShaders(VsmContext context);

    VSM_API_CALL VsmResult vsmCreateShaderModule(VsmContext context, const VsmShaderModuleCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkShaderModule *pShaderModule);

#ifdef __cplusplus
}
#endif

#endif