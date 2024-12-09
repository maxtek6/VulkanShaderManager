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

#include <vk_shader_manager.h>

#include <functional>
#include <iostream>
#include <sstream>
#include <unordered_map>

#ifndef __FUNCTION_NAME__
#ifdef WIN32 // WINDOWS
#define __FUNCTION_NAME__ __FUNCTION__
#else //*NIX
#define __FUNCTION_NAME__ __func__
#endif
#endif

#define TEST_PASS 0
#define TEST_FAIL 1
#define TEST_ASSERT(COND) test_assert((COND), __FILE__, __FUNCTION_NAME__, __LINE__, #COND)

static void test_assert(
    bool condition,
    const std::string &file,
    const std::string &function,
    int line,
    const std::string &description);

// API tests
namespace api
{
    static void create_context();
    static void destroy_context();
    static void compile_shader(){}
    static void query_shader(){}
    static void remove_shader(){}
    static void clear_shaders(){}
    static void create_shader_module(){}
}

#define TEST_CASE(NAME) {#NAME, NAME}

int main(int argc, const char **argv)
{
    const std::unordered_map<std::string, std::function<void()>> test_cases = {
        TEST_CASE(api::create_context),
        TEST_CASE(api::destroy_context),
        TEST_CASE(api::compile_shader),
        TEST_CASE(api::query_shader),
        TEST_CASE(api::remove_shader),
        TEST_CASE(api::clear_shaders),
        TEST_CASE(api::create_shader_module),
    };
    int result = TEST_PASS;
    if (argc > 1)
    {
        try
        {
            test_cases.at(argv[1])();
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << std::endl;
            result = TEST_FAIL;
        }
    }
    return result;
}

void test_assert(
    bool condition,
    const std::string &file,
    const std::string &function,
    int line,
    const std::string &description)
{
    std::stringstream error_stream;
    if (!condition)
    {
        error_stream << file << ":" << function << ":" << line << ":"
                     << " failed to assert \"" << description << "\"";
        throw std::runtime_error(error_stream.str());
    }
}

void api::create_context()
{
    VsmContextCreateInfo create_info = {
        nullptr,
        false,
        VSM_VULKAN_1_2,
        VSM_SPV_1_5,
    };
    VsmContext context;
    VsmResult result = vsmCreateContext(&create_info, nullptr, &context);
    // created with valid data
    TEST_ASSERT(result == VSM_SUCCESS);
    vsmDestroyContext(context, nullptr);
    create_info.spvVersion = VSM_SPV_MAX_ENUM;
    result = vsmCreateContext(&create_info, nullptr, &context);
    // created with invalid SPV version
    TEST_ASSERT(result == VSM_ERROR_SPV_VERSION);    
}

void api::destroy_context()
{
    VsmContextCreateInfo create_info = {
        nullptr,
        false,
        VSM_VULKAN_1_2,
        VSM_SPV_1_5,
    };
    VsmContext context;
    // discardd the result
    static_cast<void>(vsmCreateContext(&create_info, nullptr, &context));
    // double free, should never throw error
    vsmDestroyContext(context, nullptr);
    vsmDestroyContext(context, nullptr);
}