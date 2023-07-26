#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <filesystem>
#include <mutex>
#include <atomic>

namespace fs = std::filesystem;

std::mutex printMutex;
std::atomic<bool> stopSearching(false);


void searchFile(const std::string& fileName, const fs::path& directory) {
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (stopSearching) {
            return;
        }
        if (fs::is_directory(entry)) {
            std::thread([](const std::string& fName, const fs::path& dir) {
                searchFile(fName, dir);
                }, fileName, entry.path()).detach();
        }
        else if (fs::is_regular_file(entry) && entry.path().filename() == fileName) {
            std::lock_guard<std::mutex> lock(printMutex);
            std::cout << "Found file: " << entry.path() << std::endl;
            stopSearching = true;
            return;
        }
    }
}


void initiateSearch(const std::string& fileNameToFind, const fs::path& rootDirectory) {

    std::vector<std::thread> threads;
    for (const auto& entry : fs::directory_iterator(rootDirectory)) {
        if (threads.size() >= 8) {
            break;
        }
        if (fs::is_directory(entry)) {
            threads.emplace_back([](const std::string& fName, const fs::path& dir) {
                searchFile(fName, dir);
                }, fileNameToFind, entry.path());
        }
    }


    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}