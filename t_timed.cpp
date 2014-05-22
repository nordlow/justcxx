/*! \file t_timed.cpp
 * \brief Test Benchmark Logic.
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
extern "C" { extern char *gets(char *__s) { return 0; }; }
#endif

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <boost/process/search_path.hpp>
//#include <cvmlcpp/signal/Histogram>
#include "ioredirect.hpp"
#include "show_vector.hpp"
#include "enforce.hpp"
#include "algorithm_x.hpp"
#include "timed.hpp"
#include "fshow.hpp"
#include "iostream_x.hpp"
#include "ostream_x.hpp"

namespace bp = ::boost::process;
namespace bpi = ::boost::process::initializers;

using std::cout;
using std::endl;

template<class T>
std::vector<T> test_vector()
{
    std::vector<T> x = {1, 2,2, 3,3,3, 4,4,4,4, 5,5,5,5,5};
    for (size_t i = 0; i != 256; ++i) { x.push_back(6); }
    cout << "x: " << x << endl;
    return x;
}

// bp::child start_child(std::string exec)
// {
//     std::vector<std::string> args;
//     bp::context ctx;
//     ctx.stdin_behavior = bp::inherit_stream();
//     ctx.stdout_behavior = bp::inherit_stream();
//     ctx.stderr_behavior = bp::inherit_stream();
//     return bp::launch(exec, args, ctx);
// }

template<class C>
void test_benchmark()
{
    using pnw::timed;
    using namespace fio::latex;
    using namespace fio::latex::tikz;

    // Open Temporary text file
    std::ofstream os; const auto fn = open_temporary(os);

    // Benchmark Options
    auto dur = std::chrono::milliseconds(5000);

    // Plots
    const size_t mlen = 1024*1024;    // max length
    // Picture of Plots
    Picture pic(XYPlot(timed(mlen, dur).call<C>(std::reverse)).legend("\\texttt{std::reverse}"),
                XYPlot(timed(mlen, dur).call<C>(std::random_shuffle)).legend("\\texttt{std::random_shuffle}"),
                XYPlot(timed(mlen, dur).call<C>(std::sort)).legend("\\texttt{std::sort}"),
                XYPlot(timed(mlen, dur).call<C>(std::stable_sort)).legend("\\texttt{std::stable_sort}"));
    pic.loglogaxis().log_basis_x(2).grid_major().
        xlabel("Element Count").
        ylabel("Execution Time [s]").
        title("Time-Complexity of C++ STL Algorithms").
        width("1.0\\textwidth");

    // Document
    Document doc;
    doc.add(pic);

    // Flush and close
    os << endl << doc;
    os.close();

    // Execute
    if (true) {
        const auto name = "pdflatex";
        const auto pl = bp::search_path(name);
        std::vector<std::string> args = { name, fn };
        bp::child gen = bp::execute(bpi::run_exe(pl),
                                    bpi::set_args(args),
                                    bpi::inherit_env(),
                                    bpi::start_in_dir("/tmp"),
                                    bpi::close_stdin(),
                                    bpi::close_stdout(),
                                    bpi::close_stderr());
        auto exit_code = wait_for_exit(gen);
        enforce_zero(exit_code);
        // if (exit_code == 0) {
        //     cout << pl << ": OK status: " << endl;
        // } else {
        //     cout << pl << ": FAIL" << endl;
        // }
    }

    if (true) {
        const auto name = "evince";
        const auto pl = bp::search_path(name);
        std::vector<std::string> args = { name, fn + ".pdf"};
        bp::child gen = bp::execute(bpi::run_exe(pl),
                                    bpi::set_args(args),
                                    bpi::inherit_env(),
                                    bpi::start_in_dir("/tmp"),
                                    bpi::close_stdin(),
                                    bpi::close_stdout(),
                                    bpi::close_stderr());
        auto exit_code = wait_for_exit(gen);
        enforce_zero(exit_code);
        // if (exit_code == 0) {
        //     cout << pl << ": OK status: " << endl;
        // } else {
        //     cout << pl << ": FAIL" << endl;
        // }
    }
}

int main(int argc, const char * argv[], const char * envp[])
{
    test_benchmark< std::vector<int> >();
    return 0;
}
