
#include <string>
#include "../StaticLib1/StaticLib1.cpp"

int main() {
    std::string fileNameToFind;
    std::cout << "Enter file name: ";
    std::cin >> fileNameToFind;

    fs::path rootDirectory = "C:\\";

    initiateSearch(fileNameToFind, rootDirectory);

    return 0;
}