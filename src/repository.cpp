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

std::unique_ptr<sqlite3, decltype(&sqlite3_close)> vsm::repository::open_db(const std::string &path, bool shared)
{
    int mutex_flags = shared ? SQLITE_OPEN_FULLMUTEX : SQLITE_OPEN_NOMUTEX;
    int open_flags = SQLITE_OPEN_READWRITE;
    sqlite3 *db;
    const char *filename = path.c_str();

    // attempt to create new database
    if (path.empty() && !std::filesystem::exists(path))
    {
        open_flags |= SQLITE_OPEN_CREATE;
    }

    if (path.empty())
    {
        filename = ":memory:";
    }

    if (sqlite3_open_v2(filename, &db, open_flags, nullptr) != SQLITE_OK)
    {
        throw vsm::exception(VSM_ERROR_REPOSITORY_OPEN);
    }
    
    return std::move(std::unique_ptr<sqlite3, decltype(&sqlite3_close)>(db, sqlite3_close));
}

void vsm::repository::init_db(std::unique_ptr<sqlite3, decltype(&sqlite3_close)> &db)
{
    char *err_msg = nullptr;
    static const std::string sql = "CREATE TABLE IF NOT EXISTS shaders (name TEXT NOT NULL, stage INTEGER NOT NULL, code BLOB NOT NULL);"
                                   "CREATE UNIQUE INDEX IF NOT EXISTS shader_index ON shaders(name);";

    if (sqlite3_exec(db.get(), sql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK)
    {
        throw vsm::exception(VSM_ERROR_REPOSITORY_INIT);
    }
}

vsm::repository::repository(const std::string &path, bool shared) : _db(open_db(path, shared))
{
    init_db(_db);
}

void vsm::repository::store(const std::string &name, VsmShaderStage stage, const std::vector<uint32_t> &code)
{
    sqlite3_stmt *stmt;
    static const std::string sql = "INSERT OR REPLACE INTO shaders (name, stage, code) VALUES (?, ?, ?);";

    if (sqlite3_prepare_v2(_db.get(), sql.c_str(), sql.size(), &stmt, nullptr) != SQLITE_OK)
    {
        throw vsm::exception(VSM_ERROR_REPOSITORY_STORE);
    }

    if (sqlite3_bind_text(stmt, 1, name.c_str(), name.size(), SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, stage) != SQLITE_OK ||
        sqlite3_bind_blob(stmt, 3, code.data(), code.size() * sizeof(uint32_t), SQLITE_STATIC) != SQLITE_OK)
    {
        throw vsm::exception(VSM_ERROR_REPOSITORY_STORE);
    }

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        throw vsm::exception(VSM_ERROR_REPOSITORY_STORE);
    }

    sqlite3_finalize(stmt);
}

void vsm::repository::load(const std::string &name, std::vector<uint32_t> &code)
{
    sqlite3_stmt *stmt;
    static const std::string sql = "SELECT code FROM shaders WHERE name = ?;";

    if (sqlite3_prepare_v2(_db.get(), sql.c_str(), sql.size(), &stmt, nullptr) != SQLITE_OK)
    {
        throw vsm::exception(VSM_ERROR_REPOSITORY_LOAD);
    }

    if (sqlite3_bind_text(stmt, 1, name.c_str(), name.size(), SQLITE_STATIC) != SQLITE_OK)
    {
        throw vsm::exception(VSM_ERROR_REPOSITORY_LOAD);
    }

    if (sqlite3_step(stmt) != SQLITE_ROW)
    {
        throw vsm::exception(VSM_ERROR_REPOSITORY_LOAD);
    }

    const void *data = sqlite3_column_blob(stmt, 0);
    int size = sqlite3_column_bytes(stmt, 0) / sizeof(uint32_t);

    code.resize(size);
    memcpy(code.data(), data, size * sizeof(uint32_t));

    sqlite3_finalize(stmt);
}

std::pair<bool, VsmShaderStage> vsm::repository::query(const std::string &name)
{
    sqlite3_stmt *stmt;
    static const std::string sql = "SELECT stage FROM shaders WHERE name = ?;";

    if (sqlite3_prepare_v2(_db.get(), sql.c_str(), sql.size(), &stmt, nullptr) != SQLITE_OK)
    {
        throw vsm::exception(VSM_ERROR_REPOSITORY_QUERY);
    }

    if (sqlite3_bind_text(stmt, 1, name.c_str(), name.size(), SQLITE_STATIC) != SQLITE_OK)
    {
        throw vsm::exception(VSM_ERROR_REPOSITORY_QUERY);
    }

    if (sqlite3_step(stmt) != SQLITE_ROW)
    {
        throw vsm::exception(VSM_ERROR_REPOSITORY_QUERY);
    }

    VsmShaderStage stage = static_cast<VsmShaderStage>(sqlite3_column_int(stmt, 0));

    sqlite3_finalize(stmt);

    return std::make_pair(true, stage);
}

void vsm::repository::remove(const std::string &name)
{
    sqlite3_stmt *stmt;
    static const std::string sql = "DELETE FROM shaders WHERE name = ?;";

    if (sqlite3_prepare_v2(_db.get(), sql.c_str(), sql.size(), &stmt, nullptr) != SQLITE_OK)
    {
        throw vsm::exception(VSM_ERROR_REPOSITORY_REMOVE);
    }

    if (sqlite3_bind_text(stmt, 1, name.c_str(), name.size(), SQLITE_STATIC) != SQLITE_OK)
    {
        throw vsm::exception(VSM_ERROR_REPOSITORY_REMOVE);
    }

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        throw vsm::exception(VSM_ERROR_REPOSITORY_REMOVE);
    }

    sqlite3_finalize(stmt);
}

void vsm::repository::clear()
{
    static const std::string sql = "DELETE FROM shaders;";

    if (sqlite3_exec(_db.get(), sql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK)
    {
        throw vsm::exception(VSM_ERROR_REPOSITORY_CLEAR);
    }
}
