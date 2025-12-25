#ifndef SPOTLIGHT_CLIENT_H
#define SPOTLIGHT_CLIENT_H

#include <wx/wx.h>
#include "file_crawler.h"
#include "trie.h"

class Client : public wxApp {
private:
    TrieSearch trieSearcher;
    FileSystemCrawler* crawler;

public:
    virtual bool OnInit() override;
    std::vector<SQLiteWrapper::FileResult> indexSearch(std::string &query);
    std::vector<FileInfo> trieSearch(std::string &prefix, int num_results=10);
};

#endif