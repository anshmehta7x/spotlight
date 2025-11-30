#include "sqlite_wrapper.h"
#include "file_crawler.h"
#include <filesystem>

namespace fs = std::filesystem;
static const std::string DEFAULT_DB_PATH = "/home/a7x/crawl.db";

SQLiteWrapper::SQLiteWrapper(const std::string &path)
{
    db_path = path.empty() ? DEFAULT_DB_PATH : path;

    if (!exists())
    {
        init_tables();
        return;
    }

    if (!check_tables())
    {
        std::error_code ec;
        fs::remove(db_path, ec);
        init_tables();
    }
}

bool SQLiteWrapper::exists() const
{
    return fs::exists(db_path);
}

bool SQLiteWrapper::check_tables() const
{
    sqlite3 *db = nullptr;
    int rc = sqlite3_open(db_path.c_str(), &db);
    if (rc != SQLITE_OK)
        return false;

    const char *sql =
        "SELECT name FROM sqlite_master "
        "WHERE type IN ('table', 'virtual') "
        "AND name IN ('index_table', 'fts_index');";

    sqlite3_stmt *stmt = nullptr;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db);
        return false;
    }

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
        count++;

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return (count == 2);
}

void SQLiteWrapper::init_tables()
{
    sqlite3 *db = nullptr;
    int rc = sqlite3_open(db_path.c_str(), &db);
    if (rc != SQLITE_OK)
        return;

    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "PRAGMA synchronous = OFF;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "PRAGMA journal_mode = MEMORY;", nullptr, nullptr, nullptr);

    const char *sql =
        "CREATE TABLE IF NOT EXISTS index_table ("
        "    fileid INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    filename TEXT NOT NULL,"
        "    absolute_path TEXT NOT NULL UNIQUE,"
        "    extension TEXT"
        ");"
        "CREATE VIRTUAL TABLE IF NOT EXISTS fts_index "
        "USING fts5(tokens, content='', tokenize='porter unicode61');";

    char *err = nullptr;
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &err);
    if (rc != SQLITE_OK)
        sqlite3_free(err);

    sqlite3_close(db);
}

sqlite3 *SQLiteWrapper::open_db() const
{
    sqlite3 *db = nullptr;
    if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK)
        return nullptr;
    return db;
}

void SQLiteWrapper::close_db(sqlite3 *db) const
{
    if (db)
        sqlite3_close(db);
}

int SQLiteWrapper::insert_file(const std::string &filename,
                               const std::string &abs_path,
                               const std::string &ext)
{
    sqlite3 *db = open_db();
    if (!db)
        return -1;

    const char *sql =
        "INSERT INTO index_table (filename, absolute_path, extension) "
        "VALUES (?, ?, ?);";

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, filename.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, abs_path.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, ext.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    int id = (rc == SQLITE_DONE) ? sqlite3_last_insert_rowid(db) : -1;
    close_db(db);
    return id;
}

bool SQLiteWrapper::file_exists(const std::string &abs_path) const
{
    return get_fileid(abs_path) != -1;
}

int SQLiteWrapper::get_fileid(const std::string &abs_path) const
{
    sqlite3 *db = open_db();
    if (!db)
        return -1;

    const char *sql =
        "SELECT fileid FROM index_table WHERE absolute_path = ? LIMIT 1;";

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, abs_path.c_str(), -1, SQLITE_TRANSIENT);

    int id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        id = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    close_db(db);
    return id;
}

bool SQLiteWrapper::insert_token(const std::string &token, int fileid)
{
    sqlite3 *db = open_db();
    if (!db)
        return false;

    const char *sql =
        "INSERT INTO fts_index (token, fileid) VALUES (?, ?);";

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, token.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, fileid);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);
    close_db(db);
    return ok;
}

