// \see http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt12ch31s04.html#parallel_mode.design.tuning
// \see http://www.baptiste-wicht.com/2012/11/integer-linear-time-sorting-algorithms/
// \see https://github.com/gfx/cpp-TimSort

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <cstdlib>
#include <random>
#include <chrono>
#ifndef __clang__
#  include <omp.h>
#  include <parallel/algorithm> // \see http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt03ch18s03.html#parallel_mode.using.parallel_mode
#  include <parallel/settings.h>
#endif
#include "sortn.h"
#include "sortn.hpp"
#include "qsort.hpp"
#include "iqsort.hpp"
#include "msort.h"
#include "qsort.h"
#include "qsort_mt.h"
#include "utils.h"
#include "meman.h"
#include "rangerand.h"
#include "rand.hpp"
#include "randperm.hpp"
#include "reorg.h"
#include "sign.h"
#include "algorithm_x.hpp"
#include "do_file.h"
#include "deps/cpp-TimSort/timsort.hpp"
#include "isort.hpp"
#include "typenameof.hpp"
#include "enforce.hpp"
#include "show_all.hpp"

using std::cout;
using std::endl;

/*! Random Fill Container of Float Point Numbers. TODO: Merge with rand. */
template<template<class E> class C, class T>
inline void fill_random(C<typename std::enable_if<std::is_integral<T>::value,T>::type>& a,
                        const T min = pnw::minof<T>(),
                        const T max = pnw::maxof<T>()) {
    std::mt19937 gen;
    std::uniform_int_distribution<T> dist(min, max);
    for (auto& i : a) { i = dist(gen); }
}
/*! Random Fill Container of Integral Numbers. TODO: Merge with rand. */
template<template<class E> class C, class T>
inline void fill_random(C<typename std::enable_if<std::is_floating_point<T>::value,T>::type>& a) {
    std::mt19937 gen;
    std::normal_distribution<T> dist;
    for (auto& i : a) { i = dist(gen); }
}

/*! Benchmark and Verify Sorting Function \p sort_function.
  *
  * \param[in] sort_function is Function to Sort
  * \param[in] numE is Element Count
  * \param[in] tRef is Reference Time Benchmark Against
  *
  * \tparam T is Element Type.
  * \todo Merge with timed.hpp
 */
template<class T, class Function>
inline std::chrono::microseconds
bench_and_verify_sorter(Function sort_function,
                        //std::function<void(std::vector<T>&)> sort_function,
                        size_t numE = 1e6,
                        int nTries = 3,
                        bool show = false,
                        bool descending = false,
                        bool permutation = false,
                        std::chrono::microseconds tRef = std::chrono::microseconds{0})
{
    using std::cout;
    using std::endl;

    std::vector<std::vector<T> > marks(nTries);

    // Randomize and its Range
    std::pair<T,T> range(pnw::maxof<T>(),
                         pnw::minof<T>());
    for (auto& v : marks) {     // for each vector
        v.resize(numE);
        if (permutation) { randperm(v); }
        else             { rand(v); }
        range = std::minmax_element_value(v);
        // range.first  = std::min(range.first, *std::min_element(begin(v), end(v)));
        // range.second = std::max(range.second, *std::max_element(begin(v), end(v)));
    }

    // Bench it!
    typedef std::chrono::high_resolution_clock C;
    std::chrono::microseconds tC{0}; // evaluation time
    for (auto& v : marks) {
        const auto v_sorted = std::sort_copy(v);
        const auto tA = C::now(); // Tic
        sort_function(v);         // Call it!
        const auto tB = C::now(); // Toc
        tC += std::chrono::duration_cast<std::chrono::microseconds>(tB - tA);
        if (descending) { std::reverse(begin(v), end(v)); }
        enforce(std::is_sorted(begin(v), end(v))); // assure sorted result
        enforce_eq(v, v_sorted); // assure same results as std::sort()
    }

    if (tRef.count()) {          // if reference time
        cout.precision(3);
        cout << static_cast<double>(tRef.count()) / tC.count() << "x"; // use it
    } else {
        cout << tC.count() / nTries<< "us";
    }

    if (show) { cout << " range: " << range; }
    cout << " ";
    return tC;
}

