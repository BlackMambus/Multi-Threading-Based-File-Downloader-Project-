#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>
#include <curl/curl.h>

std::mutex fileMutex;

struct DownloadRange {
    std::string url;
    std::string output;
    long start;
    long end;
    int id;
};

size_t writeCallback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    std::ofstream* out = static_cast<std::ofstream*>(userdata);
    out->write(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

void downloadChunk(DownloadRange range) {
    CURL* curl = curl_easy_init();
    if (curl) {
        std::ostringstream rangeHeader;
        rangeHeader << range.start << "-" << range.end;

        std::ofstream out(range.output + ".part" + std::to_string(range.id), std::ios::binary);
        curl_easy_setopt(curl, CURLOPT_URL, range.url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
        curl_easy_setopt(curl, CURLOPT_RANGE, rangeHeader.str().c_str());

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Thread " << range.id << " failed: " << curl_easy_strerror(res) << "\n";
        } else {
            std::cout << "Thread " << range.id <<




            