#ifndef FILE_CRAWLER_H
#define FILE_CRAWLER_H

#include <string>
#include <filesystem>
#include <iostream>
#include <stack>
#include <unordered_set>
#include <vector>
#include "sqlite_wrapper.h"

using string = std::string;

struct FileRecord
{
    string filename;
    string absolute_path;
    string extension;
    std::unordered_set<string> tokens;
};

class FileSystemCrawler
{
private:
    string root_path;
    std::unordered_set<string> ignored_folders_list = {
        "node_modules",
        "build"
    };

    SQLiteWrapper db_wrapper = SQLiteWrapper("/home/a7x/crawl.db");
    static constexpr short SEARCH_LIMIT = 10;


public:
    FileSystemCrawler(const string &path) : root_path(path) {}
    void initializing_crawl();
    void crawl(const string &root);
    bool is_ignorable(const string &folder_name);
    void process_files(std::vector<FileRecord> &files);
    void search(std::string &prefix);

};

#endif
