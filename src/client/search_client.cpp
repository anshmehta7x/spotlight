#include <iostream>
#include "file_crawler.h"
using namespace std;

int main()
{
    FileSystemCrawler crawler("/home");

    std::string query;
    std::cout << "Enter search query: ";
    std::cin >> query;
    std::cout << "Searching for: '" << query << "'\n";

    crawler.index_search(query);
    std::cout << "-----------------------" << "\n";
    crawler.trie_search(query);
    return 0;
}
