#include "gzstream.hpp"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <iomanip>

using std::cout;
using std::endl;

// GZip, Encode, Pack
int test_ogzstream(int argc, char*argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] <<" <in-file> <out-file>\n";
        return -1;
    }

    std::ifstream is(argv[1]);
    if (not is.good()) { std::cerr << "error: opening file `" << argv[1] << "' failed.\n"; return EXIT_FAILURE; }

    // now use the shorter way with the constructor to open the same file
    ogzstream os(argv[2]);
    if (not os.good()) { std::cerr << "ERROR: Opening file `" << argv[2] << "' failed.\n"; return EXIT_FAILURE; }

    char c; while (is.get(c)) os << c;

    is.close();
    os.close();

    if (not is.eof()) { std::cerr << "error: reading file `" << argv[1] << "' failed.\n"; return EXIT_FAILURE; }
    if (not os.good()) { std::cerr << "error: writing file `" << argv[2] << "' failed.\n"; return EXIT_FAILURE; }

    return EXIT_SUCCESS;
}

// gunzip, decode, unpack
int test_igzstream(int argc, char*argv[])
{
    if (argc != 3) {
        std::cerr << "usage: " << argv[0] <<" <in-file> <out-file>\n";
        return -1;
    }

    // now use the shorter way with the constructor to open the same file
    igzstream is(argv[1]);
    if (not is.good()) { std::cerr << "ERROR: Opening file `" << argv[1] << "' failed.\n"; return EXIT_FAILURE; }

    std::ofstream os(argv[2]);
    if (not os.good()) { std::cerr << "ERROR: Opening file `" << argv[2] << "' failed.\n"; return EXIT_FAILURE; }

    char c; while (is.get(c)) os << c;

    is.close();
    os.close();

    if (not is.eof()) { std::cerr << "ERROR: Reading file `" << argv[1] << "' failed.\n"; return EXIT_FAILURE; }
    if (not os.good()) {  std::cerr << "ERROR: Writing file `" << argv[2] << "' failed.\n"; return EXIT_FAILURE; }

    return EXIT_SUCCESS;
}

// Unpack line-wise
int test_igzstream_line(int argc, char*argv[])
{
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] <<" <in-file>\n";
        return -1;
    }

    // now use the shorter way with the constructor to open the same file
    igzstream is(argv[1]);
    if (not is.good()) { std::cerr << "ERROR: Opening file `" << argv[1] << "' failed.\n"; return EXIT_FAILURE; }

    char s[2048];
    size_t i = 0;
    while (not is.eof()) {
        is.getline (s, sizeof(s), '\n' );
        //cout << is.gcount() << endl;
        i++;
    }

    is.close();

    return EXIT_SUCCESS;
}

int main(int argc, char*argv[])
{
    test_igzstream_line(argc, argv);
}
