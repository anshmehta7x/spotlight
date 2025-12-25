//
// Created by a7x on 23/12/2025.
//

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

void Client::indexSearch(std::string& query) {
    crawler->index_search(query);
}

void Client::trieSearch(std::string &prefix, int num_results) {
        std::vector<FileInfo> search_results = trieSearcher.search_prefix_n_results(prefix, num_results);

        if (search_results.empty()) {
            std::cout << "No results for '" << prefix << "'\n";
            return;
        }

        std::cout << "\nFound " << search_results.size() << " result(s):\n\n";

        for (size_t i = 0; i < search_results.size(); ++i) {
            const auto &result = search_results[i];
            std::cout << std::setw(2) << (i + 1) << ". "
                      << std::left << std::setw(30) << result.filename
                      << " | " << result.absolute_path << "\n";
        }
        std::cout << "\n";

}

wxIMPLEMENT_APP(Client);