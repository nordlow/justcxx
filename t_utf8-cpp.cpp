/*! \file t_utf8-cpp.cpp
 * \brief Test Packag UTF8-CPP.
 * \see http://utfcpp.sourceforge.net
 */

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "utf8/utf8.h"

using std::cout;
using std::endl;

inline bool is_valid_utf8_file(const char* file_name)
{
    std::ifstream ifs(file_name);
    if (not ifs)
        return false; // even better, throw here
    std::istreambuf_iterator<char> it(ifs.rdbuf());
    std::istreambuf_iterator<char> eos;
    return utf8::is_valid(it, eos);
}

inline void fix_utf8_string(std::string& str)
{
    std::string temp;
    utf8::replace_invalid(str.begin(), str.end(), back_inserter(temp));
    str = temp;
}

int main(int argc, const char * argv[], const char * envp[])
{
    if (argc != 2) {
        cout << "\nUsage: docsample filename\n";
        return 0;
    }

    const char* test_file_path = argv[1];
    // Open the test file (contains UTF-8 encoded text)
    std::ifstream fs8(test_file_path);
    if (not fs8.is_open()) {
        cout << "Could not open " << test_file_path << endl;
        return 0;
    }

    unsigned int line_count = 1;
    std::string line;
    // Play with all the lines in the file
    while (std::getline(fs8, line)) {

        // check for invalid utf-8 (for a simple yes/no check, there is also utf8::is_valid function)
        auto end_it = utf8::find_invalid(line.begin(), line.end());
        if (end_it != line.end()) {
            cout << "Invalid UTF-8 encoding detected at line " << line_count << "\n";
            cout << "This part is fine: " << std::string(line.begin(), end_it) << "\n";
        }

        // Get the line length (at least for the valid part)
        auto length = utf8::distance(line.begin(), end_it);
        cout << "Length of line " << line_count << " is " << length <<  "\n";

        // Convert it to utf-16
        std::vector<uint16_t> utf16line;
        utf8::utf8to16(line.begin(), end_it, back_inserter(utf16line));

        // And back to utf-8
        std::string utf8line;
        utf8::utf16to8(utf16line.begin(), utf16line.end(), back_inserter(utf8line));

        // Confirm that the conversion went OK:
        if (utf8line != std::string(line.begin(), end_it))
            cout << "Error in UTF-16 conversion at line: " << line_count << "\n";

        line_count++;
    }
    return 0;
}
