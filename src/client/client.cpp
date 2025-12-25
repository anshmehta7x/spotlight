#include "client.h"
#include "window.h"

bool Client::OnInit() {
    crawler = new FileSystemCrawler("/home");
    trieSearcher.load("/home/a7x/trie.dat");

    Window* window = new Window();
    window->Show(true);
    window->searchClient = this;

    return true;
}

std::vector<SQLiteWrapper::FileResult> Client::indexSearch(std::string& query) {
    return crawler->index_search(query);
}

std::vector<FileInfo> Client::trieSearch(std::string &prefix, int num_results) {
    return trieSearcher.search_prefix_n_results(prefix, num_results);
}

wxIMPLEMENT_APP(Client);