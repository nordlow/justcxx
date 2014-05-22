#include <fstream>
#include <list>
#include <vector>
#include <iostream>
#include <complex>
#include <boost/iterator.hpp>
#include <boost/concept_check.hpp>

#include <memory>
#include <chrono>

#include "show.hpp"
#include "show_vector.hpp"
#include "algorithm_x.hpp"
#include "assign.hpp"
#include "mean.hpp"
#include "split.hpp"
#include "norm.hpp"
#include "Timer.hpp"
#include "numeric_x.hpp"
#include "enforce.hpp"
#include "memoize.hpp"
#include "ramp.hpp"

using std::cout;
using std::cin;
using std::endl;

typedef std::vector<int> ivec;

void test_perftest()
{
    bool show = false;
    uint n = 100000;

    ivec a, b;

    Timer timer;

    for (uint i = 0; i < n; i++)
    {
        a.push_back( 0 + i);
        b.push_back(10 + i);
    }

    if (show)
    {
        cout << "a: " << a << endl;
        cout << "b: " << b << endl;
    }

    timer.restart();
    b.insert(begin(b), begin(a), end(a));
    timer.stop();
    cout << "insert took: " << timer << endl;

    if (show)
    {
        cout << "b: " << b << endl;
    }

    uint prev_size = b.size();
    b.resize(a.size() + b.size());

    timer.restart();
    copy(begin(a), end(a), begin(b) + prev_size);
    timer.stop();
    cout << "copy took: " << timer << endl;

    if (show)
    {
        cout << "b: " << b << endl;
    }
}

void test_general()
{
    std::list<int> il;
    il.push_back(13);

    std::vector<int> iv;

    iv.push_back(3);
    iv.push_back(6);
    iv.push_back(2);
    iv.push_back(9);

    enforce_eq(end(iv) - begin(iv),
              4 * sizeof(int));

    cout << iv << endl << endl;
    cout << "sorting..." << endl << endl;
    std::sort (begin(iv), end(iv));
    cout << iv << endl << endl;

    std::sort(iv);
    iv = std::sort_copy(iv);

    //cout << pnw::accumulate(iv, (int)0) << endl;

    for (int i = 0; i < 10; i++)
    {
        auto hit = std::find(begin(iv), end(iv), i);
        if (hit == end(iv))
        {
            cout << i << " not found" << endl << endl;
        }
        else
        {
            cout << i << " was found and erased" << endl;
            iv.erase(hit);
            cout << "iv is not " << iv << endl << endl;
        }
    }

    std::vector<bool> bv;
    cout << "Tjoho!" << endl;
}

void test_range_split()
{
    uint n, si, sn;
    cout << "n:"; cin >> n;
    cout << "si:"; cin >> si;
    cout << "sn:"; cin >> sn;
    std::vector<uint> a(n), b, c;
    ramp(a, 0, 1);
    cout << "a:" << a << endl << endl;
    pnw::range_split(a, b, c, si, sn);
    cout << "b:" << b << endl;
    cout << "c:" << c << endl;
}

void test_variadics()
{
    cout
        << "pnw::min(1,2,3,4):" << pnw::min(1,2,3,4) << endl
        << "pnw::common_type_min(1, 2.2f, 3.3d):" << pnw::common_type_min(1, 2.2f, 3.3d) << endl

        << "pnw::max(1,2,3,4):" << pnw::max(1,2,3,4) << endl
        << "pnw::common_type_max(1, 2.2f, 3.3d):" << pnw::common_type_max(1, 2.2f, 3.3d) << endl

        << "pnw::sum(1,2,3,4):" << pnw::sum(1,2,3,4) << endl
        << "pnw::common_type_sum(1, 2.2f, 3.3d):" << pnw::common_type_sum(1, 2.2f, 3.3d) << endl

        << "pnw::mean(1,2,3,4):" << pnw::mean(1,2,3,4) << endl
        //<< "pnw::common_type_mean(1, 2.2f, 3.3d):" << pnw::common_type_mean(1, 2.2f, 3.3d) << endl
        ;
}

// template<class T>
// void my_do_sort(std::vector<T>& v)
// {
//     BOOST_CONCEPT_ASSERT((boost::RandomAccessContainer<T>));
//     std::stable_sort(begin(v),end(v));
// }

void test_boost_concept_check()
{
    //typedef std::complex<double> T;
    // typedef double T;
    // std::vector<T> v;
    // my_do_sort(v);
}

void test_norm()
{
    cout
        << pnw::norm1(1, 2.2) << endl
        << pnw::norm2(1, 2.2) << endl
        ;
}

void test_unique_ptr()
{
    std::unique_ptr<int> x(new int[1]);
    std::unique_ptr<int> y = std::move(x);
    std::unique_ptr<int> z = std::move(std::unique_ptr<int>(new int[1]));
    enforce(not x.get());
    enforce(y.get());
    enforce(z.get());
    cout << "x:" << x.get() << endl;
    cout << "y:" << y.get() << endl;
    cout << "z:" << z.get() << endl;
}

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <list>
#include <iomanip>
#include <algorithm>

// \see https://secure.wikimedia.org/wikipedia/en/wiki/C%2B%2B11
void test_unicode_literals()
{
    cout << "This is a Unicode Character: " << u8"\u2018." << endl;
    cout << "This is a bigger Unicode Character: " << u"\u2018." << endl;
    cout << "This is a Unicode Character: "  << U"\u2018." << endl;
}

void test_builtin_math()
{
    cout << std::min(1.1f, 2.2f) << endl;
    cout << std::min(1.1d, 2.2d) << endl;
}

int main(int argc, char *argv[])
{
    test_builtin_math();

    if (int x = 13) { enforce_eq(x, 13); cout << "x:" << x << endl; }
    if (int y = 0) { cout << y << endl; }

    dshowln(sizeof(boost::shared_ptr<int>));
    dshowln(sizeof(boost::weak_ptr<int>));
    dshowln(sizeof(boost::intrusive_ptr<int>));
    endl();
    dshowln(sizeof(std::shared_ptr<int>));
    dshowln(sizeof(std::weak_ptr<int>));
    dshowln(sizeof(std::unique_ptr<int>));
    endl();
    dshowln(sizeof(std::string));
    dshowln(sizeof(std::wstring));
    dshowln(sizeof(std::fstream));
    dshowln(sizeof(std::ifstream));
    dshowln(sizeof(std::ofstream));
    endl();

    test_unicode_literals();
    test_unique_ptr();
    test_norm();
    test_variadics();
    test_boost_concept_check();

    while (true) { test_range_split(); }
}
