#include "file_crawler.h"

namespace fs = std::filesystem;

string slice_after_last(const string& str, char delimiter) {
    size_t pos = str.find_last_of(delimiter);
    if (pos == string::npos) {
        return str;
    }
    if (pos + 1 >= str.length()) {
        return "";
    }
    return str.substr(pos + 1);
}


void FileSystemCrawler::crawl() {
    std::stack<fs::path> dirs;
    dirs.push(root_path);
    while(!dirs.empty()){
        fs::path current_dir = dirs.top();
        dirs.pop();
        for (const auto& entry : fs::directory_iterator(current_dir, fs::directory_options::skip_permission_denied)) {
            try {
                if (entry.is_directory()) {
                    string folder_name = slice_after_last(entry.path().string(), '/');
                    std::cout << "Folder: " << folder_name << '\n';
                    if(!is_ignorable(folder_name)){
                        dirs.push(entry.path());
                    };
                } else {
                    std::cout << "File: " << slice_after_last(entry.path().string(), '/') << '\n';
                }
            } catch (const fs::filesystem_error& e) {
                std::cerr << "Error accessing " << entry.path() << ": " << e.what() << '\n';
            }
        }
    }
}

bool FileSystemCrawler::is_ignorable(const string& folder_name) {
    if(ignored_folders_list.count(folder_name) || folder_name[0] == '.'){
        return true;
    }
    return false;
}
