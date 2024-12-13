#include "internal.hpp"

std::string vsm::utilities::make_string(const char *raw)
{
    std::string result;
    if (raw != nullptr)
    {
        result = raw;
    }
    return result;
}

std::unique_ptr<vsm::compiler> &vsm::utilities::get_compiler(VsmContext context)
{
    if (context == VK_NULL_HANDLE)
    {
        throw vsm::exception(VSM_ERROR_INVALID_CONTEXT);
    }
    return context->compiler;
}

std::unique_ptr<vsm::repository> &vsm::utilities::get_repository(VsmContext context)
{
    if (context == VK_NULL_HANDLE)
    {
        throw vsm::exception(VSM_ERROR_INVALID_CONTEXT);
    }
    return context->repository;
}