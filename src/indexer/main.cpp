#include <iostream>
#include "file_crawler.h"
using namespace std;

int main(){
    FileSystemCrawler crawler("/home");
    crawler.crawl();
    return 0;
}
