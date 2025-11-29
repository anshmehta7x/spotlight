#ifndef FILE_CRAWLER_H
#define FILE_CRAWLER_H

#include <string>
#include <filesystem>
#include <iostream>
#include <stack>
#include <unordered_set>

using string = std::string;

class FileSystemCrawler {
private:
    string root_path;
    std::unordered_set<string> ignored_folders_list = {
        "node_modules",
        "build"// add all skippable folders here....
    };

public:
    FileSystemCrawler(const string& path) : root_path(path) {}
    void crawl();
    bool is_ignorable(const string& folder_name);


};

#endif
