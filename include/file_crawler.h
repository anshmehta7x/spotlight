#ifndef FILE_CRAWLER_H
#define FILE_CRAWLER_H

#include <string>
#include <filesystem>
#include <iostream>
#include <stack>
#include "ignored_folders.h"
#include <unordered_set>
#include <vector>
#include "sqlite_wrapper.h"
#include "trie.h"

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

    SQLiteWrapper db_wrapper = SQLiteWrapper("/home/a7x/crawl.db");

    TrieSearch trie_searcher = TrieSearch();
    static constexpr short SEARCH_LIMIT = 10;


public:
    FileSystemCrawler(const string &path) : root_path(path) {}
    void initializing_crawl();
    void crawl(const string &root);
    bool is_ignorable(const string &folder_name);
    void process_files(std::vector<FileRecord> &files);
    std::vector<SQLiteWrapper::FileResult> index_search(std::string &prefix, short offset = 0);
    TrieSearch& get_trie();

};

#endif