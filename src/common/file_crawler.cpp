#include "file_crawler.h"

#include "ignored_folders.h"
#include "util.h"

namespace fs = std::filesystem;

std::unordered_set<std::string> tokenize(const std::string &str)
{
    std::unordered_set<std::string> tokens;
    std::string cur;

    auto push = [&](const std::string &s)
    {
        if (!s.empty())
            tokens.insert(s);
    };

    for (size_t i = 0; i < str.size(); i++)
    {
        char c = str[i];

        if (isspace(c) || c == '-' || c == '_' || c == '.' ||
            c == '/' || c == '\\' || c == '(' || c == ')' ||
            c == '[' || c == ']' || c == '{' || c == '}')
        {
            push(cur);
            cur.clear();
            continue;
        }

        if (isupper(c))
        {
            push(cur);
            cur.clear();
            cur.push_back(tolower(c));
            continue;
        }

        cur.push_back(tolower(c));
    }

    push(cur);
    return tokens;
}

string slice_after_last(const string &str, char delimiter)
{
    size_t pos = str.find_last_of(delimiter);
    if (pos == string::npos)
    {
        return str;
    }
    if (pos + 1 >= str.length())
    {
        return "";
    }
    return str.substr(pos + 1);
}

void FileSystemCrawler::crawl(const string &root)
{
    std::stack<fs::path> dirs;
    std::vector<FileRecord> file_batch;
    const size_t BATCH_SIZE = 1000;

    dirs.push(root);
    while (!dirs.empty())
    {
        fs::path current_dir = dirs.top();
        dirs.pop();
        for (const auto &entry : fs::directory_iterator(current_dir, fs::directory_options::skip_permission_denied))
        {
            try
            {
                if (entry.is_directory())
                {
                    string folder_name = slice_after_last(entry.path().string(), '/');
                    if (!is_ignorable(folder_name))
                    {
                        dirs.push(entry.path());
                    };
                }
                else
                {
                    string file_path = entry.path().string();
                    FileRecord rec;
                    rec.filename = slice_after_last(file_path, '/');
                    rec.absolute_path = file_path;
                    rec.extension = slice_after_last(file_path, '.');
                    rec.tokens = tokenize(file_path);
                    trie_searcher.insert(rec.filename,rec.absolute_path, rec.extension);
                    file_batch.push_back(std::move(rec));
                    if (file_batch.size() >= BATCH_SIZE)
                    {
                        process_files(file_batch);
                        file_batch.clear();
                    }
                }
            }
            catch (const fs::filesystem_error &e)
            {
                std::cerr << "Error accessing " << entry.path() << ": " << e.what() << '\n';
            }
        }
    }

    if (!file_batch.empty())
    {
        process_files(file_batch);
    }
}

bool FileSystemCrawler::is_ignorable(const string &folder_name)
{
    if (ignored_folders_list.count(folder_name) || folder_name[0] == '.')
    {
        return true;
    }
    return false;
}

void FileSystemCrawler::initializing_crawl()
{
    if (!check_sudo()) {
        std::cerr << "You are not root.\n";
    }
    crawl(root_path);
}

void FileSystemCrawler::process_files(std::vector<FileRecord> &files)
{
    db_wrapper.batch_insert_files(files);
}

std::vector<SQLiteWrapper::FileResult> FileSystemCrawler::index_search(std::string &prefix,short offset) {
    return db_wrapper.search(prefix, 10, offset);
}

TrieSearch& FileSystemCrawler::get_trie() {
    return trie_searcher;
}