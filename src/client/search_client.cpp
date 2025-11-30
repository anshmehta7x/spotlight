#include <iostream>
#include "trie.h"
#include <iomanip>

#include "file_crawler.h"

void trie_search(TrieSearch& trie_searcher, std::string &prefix, int num_results = 10) {
    std::vector<FileInfo> search_results = trie_searcher.search_prefix_n_results(prefix, num_results);

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

int main()
{
    TrieSearch trie_searcher;
    FileSystemCrawler crawler("/home");
    trie_searcher.load("/home/a7x/trie.dat");

    std::string query;
    std::cout << "Enter search query: ";
    std::cin >> query;
    std::cout << "Searching for: '" << query << "'\n";


    std::cout<< "TRIE RESULTS "<< std::endl;
    trie_search(trie_searcher, query);
    std::cout << std::endl;
    std::cout<< "INVERTED INDEX RESULTS "<< std::endl;
    crawler.index_search(query);
    return 0;
}