void SQLiteWrapper::batch_insert_files(std::vector<FileRecord> &files)
{
    sqlite3 *db = open_db();
    if (!db)
        return;

    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

    const char *file_sql =
        "INSERT INTO index_table (filename, absolute_path, extension) "
        "VALUES (?, ?, ?);";
    const char *token_sql =
        "INSERT INTO fts_index(rowid, tokens) VALUES (?, ?);";

    sqlite3_stmt *file_stmt;
    sqlite3_stmt *token_stmt;

    sqlite3_prepare_v2(db, file_sql, -1, &file_stmt, nullptr);
    sqlite3_prepare_v2(db, token_sql, -1, &token_stmt, nullptr);

    for (auto &file : files)
    {
        sqlite3_bind_text(file_stmt, 1, file.filename.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(file_stmt, 2, file.absolute_path.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(file_stmt, 3, file.extension.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(file_stmt) == SQLITE_DONE)
        {
            int fileid = sqlite3_last_insert_rowid(db);

            // Concatenate all tokens into a single string
            std::string all_tokens;
            for (const auto &token : file.tokens)
            {
                all_tokens += token + " ";
            }

            if (!all_tokens.empty())
            {
                sqlite3_bind_int64(token_stmt, 1, fileid);
                sqlite3_bind_text(token_stmt, 2, all_tokens.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_step(token_stmt);
                sqlite3_reset(token_stmt);
                sqlite3_clear_bindings(token_stmt);
            }
        }

        sqlite3_reset(file_stmt);
        sqlite3_clear_bindings(file_stmt);
    }

    sqlite3_finalize(file_stmt);
    sqlite3_finalize(token_stmt);

    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
    close_db(db);
}

// void SQLiteWrapper::debug_print_tokens(int limit) const
// {
//     sqlite3 *db = open_db();
//     if (!db) return;
//
//     const char *sql = "SELECT token, fileid FROM fts_index LIMIT ?;";
//     sqlite3_stmt *stmt;
//
//     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
//     {
//         sqlite3_bind_int(stmt, 1, limit);
//
//         std::cout << "\n=== DEBUG: Tokens in database ===\n";
//         while (sqlite3_step(stmt) == SQLITE_ROW)
//         {
//             const char* token = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
//             int fileid = sqlite3_column_int(stmt, 1);
//             std::cout << "Token: '" << token << "' | FileID: " << fileid << "\n";
//         }
//         std::cout << "=================================\n\n";
//     }
//
//     sqlite3_finalize(stmt);
//     close_db(db);
// }
//
// void SQLiteWrapper::debug_print_files(int limit) const
// {
//     sqlite3 *db = open_db();
//     if (!db) return;
//
//     const char *sql = "SELECT fileid, filename, absolute_path FROM index_table LIMIT ?;";
//     sqlite3_stmt *stmt;
//
//     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
//     {
//         sqlite3_bind_int(stmt, 1, limit);
//
//         std::cout << "\n=== DEBUG: Files in database ===\n";
//         while (sqlite3_step(stmt) == SQLITE_ROW)
//         {
//             int fileid = sqlite3_column_int(stmt, 0);
//             const char* filename = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
//             const char* path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
//             std::cout << "ID: " << fileid << " | File: '" << filename << "' | Path: " << path << "\n";
//         }
//         std::cout << "=================================\n\n";
//     }
//
//     sqlite3_finalize(stmt);
//     close_db(db);
// }
//

std::vector<SQLiteWrapper::FileResult> SQLiteWrapper::search(const std::string &prefix, const short limit, const short offset) const
{
    sqlite3 *db = open_db();
    std::vector<FileResult> results;
    if (!db)
        return results;

    std::string search_term = prefix + "*";

    std::string q =
        "SELECT DISTINCT i.filename, i.absolute_path, i.extension "
        "FROM fts_index f "
        "JOIN index_table i ON f.rowid = i.fileid "
        "WHERE fts_index MATCH ? "
        "LIMIT ? OFFSET ?;";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, q.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "SQL Error: " << sqlite3_errmsg(db) << "\n";
        close_db(db);
        return results;
    }

    sqlite3_bind_text(stmt, 1, search_term.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, limit);
    sqlite3_bind_int(stmt, 3, offset);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        FileResult fr;
        fr.filename = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        fr.absolute_path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        fr.extension = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        results.push_back(fr);
    }

    sqlite3_finalize(stmt);
    close_db(db);
    return results;
}