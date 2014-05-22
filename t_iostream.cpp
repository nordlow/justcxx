/*!
 * \file t_iostream.cpp
 * \breif Test iostreams.
 * \see http://stackoverflow.com/questions/7889032/utf-8-compliant-iostreams
 */

#include "algorithm_x.hpp"
#include "show.hpp"

#include <sstream>              // stringstream, ostringstream, istringstream
#include <iostream>
#include <fstream>
#include <boost/progress.hpp>
#include <climits>

using boost::timer;
using boost::progress_timer;
using boost::progress_display;
using std::cout;
using std::endl;

void test_sstream()
{
    std::ostringstream ostr;
    ostr << 12 << ' ' << 13;
    cout << ostr.str() << endl;
}

size_t n = 1024*1024;

void test_fstream()
{
    std::fstream str("t_iostream.dat");

    str.put('X');
    str.seekp(0);

    timer timer;

    timer.restart();
    for (size_t i = 0; i < n; ++i) { str.seekp(0); }
    cout << "stream::seek " << timer.elapsed() << " seconds\n";

    timer.restart();
    for (size_t i = 0; i < n; ++i) { str.peek(); }
    cout << "stream::peek " << timer.elapsed() << " seconds\n";

    timer.restart();
    for (size_t i = 0; i < n; ++i) { str.seekp(0); str.get(); }
    cout << "stream::seek and stream::get " << timer.elapsed() << " seconds\n";

    str.close();
}

void test_cstream()
{
    FILE * f = fopen("t_iostream.dat", "w+");

    timer timer;

    fputc('X', f);

    timer.restart();
    for (size_t i = 0; i < n; ++i) { fseek(f, 0, SEEK_SET); }
    cout << "fseek " << timer.elapsed() << " seconds\n";

    timer.restart();
    for (size_t i = 0; i < n; ++i) { ungetc(fgetc(f), f); }
    cout << "ungetc and fgetc " << timer.elapsed() << " seconds\n";

    timer.restart();
    for (size_t i = 0; i < n; ++i) { fseek(f, 0, SEEK_SET); fgetc(f); }
    cout << "fseek and fgetc " << timer.elapsed() << " seconds\n";

    fclose(f);
}

int main(int argc, char *argv[])
{
    test_sstream();
    test_fstream();
    test_cstream();
    return 0;
}
