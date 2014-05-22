#include <fstream>
#include <string>
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Need a file name as first argument!" << endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    std::string line, longest;
    std::size_t longest_on = 0;
    std::size_t num_lines = 0;

    for (num_lines = 1 ; std::getline(file, line) ; ++num_lines) {
        if (line.length() > longest.length()) {
            longest.swap(line);
            longest_on = num_lines;
        }
    }

    if (longest_on) {
        cout << "File had total of " << num_lines << " lines." << endl;
        cout << "Longest line found on line " << longest_on << " with " << longest.length() << " characters." << endl;
        cout << "The line was: " << longest << endl;
    }
    else {
        std::cerr << "The file had no readable lines!" << endl;
    }
}
