/*! \file t_histogram.cpp
 * \brief Test Histograms.
 */

#include <iomanip>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
// #include <cvmlcpp/signal/Histogram>
#include <unordered_map>
#include "histogram.hpp"
#include "ioredirect.hpp"
#include "show_vector.hpp"
#include "show_unordered_map.hpp"
#include "show_histogram.hpp"
#include "enforce.hpp"
#include <tuple>

using std::cout;
using std::endl;

template<class V>
std::vector<V> test_vector()
{
    std::vector<V> x = {1, 2,2, 3,3,3, 4,4,4,4, 5,5,5,5,5};
    for (size_t i = 0; i != 256; ++i) { x.push_back(6); }
    //cout << "x: " << x << endl;
    return x;
}

template<class V, class C = size_t, class H = typename std::vector<C>>
void test_dense_histogram()
{
    const auto x = test_vector<V>();
    cout << pnw::histogram::dense<V,C,H>(x) << endl;
}

template<class V, class C = size_t>
void test_sparse_histogram()
{
    const auto x = test_vector<V>();
    cout << pnw::histogram::sparse<V,C>(x) << endl;
}

// template<class V, class C = size_t>
// void test_cvmlcpp_histogram()
// {
//     cvmlcpp::Histogram<V> h;
//     std::vector<V> x = {1, 2,2, 3,3,3, 4,4,4,4, 5,5,5,5,5};
//     h.insert(begin(x), end(x));
//     h += h;
// }

template<class C = size_t>
void test_histograms()
{
    test_dense_histogram<uint8_t, C, std::array<C, 256> >();
    test_dense_histogram<uint8_t, C>();

    test_dense_histogram<uint16_t, C>();
    test_sparse_histogram<uint8_t, C>();

    test_sparse_histogram<uint16_t, C>();
    // test_cvmlcpp_histogram<uint8_t, C>();
    // test_cvmlcpp_histogram<uint16_t, C>();
    // test_cvmlcpp_histogram<uint32_t, C>();
}

int main(int argc, const char * argv[], const char * envp[])
{
    test_histograms<uint8_t>();
    test_histograms<size_t>();
    return 0;
}