template<class T>
inline int test_all_sort(const size_t numE = 1e5,
                         const int nTries = 3,
                         bool show = false,
                         bool doInPlace = false,
                         uint radixBinaryPowerMin = 8,      // min radix exponent (radix = 2^r_max). todo: Why does 22 segfault?
                         uint radixBinaryPowerMax = 18)      // max radix exponent (radix = 2^radixBinaryPowerMax). todo: Why does 22 segfault?
{
    // std::vector<size_t> perm(numE);
    // randperm(perm);
    // enforce(is_continuous_permutation(perm));
    // typedef std::chrono::high_resolution_clock C;
    // std::chrono::microseconds tC{0}; // evaluation time
    // const auto tA = C::now(); // Toc
    // if (in_place) {
    //     pnw::in_place_indexed_reorder<T>(v, v2, perm);
    // } else {
    //     pnw::indexed_reorder<T>(v, v2, perm);
    // }
    // const auto tB = C::now(); // Toc
    // auto tMin = std::chrono::duration_cast<std::chrono::microseconds>(tB - tA);

    auto tRef = bench_and_verify_sorter<T>(&std_sort<T>, numE, nTries);

    const auto bitsOfT = 8*sizeof(T);
    for (uint r = std::min<uint>(radixBinaryPowerMin, bitsOfT); r <= std::min<uint>(radixBinaryPowerMax, bitsOfT) ; ++r) {
        std::cout << typenameof<T>() << " ";
        std::cout << r << " ";
        bench_and_verify_sorter<T>([r](std::vector<T>& a) { radix_sort<T,T>(a, r, false, false); }, numE, nTries, show, false, false, tRef);
        if (doInPlace and r == bitsOfT) {
            bench_and_verify_sorter<T>([r](std::vector<T>& a) { radix_sort<T,T>(a, r, false, true); }, numE, nTries, show, false, true, tRef);
        }
        bench_and_verify_sorter<T>([r](std::vector<T>& a) {              radix_sort<T,T>(a, r,  true); }, numE, nTries, show, true, false, tRef);
        bench_and_verify_sorter<T>([r](std::vector<T>& a) {     parallel_radix_sort<T,T>(a, r, false); }, numE, nTries, show, false, false, tRef);
        bench_and_verify_sorter<T>([r](std::vector<T>& a) { tbb_parallel_radix_sort<T,T>(a, r, false); }, numE, nTries, show, false, false, tRef);
    }

    // std::cout << "counting_sort: "; bench_and_verify_sorter(&counting_sort<T>, numE, nTries);
    // std::cout << "bin_sort: "; bench_and_verify_sorter(&::bin_sort, numE, nTries);
    // std::cout << "in_place_counting_sort: "; bench_and_verify_sorter(&in_place_counting_sort<T>, numE, nTries);

    endl();
    return 0;
}

template<class T>
inline int test_all_sort_and_sizes(const size_t lengthBinaryPowerMin = 10,
                                   const size_t lengthBinaryPowerMax = 20,
                                   const int nTries = 3,
                                   bool show = false,
                                   bool doInPlace = false,
                                   uint radixBinaryPowerMin = 8,      // min radix exponent (radix = 2^r_max). todo: Why does 22 segfault?
                                   uint radixBinaryPowerMax = 18)      // max radix exponent (radix = 2^radixBinaryPowerMax). todo: Why does 22 segfault?
{
    int ret = 0;
    for (size_t numE = 1 << lengthBinaryPowerMin; numE <= 1 << lengthBinaryPowerMax; numE <<= 1) {
        ret += test_all_sort<T>(numE,
                                nTries,
                                show,
                                doInPlace,
                                radixBinaryPowerMin,
                                radixBinaryPowerMax);
    }
    return ret;
}

