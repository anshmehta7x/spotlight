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

    crawler.search(query);
    return 0;
}
