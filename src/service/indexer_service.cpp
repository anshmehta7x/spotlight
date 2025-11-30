#include <mutex>
#include <condition_variable>
#include <thread>
#include "file_crawler.h"

// this will be a systemd service

std::mutex m;
std::condition_variable cv;

void log(const std::string& s) {
    std::cout << s << std::endl;
}

std::string current_datetime() {
    using namespace std::chrono;

    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);

    std::tm tm{};
    localtime_r(&t, &tm);

    std::ostringstream oss;
    oss
        << (tm.tm_year + 1900) << "-"
        << (tm.tm_mon + 1)     << "-"
        << tm.tm_mday          << " "
        << tm.tm_hour          << ":"
        << tm.tm_min           << ":"
        << tm.tm_sec;

    return oss.str();
}

void re_index(FileSystemCrawler* fs) {
    while (true) {
        log("beginning index at " + current_datetime());
        fs->initializing_crawl();
        log("created index at " + current_datetime());
        std::this_thread::sleep_for(std::chrono::minutes(5));
    }
}

int main() {
    FileSystemCrawler crawler("/home");
    std::thread t(re_index, &crawler);
    t.detach();
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock);    // waits forever

    return 0;
}