template<typename T> void
test_array_sort(size_t numE,
                cmp_function_t * cmp,
                cmp_function_t * cmp2,
                void (*qsort_function)(T*, size_t),
                void (*qsorti_function)(T*, size_t*, size_t),
                const char * type_name)
{
    typedef std::chrono::high_resolution_clock t;// pTimer t; ptimer_init(&t, CLOCK_PROCESS_CPUTIME_ID);
    T a[numE];
    cout << "numE:" << numE << " <" << type_name << ">";
    {
        rrand_n<T>(a,numE, 0,numE);
        const auto tA = t::now(); qsort(a, numE, sizeof(T), cmp2); const auto tB = t::now();
        cout << "qsort(cmp2):" << std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count() << "ms ";
        if (not std::is_sorted(a, a+numE)) { cout << "qsort gave unsorted result!" << endl; }
    }
    {
        rrand_n<T>(a,numE, 0,numE);
        const auto tA = t::now(); qsort_mt(a, numE, sizeof(T), cmp2, 0, 0); const auto tB = t::now();
        cout << "qsort_mt(cmp2):" << std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count() << "ms ";
        if (not std::is_sorted(a, a+numE)) { cout << "qsort_mt gave unsorted result!" << endl; }
    }
    {
        rrand_n<T>(a,numE, 0,numE);
        const auto tA = t::now(); msort(a, numE, sizeof(T), cmp2); const auto tB = t::now();
        cout << "msort_mt(cmp2):" << std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count() << "ms ";
        if (not std::is_sorted(a, a+numE)) { cout << "msort gave unsorted result!" << endl; }
    }
    {
        rrand_n<T>(a,numE, 0,numE);
        const auto tA = t::now(); aqsort(a, numE, sizeof(T), cmp); const auto tB = t::now();
        cout << "aqsort_mt(cmp):" << std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count() << "ms ";
        if (not std::is_sorted(a, a+numE)) { cout << "aqsort gave unsorted result!" << endl; }
    }
    // size_t i[numE];
    // NOTE: Disabled because it crashes for numE = 1048576
    // rrand_n<T>(a,numE, 0,numE);
    // sizearray_ramp(i, numE);
    // const auto tA = t::now(); iqsort(a, i, numE, sizeof(T), cmp); const auto tB = t::now();
    // printf("iqsort(cmp):%.2f%% ", ptimer_get_sec(&t)/st);
    // if (not std::is_sorted(a, a+numE)) { printf("iqsort gave unsorted result!" << endl; }
    // if (not pnw::is_continuous_permutation(i, numE)) { printf("iqsort: i is not a permuation!" << endl; }
    // NOTE: Disabled because it crashes for numE = 1048576
    // rrand_n<T>(a,numE, 0,numE);
    // sizearray_ramp(i, numE);
    // const auto tA = t::now(); xqsort(a,i, numE, sizeof(T),sizeof(size_t), cmp); const auto tB = t::now();
    // printf("xqsort(cmp):%.2f%% ", ptimer_get_sec(&t)/st);
    // if (not std::is_sorted(a, a+numE)) { printf("xqsort gave unsorted result!" << endl; }
    // if (not pnw::is_continuous_permutation(i, numE)) { printf("xqsort: i is not a permuation!" << endl; }
    // NOTE: Disabled because it crashes for numE = 1048576
    // rrand_n<T>(a,numE, 0,numE);
    // const auto tA = t::now(); qsort_function(a, numE); const auto tB = t::now();
    // printf("<T>array_qsort:%.2f%% ", ptimer_get_sec(&t)/st);
    // if (not std::is_sorted(a, a+numE)) { printf("<T>array_qsort gave unsorted result!" << endl; }
    // NOTE: Disabled because it crashes for numE = 1048576
    // rrand_n<T>(a,numE, 0,numE);
    // sizearray_ramp(i, numE);
    // const auto tA = t::now(); qsorti_function(a, i, numE); const auto tB = t::now();
    // printf("<T>array_iqsort:%.2f%% ", ptimer_get_sec(&t)/st);
    // if (not std::is_sorted(a, a+numE)) { printf("<T>array_iqsort gave unsorted result!" << endl; }
    // if (not pnw::is_continuous_permutation(i, numE)) { printf("<T>array_iqsort: i is not a permuation!" << endl; }
    {
        rrand_n<T>(a,numE, 0,numE);
        const auto tA = t::now();
        std::sort(a, a+numE);
        const auto tB = t::now();
        cout << "std::sort: " << std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count() << "ms ";
        if (not std::is_sorted(a, a+numE)) { cout << "std::sort gave unsorted result!" << endl; }
    }
    {
        rrand_n<T>(a,numE, 0,numE);
        const auto tA = t::now();
        __gnu_parallel::sort(a, a+numE);
        const auto tB = t::now();
        cout << "__gnu_parallel::sort: " << std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count() << "ms ";
        if (not std::is_sorted(a, a+numE)) { cout << "__gnu_parallel::sort gave unsorted result!" << endl; }
    }
    {
        rrand_n<T>(a,numE, 0,numE);
        const auto tA = t::now();
        std::stable_sort(a, a+numE);
        const auto tB = t::now();
        cout << "std::stable_sort: " << std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count() << "ms ";
        if (not std::is_sorted(a, a+numE)) { cout << "std::stable_sort gave unsorted result!" << endl; }
    }
    {
        rrand_n<T>(a,numE, 0,numE);
        const auto tA = t::now();
        __gnu_parallel::stable_sort(a, a+numE);
        const auto tB = t::now();
        cout << "__gnu_parallel::stable_sort: " << std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count() << "ms ";
        if (not std::is_sorted(a, a+numE)) { cout << "__gnu_parallel::stable_sort gave unsorted result!" << endl; }
    }
    {
        rrand_n<T>(a,numE, 0,numE);
        const auto tA = t::now();
        gfx::timsort(a, a+numE);
        const auto tB = t::now();
        cout << "gfx::timsort: " << std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count() << "ms ";
        if (not std::is_sorted(a, a+numE)) { cout << "gfx::timesort gave unsorted result!" << endl; }
    }
}

