#include "pch.h"

#include <iostream>
#include <filesystem>
#include <vector>
#include <thread>
#include <future>
#include <mutex>

namespace fs = std::filesystem;
const int numberOfStream = 8;
std::mutex printMutex;
bool has_directory_permission(const fs::path& dir) {
    if (!fs::exists(dir)) {
        return false; // File does not exist, so no permission
    }

    fs::file_status status = fs::status(dir);
    if (fs::status_known(status) && fs::is_regular_file(status) && ((status.permissions() & fs::perms::owner_read) != fs::perms::none)) {
        return true; // File has permission
    }

    return false; // File has no permission
}
std::vector<std::string> search_files_helper(const std::string& path, const std::string& query, std::atomic<bool>& stop_event, std::atomic<bool>& foundFlag) {
    std::vector<std::string> results;
    for (const auto& entry : fs::directory_iterator(path)) {
        try {
            if (stop_event) {
                return results;
            }
          
            const std::string& file = entry.path().filename().string();
            const std::string& full_path = entry.path().string();
            fs::file_status status = fs::status(full_path);


            if (fs::is_directory(entry) && !has_directory_permission(full_path)) {
                std::vector<std::future<std::vector<std::string>>> futures;
                
                for (int i = 0; i < numberOfStream; ++i) {
                    futures.push_back(std::async(std::launch::async, search_files_helper, full_path, query, std::ref(stop_event), std::ref(foundFlag)));
                }
                for (auto& future : futures) {
                    std::vector<std::string> sub_results = future.get();
                    results.insert(results.end(), sub_results.begin(), sub_results.end());
                }
            }
            else {
                
                if (file.find(query) != std::string::npos && full_path.find("D:\\") == 0 && file.substr(file.size() - 4) == ".txt") {
                    if (!foundFlag.exchange(true)) {
                        std::lock_guard<std::mutex> lock(printMutex);
                        std::cout << "Found file: " << full_path << std::endl;
                        stop_event = true;
                    }
                    results.push_back(full_path);
                }
            }
        }
        catch (const std::exception& ex) {
           
            std::cerr << "Exception caught: " << ex.what() << std::endl;
        }
    }
    return results;
}


std::vector<std::string> search_files(const std::string& path, const std::string& query) {
    std::atomic<bool> stop_event(false);
    std::atomic<bool> foundFlag(false);
    return search_files_helper(path, query, stop_event, foundFlag);
}