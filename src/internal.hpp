#include "vk_shader_manager.h"
#include <glslang/Include/glslang_c_interface.h>

#include "sqlite3ext.h"
#include "sqlite3.h"

#include <fstream>
#include <memory>
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

    class cache
    {
    private:
        static void close_db(sqlite3 *db);
        static std::unique_ptr<sqlite3, decltype(close_db)> open_db(const std::string &path);
        std::unique_ptr<sqlite3, decltype(close_db)> _db;
    public:
        cache(const std::string &path);
        ~cache() = default;

    };

    class context
    {
    private:
        static std::unordered_map<int, std::shared_ptr<context>> _contexts;
        static int _next_id;

        std::unique_ptr<cache> _cache;

    public:
        static int create(const std::string &cache_path);

        static void destroy(int id);

        static std::shared_ptr<context> get(int id);

        context(const std::string &cache_path);

        ~context() = default;
    };

    void create_context(const VsmContextCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VsmContext *pContext);

    void destroy_context(VsmContext context, const VkAllocationCallbacks *pAllocator);

    void compile_shader(VsmContext context, const VsmShaderCompileInfo *pCompileInfo);

    void find_shader(VsmContext context, const char *shaderName, VkBool32 *pFound);

    void remove_shader(VsmContext context, const char *shaderName);

    void clear_shader_cache(VsmContext context);

    void create_shader_module(VsmContext context, const VsmShaderModuleCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkShaderModule *pShaderModule);
}