int
main(int argc, char *argv[])
{
    /* size_t numE_ = 1e5; */
    /* if (argc == 1+1) { */
    /*     numE_ = atoi(argv[1]); */
    /*     cout << "Element Count: " << numE_ << endl; */
    /* } else { */
    /*     cout << "Usage: " << argv[0] << " [ELEMENT_COUNT]" << endl; */
    /*     exit(-1); */
    /* } */

    const int nTries = 3;
    const bool show = false;
    const bool doInPlace = false;
    const uint radixBinaryPowerMin = 16;      // 8, min radix exponent (radix = 2^radixBinaryPowerMax). todo: Why does 22 segfault?
    const uint radixBinaryPowerMax = 16;      // 18, max radix exponent (radix = 2^radixBinaryPowerMax). todo: Why does 22 segfault?
    const uint lengthBinaryPowerMin = 4;
    const uint lengthBinaryPowerMax = 24;

    cout << "Number of tries: " << nTries << endl;
    cout << "Do in place: " << doInPlace << endl;

    for (size_t numE = 1 << lengthBinaryPowerMin; numE <= 1 << lengthBinaryPowerMax; numE <<= 1) {
        cout << "Element Count: " << numE << endl;
        cout << "ElementType Reference(std::sort) Radix radix_sort descending_radix_sort parallel_radix_sort tbb_parallel_radix_sort" << endl;
        test_all_sort<float>   (numE, nTries, show, doInPlace, radixBinaryPowerMin, radixBinaryPowerMax);
        test_all_sort<double>  (numE, nTries, show, doInPlace, radixBinaryPowerMin, radixBinaryPowerMax);

        test_all_sort<uint8_t>(numE, nTries, show, doInPlace, radixBinaryPowerMin, radixBinaryPowerMax);
        test_all_sort< int8_t>(numE, nTries, show, doInPlace, radixBinaryPowerMin, radixBinaryPowerMax);

        test_all_sort<uint16_t>(numE, nTries, show, doInPlace, radixBinaryPowerMin, radixBinaryPowerMax);
        test_all_sort< int16_t>(numE, nTries, show, doInPlace, radixBinaryPowerMin, radixBinaryPowerMax);

        test_all_sort<uint32_t>(numE, nTries, show, doInPlace, radixBinaryPowerMin, radixBinaryPowerMax);
        test_all_sort< int32_t>(numE, nTries, show, doInPlace, radixBinaryPowerMin, radixBinaryPowerMax);

        test_all_sort<uint64_t>(numE, nTries, show, doInPlace, radixBinaryPowerMin, radixBinaryPowerMax);
        test_all_sort< int64_t>(numE, nTries, show, doInPlace, radixBinaryPowerMin, radixBinaryPowerMax);
    }


    // test_all_sort<int8_t> (numE, nTries, show, doInPlace, radixBinaryPowerMin, radixBinaryPowerMax);
    // test_all_sort<int16_t> (numE, nTries, show, doInPlace, radixBinaryPowerMin, radixBinaryPowerMax);
    // test_all_sort<int32_t> (numE, nTries, show, doInPlace, radixBinaryPowerMin, radixBinaryPowerMax);
    // test_all_sort<int64_t> (numE, nTries, show, doInPlace, radixBinaryPowerMin, radixBinaryPowerMax);

    // Explicitly set number of threads.
    // \see http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt12ch31s04.html#parallel_mode.design.tuning
    omp_set_dynamic(false); // runtime will not dynamically adjust the number of threads used
    const int threads_wanted = 2;
    omp_set_num_threads(threads_wanted);

    // Force Parallelism
    __gnu_parallel::_Settings s;
    s.algorithm_strategy = __gnu_parallel::force_parallel;
    __gnu_parallel::_Settings::set(s);

    test_ip_sort<int>(true);
    /* test_qsort_mt(argc, argv); */

    for (size_t i = 2; i <= 15; i++) {
        test_array_sort<int>(i, intp_sgn, intp_sgn2, intarray_qsort,intarray_iqsort, "int"); endl();
        // test_array_sort<float>(i, floatp_sgn, floatp_sgn2, farray_qsort,farray_iqsort, "float"); endl();
        // test_array_sort<double>(i, doublep_sgn, doublep_sgn2, darray_qsort,darray_iqsort, "double"); endl();
        // test_array_sort<ldouble>(i, ldoublep_sgn, ldoublep_sgn2, ldarray_qsort,ldarray_iqsort, "ldouble"); endl();
    }

    for (size_t i = 16; i <= 4096 * 64 * 4; i *= 2) {
        test_array_sort<int>(i, intp_sgn, intp_sgn2, intarray_qsort,intarray_iqsort, "int"); endl();
        // test_array_sort<float>(i, floatp_sgn, floatp_sgn2, farray_qsort,farray_iqsort, "float"); endl();
        // test_array_sort<double>(i, doublep_sgn, doublep_sgn2, darray_qsort,darray_iqsort, "double"); endl();
        // test_array_sort<ldouble>(i, ldoublep_sgn, ldoublep_sgn2, ldarray_qsort,ldarray_iqsort, "ldouble"); endl();
    }

    return 0;
}
