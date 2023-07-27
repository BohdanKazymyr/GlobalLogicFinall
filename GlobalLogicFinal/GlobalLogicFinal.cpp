

/*Unfortunately, I encountered an error while searching for files on the C drive,
but the files on the D drive are being searched correctly.The error is related to
the access permissions to the folders on the C drive.I tried checking file access
and skipping them, as well as using try - catch, but it didn't help).*/


#include <string>
#include "../StaticLib1/StaticLib1.cpp"

int main() {
    std::cout << "Enter name of file to find." << std::endl;
    std::string search_path = "D:\\";
    std::string search_query;
    std::cin >> search_query;
    try {
        std::vector<std::string> found_files = search_files(search_path, search_query);
        if (found_files.empty()) {
            std::cout << "File not founded." << std::endl;
        }
    }
    catch (const std::exception& ex) {
        // Catch and ignore the exception for files or directories with no access permission.
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    }

    return 0;
}