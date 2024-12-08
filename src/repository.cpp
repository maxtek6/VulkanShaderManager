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

std::unique_ptr<sqlite3, decltype(&sqlite3_close)> &&vsm::repository::open_db(const std::string &path, bool shared)
{
    int mutex_flags = shared ? SQLITE_OPEN_FULLMUTEX : SQLITE_OPEN_NOMUTEX;
    int open_flags = SQLITE_OPEN_READWRITE;
    sqlite3 *db;
    const char *filename = path.c_str();

    // attempt to create new database
    if (path.empty() || !std::filesystem::exists(path))
    {
        open_flags |= SQLITE_OPEN_CREATE;
    }

    if (path.empty())
    {
        filename = ":memory:";
    }

    if (sqlite3_open_v2(filename, &db, open_flags, nullptr) != SQLITE_OK)
    {
        throw vsm::exception(VSM_ERROR_INVALID_CONTEXT);
    }
    
    return std::move(std::unique_ptr<sqlite3, decltype(&sqlite3_close)>(db, sqlite3_close));
}

void vsm::repository::init_db(std::unique_ptr<sqlite3, decltype(&sqlite3_close)> &db)
{
    char *err_msg = nullptr;
    const char *sql = "CREATE TABLE IF NOT EXISTS shaders (id INTEGER PRIMARY KEY, name TEXT NOT NULL, stage INTEGER NOT NULL, code BLOB NOT NULL);";

    if (sqlite3_exec(db.get(), sql, nullptr, nullptr, &err_msg) != SQLITE_OK)
    {
        sqlite3_free(err_msg);
        throw vsm::exception(VSM_ERROR_INVALID_CONTEXT);
    }
}