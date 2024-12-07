#include "vk_shader_manager.h"
#include <glslang/Include/glslang_c_interface.h>

#include <fstream>
#include <iostream>
#include <unordered_map>

struct VsmContext_T
{
    int id;
};

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

    class context
    {
    private:
        static std::unordered_map<int,std::shared_ptr<context>> _contexts;
        static int _next_id;
    public:
        static std::shared_ptr<context> create(const std::string& cache_path);

        static void destroy(int id);

        context(const std::string& cache_path);

        ~context();
    };

    void create_context(const char *cachePath, VsmContext *pContext);
}