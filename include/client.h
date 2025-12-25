//
// Created by a7x on 23/12/2025.
//

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
    void indexSearch(std::string &query);
    void trieSearch(std::string &prefix, int num_results=10);
};

#endif //SPOTLIGHT_CLIENT_H