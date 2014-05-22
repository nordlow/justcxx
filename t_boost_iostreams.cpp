#include <fstream>
#include <iostream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

int main(int argc, char*argv[])
{
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] <<" <in-file>\n";
        return -1;
    }
    using namespace std;
    ifstream is(argv[1], ios_base::in | ios_base::binary);
    boost::iostreams::filtering_istreambuf in;
    in.push(boost::iostreams::gzip_decompressor());
    in.push(is);
    boost::iostreams::copy(in, cout);
}
