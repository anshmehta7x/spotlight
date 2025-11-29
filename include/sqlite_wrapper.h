#ifndef SQLITE_WRAPPER_H
#define SQLITE_WRAPPER_H

#include <string>
#include <sqlite3.h>
#include <vector>
#include <unordered_set>

struct FileRecord;



class SQLiteWrapper
{
private:
    std::string db_path;

public:
    SQLiteWrapper(const std::string &path);

    struct FileResult {
        std::string filename;
        std::string absolute_path;
        std::string extension;
    };

    sqlite3 *open_db() const;
    void close_db(sqlite3 *db) const;

    bool exists() const;
    bool check_tables() const;
    void init_tables();
    int insert_file(const std::string &filename,
                    const std::string &abs_path,
                    const std::string &ext);

    bool file_exists(const std::string &abs_path) const;
    int get_fileid(const std::string &abs_path) const;

    bool insert_token(const std::string &token, int fileid);

    void batch_insert_files(std::vector<FileRecord> &files);
    void debug_print_tokens(int limit = 20) const;
    void debug_print_files(int limit = 20) const;

    std::vector<FileResult> search(const std::string &prefix,short limit) const;
};

#endif