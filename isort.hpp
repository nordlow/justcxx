/*! \file isort.hpp
 * \brief Linear Time Integer Sorting Algorithms.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-02-09 23:53
 * \see http://www.baptiste-wicht.com/2012/11/integer-linear-time-sorting-algorithms/
 */

#pragma once
#include <algorithm>
#include <vector>
#include <limits>
#include "cc_features.h"
#include "bitwise.hpp"
#include "enforce.hpp"
#include "sortn.hpp"
#include "algorithm_x.hpp"

#if HAVE_CXX11_FUTURE
#include <future>
#endif
// #include <type_traits>

// Use SSE prefetch (needs compiler support), not really a problem on non-SSE machines.
// Need http://msdn.microsoft.com/vstudio/downloads/ppack/default.asp or recent VC to use this
#define PREFETCH 0
#if PREFETCH
#  include <xmmintrin.h> // for prefetch
#  define pfoff (64)
#  define pfoff2 (128)
// #  define pf(x) _mm_prefetch(cpointer(x + i + pfoff), 0)
// #  define pf2(x) _mm_prefetch(cpointer(x + i + pfoff2), 0)
// Prefetch Read
#  define pf_r(a)  __builtin_prefetch(a+pfoff, 0)
// Prefetch Write
#  define pf_w(a)  __builtin_prefetch(a+pfoff, 1)
#else
#  define pf_r(a)
#  define pf_w(a)
#endif

/*! Standard Sort.
 */
template<class T> inline void std_sort(std::vector<T>& a) { std::sort(begin(a), end(a)); }

/*! Counting Sort.
 */
template<class T> inline void counting_sort(std::vector<T>& a, T a_max = std::numeric_limits<T>::min()) {
    if (a.size() < 2) return;
    if (pnw::is_min(a_max)) { a_max = *std::max_element(begin(a), end(a)); }
    typedef size_t S;
    std::vector<S> b(a.size());
    std::vector<S> hi(a_max); // histogram
    for (auto i : a) {
        ++hi[i];
    }
    for (S i = 1; i != a_max + 1; ++i) {
        hi[i] += hi[i - 1];
    }
    for (long i = a.size() - 1; i >= 0; --i) {
        b[hi[a[i]] - 1] = a[i];
        --hi[a[i]];
    }
    for (S i = 0; i != a.size(); ++i) {
        a[i] = b[i];
    }
}

/*! In-Place Counting Sort.
 * \param a_max must equal \em maximum value of \p a or zero meaning unknown/undefined.
 * \see http://en.wikipedia.org/wiki/In-place_algorithm
 */
template<class T> inline void in_place_counting_sort(std::vector<T>& a,
                                                     T a_max = std::numeric_limits<T>::min()) {
    if (a.size() < 2) return;
    if (pnw::is_min(a_max)) { a_max = *std::max_element(begin(a), end(a)); }
    typedef size_t S;
    std::vector<S> hi(a_max + 1); // histogram. TODO: Check that vector size fits maximum value of \p a
    for (auto e : a) {
        ++hi[e];
    }
    S curr = 0;            // current
    for (S i = 0; i != a_max; ++i) {
        for (S j = 0; j != hi[i]; ++j) {
            a[curr++] = i;
        }
    }
}

/*! Bin/Bucket Sort.
 */
template<class T> inline void bin_sort(std::vector<T>& a,
                                       T a_max = std::numeric_limits<T>::min()) {
    if (a.size() < 2) return;
    if (pnw::is_min(a_max)) { a_max = *std::max_element(begin(a), end(a)); }
    std::vector<std::vector<T> > hi(a_max + 1); // histogram
    for (auto i : a) {
        hi[i].push_back(i);
    }
    T current = 0;
    for (size_t i = 0; i != a_max; ++i) {
        for (auto item : hi[i]) {
            a[current++] = item;
        }
    }
}

/*! Shift Signed Number \p up to Unsigned before Radix Sorting.
 * \see http://en.wikipedia.org/wiki/Bijection
 * TODO: Use typename std::enable_if<std::is_signed<T>::value, T>::type
 */
// template<class T> inline spure T uize(typename std::enable_if<std::is_signed<T>::value, T>::type a) { return a + (static_cast<T>(1) << (pnw::bitsizeof<T>() - 1)); }
/*! Shift Unsigned Number \p back down to Signed after Radix Sorting.
 * \see http://en.wikipedia.org/wiki/Bijection
 * TOOD: Use typename std::enable_if<std::is_unsigned<T>::value, T>::type
 */
inline spure uint8_t  uize(uint8_t  a) { return a; } ///< Identity.
inline spure uint16_t uize(uint16_t a) { return a; } ///< Identity.
inline spure uint32_t uize(uint32_t a) { return a; } ///< Identity.
inline spure uint64_t uize(uint64_t a) { return a; } ///< Identity.
inline spure uint8_t  uize( int8_t  a) { return a + (static_cast<uint8_t> (1) << (pnw::bitsizeof<uint8_t>()  - 1)); } // "add up""
inline spure uint16_t uize( int16_t a) { return a + (static_cast<uint16_t>(1) << (pnw::bitsizeof<uint16_t>() - 1)); } // "add up""
inline spure uint32_t uize( int32_t a) { return a + (static_cast<uint32_t>(1) << (pnw::bitsizeof<uint32_t>() - 1)); } // "add up""
inline spure uint64_t uize( int64_t a) { return a + (static_cast<uint64_t>(1) << (pnw::bitsizeof<uint64_t>() - 1)); } // "add up""

/*! Map a Floating Point Number \p a Back from Radix Sorting
 * (Inverse of \c radix_flip_float()).
 * - if sign is 1 (negative), it flips the sign bit back
 * - if sign is 0 (positive), it flips all bits back
 */
// typename std::enable_if<std::is_unsigned<U>::value, U>::type
// template<class T> inline spure T unuize(T a) { return a - (static_cast<T>(1) << (pnw::bitsizeof<T>() - 1)); }
inline void unuize(uint8_t  a, uint8_t & b) { b = a; } ///< Identity.
inline void unuize(uint16_t a, uint16_t& b) { b = a; } ///< Identity.
inline void unuize(uint32_t a, uint32_t& b) { b = a; } ///< Identity.
inline void unuize(uint64_t a, uint64_t& b) { b = a; } ///< Identity.
inline void unuize(uint8_t  a,  int8_t & b) { b = a - (static_cast<uint8_t> (1) << (pnw::bitsizeof(a) - 1)); }
inline void unuize(uint16_t a,  int16_t& b) { b = a - (static_cast<uint16_t>(1) << (pnw::bitsizeof(a) - 1)); }
inline void unuize(uint32_t a,  int32_t& b) { b = a - (static_cast<uint32_t>(1) << (pnw::bitsizeof(a) - 1)); }
inline void unuize(uint64_t a,  int64_t& b) { b = a - (static_cast<uint64_t>(1) << (pnw::bitsizeof(a) - 1)); }

/*! Map Bits of Floating Point Number \p a to Unsigned Integer that can be Radix Sorted.
 * Also finds \em sign of \p a.
 * - if it's 1 (negative float), it flips all bits.
 * - if it's 0 (positive float), it flips the sign only.
 */
inline spure uint32_t  ff(uint32_t f) { return f ^ (-(int32_t) (f >> (8*sizeof(f)-1))      | 0x80000000); }
inline spure uint32_t iff(uint32_t f) { return f ^           (((f >> (8*sizeof(f)-1)) - 1) | 0x80000000); }
inline spure uint64_t  ff(uint64_t f) { return f ^ (-(int64_t) (f >> (8*sizeof(f)-1))      | 0x8000000000000000LL); }
inline spure uint64_t iff(uint64_t f) { return f ^           (((f >> (8*sizeof(f)-1)) - 1) | 0x8000000000000000LL); }
inline spure uint32_t uize(float  a)  { return ff(*(uint32_t*)(&a)); }
inline spure uint64_t uize(double a)  { return ff(*(uint64_t*)(&a)); }
inline                           void unuize(uint32_t a, float&   b) { uint32_t t = iff(a); b = *(float*)(&t); }
inline                           void unuize(uint64_t a, double&  b) { uint64_t t = iff(a); b = *(double*)(&t); }

#if 0
#include "show_binary.hpp"
#include "enforce.hpp"
template<class UInt, class Float>
inline void test_ff(Float f)
{
    using std::cout;
    using std::endl;
    cout << "f:  " << f << endl;
    UInt u = *(UInt*)(&f);
    cout << "u: "; show_binary(u); endl();
    UInt v = ff(u);
    cout << "v: "; show_binary(v); endl();
    UInt w = iff(v);
    cout << "w: "; show_binary(w); endl();
    Float ff = *(Float*)(&w);
    cout << "ff: " << ff << endl;
    enforce_eq(f, ff);
}
#endif

template<class T> inline spure auto uize(T a, bool descending = false) -> decltype(uize(a)) {
    const auto ua = uize(a);
    return descending ? std::numeric_limits<decltype(ua)>::max()-ua : ua;
}

#include "range.hpp"

/*! Identity Function used as default argument to \c radix_sort.
 */
template<class T> inline T identity(T a) { return a; }
//template<class T> inline T& identity(T& a) { return a; }

/*! Least-Significant-Digit (LSD) First Radix Sort.
 *
 * Automatically handles Nan correctly (puts them at the and given \p descending is \c false).
 * \see http://www.drdobbs.com/cpp/its-hard-to-compare-floating-point-numbe/240149806
 *
 * \param[in,out] a Pointer To Element Array to Sort.
 * \param[in] n is Element Count.
 * \param[in] digit_nbits is Number of Bits (for each Digit).
 *            2^digit_nbits * sizeof(T) should match size of L1 cache.
 * \param[in] descending is set if we should sort in (reverse) descending order
 *
 * \param[in] in_place Flags for <a href="http://en.wikipedia.org/wiki/In-place_algorithm">in-place</a> sort.
 *            Requires no extra memory but is \em unstable, that is does \em not preserve order of equal elements.
 *
 * \see http://stackoverflow.com/questions/463105/in-place-radix-sort
 * \see https://github.com/boost-vault/Sorting
 * \see http://stereopsis.com/radix.html
 * \see http://en.wikipedia.org/wiki/Radix_sort
 * \see http://erik.gorset.no/2011/04/radix-sort-is-faster-than-quicksort.html
 * \see https://github.com/gorset/radix
 * \see http://en.wikipedia.org/wiki/American_flag_sort
 *
 * \complexity[space] n * (radix + bitsizeof<T>())
 * \complexity[time]  n * bitsizeof<T>()
 *
 * TODO: Implement parallel version of calculating histograms.
 * TODO: Relax \p a to ForwardIterator.
 * TODO: Extend to support strings by treating each string char (ASCII and UTF-8/16) as a digit (radix 8/16).
 * TODO: Can we use typename std::enable_if<std::is_unsigned<T>::value, T>::type
 */
template<class T, class V>
inline void radix_sort_n(T* a, const size_t n,
                         uint digit_nbits = 16,
                         const bool descending = false,
                         bool in_place = false,
                         const std::function<V(T)> op = identity<T>,
                         T a_min = std::numeric_limits<T>::max(),
                         T a_max = std::numeric_limits<T>::min()) {
    if (ip_sort(a, n)) { return; }    // small size optimizations

    using namespace pnw;

    // TODO: Activate this: subtract min from all values and then const uint nBits = is_min(a_max) ? 8*sizeof(T) : binlog(a_max); and add it back.
    const uint nBits = bitsizeof<T>(); // Total Number of Bits needed to code each element
    digit_nbits = std::min(digit_nbits, nBits);           // Radix may be at most bit precision
    uint nDigits = nBits / digit_nbits;         // Number of \c nDigits in radix \p digit_nbits

    const uint nRemBits = nBits % digit_nbits; // number remaining bits to sort
    if (nRemBits) { nDigits++; }     // one more for remainding bits

    const auto radix = static_cast<decltype(digit_nbits)>(1) << digit_nbits;    // Bin Count
    const auto mask = radix-1;                              // radix bit mask

    typedef decltype(uize(a[0])) U; // Get Unsigned Integer Type of same precision as \tparam T.

    if (nDigits != 1) {         // if more than one bucket sort pass (BSP)
        in_place = false; // we cannot do in-place because each BSP is unstable and may ruin order from previous digit passes
    }

    const U top = std::numeric_limits<U>::max(); // alias to make code simpler
    if (in_place) {
        // Histogram Buckets Upper-Limits/Walls for values in \p a.
#if (defined HAVE_C99_VARIABLE_LENGTH_ARRAYS) && !(defined __clang__)
        range<size_t> R[radix]; // bucket ranges
#else
        std::vector<range<size_t> > R(radix); // bucket ranges
#endif
        for (uint d = 0; d != nDigits; ++d) { // for each digit-index \c d (in base \c radix) starting with least significant (LSD-first)
            const uint sh = d*digit_nbits;   // digit bit shift

            // TODO: Activate and verify that performance is unchanged.
            // auto uize_ = [descending, sh, mask](T a) { return (uize(a, descending) >> sh) & mask; }; // local shorthand

            // Reset Histogram Counters
#ifdef HAVE_C99_VARIABLE_LENGTH_ARRAYS
            std::fill_n(R, radix, 0);
#else
            R.assign(radix, 0);
#endif

            // Populate Histogram \c O for current digit
            U ors  = 0;             // digits "or-sum"
            U ands = ~ors;          // digits "and-product"
#if (defined HAVE_CXX11_LAMBDA_EXPRESSIONS) && !(defined HAVE_C99_VARIABLE_LENGTH_ARRAYS) // NOTE: GCC-4.8 doesn't allow lambda scope reference capture of Variable Length Arrays. Ask why on stackoverflow.
            std::for_each(a, a+n, [sh,mask,&O,&ors,&ands,descending,top](const T& aj) { // for each element \c aj in \p a
                    const uint i = (uize(aj, descending) >> sh) & mask; // digit (index)
                    ++R[i].high();       // increase histogram bin counter
                    ors |= i; ands &= i; // accumulate bits statistics
                });
#else
            for (size_t j = 0; j != n; ++j) { // for each element index \c j in \p a
                const uint i = (uize(a[j], descending) >> sh) & mask; // digit (index)
                ++R[i].high();       // increase histogram bin counter
                ors |= i; ands &= i; // accumulate bits statistics
            }
#endif
            if ((! ors) or (! ~ands)) { // if bits in digit[d] are either all \em zero or all \em one
                continue;               // no sorting is needed for this digit
            }

            // Bin Boundaries: Accumulate Bin Counters Array
            size_t bin_max = R[0].high();
            R[0].low() = 0;                    // first floor is always zero
            for (size_t j = 1; j != radix; ++j) { // for each successive bin counter
                bin_max = std::max(bin_max, R[j].high());
                R[j].low()  = R[j - 1].high(); // previous roof becomes current floor
                R[j].high() += R[j - 1].high(); // accumulate bin counter
            }
            // TODO: if (bin_max == 1) { std::cout << "No accumulation needed!" << std::endl; }

            /*! \em Unstable In-Place (Permutate) Reorder/Sort \p a.
              * Access \p a's elements in \em reverse to \em reuse filled caches from previous forward iteration.
              * \see \c in_place_indexed_reorder
              */
            for (int r = radix - 1; r >= 0; --r) { // for each radix digit r in reverse order (cache-friendly)
                while (R[r]) {  // as long as elements left in r:th bucket
                    const uint i0 = R[r].pop_back(); // index to first element of permutation
                    T          e0 = a[i0]; // value of first/current element of permutation
                    while (true) {
                        const int rN = (uize(e0, descending) >> sh) & mask; // next digit (index)
                        if (r == rN) // if permutation cycle closed (back to same digit)
                            break;
                        const auto ai = R[rN].pop_back(); // array index
                        std::swap(a[ai], e0); // do swap
                    }
                    a[i0] = e0;         // complete cycle
                }
            }
        }

    } else {
        // Histogram Buckets Upper-Limits/Walls for values in \p a.
#ifdef HAVE_C99_VARIABLE_LENGTH_ARRAYS
        size_t O[radix]; // most certainly fits in the stack (L1-cache) => Use C99 variable length array (VLA) when available
#else
        std::vector<size_t> O(radix);
#endif
        std::vector<T> b(n); // Non-In-Place requires temporary \p b. TODO: We could allocate these as a Variable Length Arrays (VLA) for small arrays and gain extra speed.

        for (uint d = 0; d != nDigits; ++d) { // for each digit-index \c d (in base \c radix) starting with least significant (LSD-first)
            const uint sh = d*digit_nbits;   // digit bit shift

            // TODO: Activate and verify that performance is unchanged.
            // auto uize_ = [descending, sh, mask](T a) { return (uize(a, descending) >> sh) & mask; }; // local shorthand

            // Reset Histogram Counters
#ifdef HAVE_C99_VARIABLE_LENGTH_ARRAYS
            std::fill_n(O, radix, 0);
#else
            O.assign(radix, 0);
#endif

            // Populate Histogram \c O for current digit
            U ors  = 0;             // digits "or-sum"
            U ands = ~ors;          // digits "and-product"
#if (defined HAVE_CXX11_LAMBDA_EXPRESSIONS) && !(defined HAVE_C99_VARIABLE_LENGTH_ARRAYS) // NOTE: GCC-4.8 doesn't allow lambda scope reference capture of Variable Length Arrays. Ask why on stackoverflow.
            std::for_each(a, a+n, [sh,mask,&O,&ors,&ands,descending,top](const T& aj) { // for each element \c aj in \p a
                    const uint i = (uize(aj, descending) >> sh) & mask; // digit (index)
                    ++O[i];              // increase histogram bin counter
                    ors |= i; ands &= i; // accumulate bits statistics
                });
#else
            for (size_t j = 0; j != n; ++j) { // for each element index \c j in \p a
                const uint i = (uize(a[j], descending) >> sh) & mask; // digit (index)
                ++O[i];              // increase histogram bin counter
                ors |= i; ands &= i; // accumulate bits statistics
            }
#endif
            if ((! ors) or (! ~ands)) { // if bits in digit[d] are either all \em zero or all \em one
                continue;               // no sorting is needed for this digit
            }

            // Bin Boundaries: Accumulate Bin Counters Array
            for (size_t j = 1; j != radix; ++j) { // for each successive bin counter
                O[j] += O[j - 1]; // accumulate bin counter
            }

            // Reorder. Access \p a's elements in \em reverse to \em reuse filled caches from previous forward iteration.
            // \em Stable Reorder From \p a to \c b using Normal Counting Sort (see \c counting_sort above).
            for (ssize_t j = n - 1; j >= 0; --j) { // for each element \c j in reverse order
                const uint i = (uize(a[j], descending) >> sh) & mask; // digit (index)
                b[--O[i]] = a[j]; // reorder into b
            }
            std::copy(begin(b), end(b), a);            // put them back into \p a
// #ifdef HAVE_CXX11_LAMBDA_EXPRESSIONS
//             size_t j = 0; std::for_each(a, a+n,
//                 [&b, &j, descending, top](T& aj) { unuize(descending ? top-b[j] : b[j], aj); j++; pf_r(&b[j]); pf_w(&aj); });
// #else
//             for (size_t j = 0; j != n; ++j) { // for all element indexes in \p a
//                 a[j] = b[j]; // put them back into \p a. NOTE: Old: unuize(descending ? top-b[j] : b[j], a[j]);
//                 pf_r(&b[j]); pf_w(&a[j]);
//             }
// #endif
        }
    }
}

template<class T, class V> inline void radix_sort(std::vector<T>& a,
                                                  const uint r = 16,
                                                  const bool descending = false,
                                                  const bool in_place = false,
                                                  const std::function<V(T)> op = identity<T>,
                                                  T a_min = std::numeric_limits<T>::max(),
                                                  T a_max = std::numeric_limits<T>::min()) {
    radix_sort_n(a.data(), a.size(), r, descending, in_place, op, a_min,a_max);
}

#if defined (_GLIBCXX_ARRAY) ///< If <array> was included
template<class T, size_t N, class V> inline void radix_sort(std::array<T,N>& a,
                                                            const uint r = 16,
                                                            const bool descending = false,
                                                            const bool in_place = false,
                                                            const std::function<V(T)> op = identity<T>,
                                                            T a_min = std::numeric_limits<T>::max(),
                                                            T a_max = std::numeric_limits<T>::min()) {
    radix_sort_n(a.data(), N, r, descending, in_place, op, a_min,a_max);
}
#endif

template<class T, class V>
inline void parallel_radix_sort(std::vector<T>& a,
                                const uint r = 16,
                                const bool descending = false,
                                const bool in_place = false,
                                const std::function<V(T)> op = identity<T>,
                                T a_min = std::numeric_limits<T>::max(),
                                T a_max = std::numeric_limits<T>::min()) {
#if HAVE_CXX11_FUTURE
    const auto n = a.size();
    const auto s0 = n/2;              // first size
    const auto s1 = n-n/2;            // second size
    const auto a0 = a.data();         // first part
    const auto a1 = a.data()+s0;      // second part
    const auto policy = std::launch::async;
    auto p = std::async(policy, radix_sort_n<T,V>, a0,s0, r, descending, in_place, op, a_min,a_max);
    auto q = std::async(policy, radix_sort_n<T,V>, a1,s1, r, descending, in_place, op, a_min,a_max);
    p.get();
    q.get();
    std::inplace_merge(a0, a1, a1+s1);
#else
    radix_sort(a, r, descending, in_place, op, a_min, a_max); // default non-parallel
#endif
}

/*! Intel-TBB Parallel Merge-Radix Hybrid Sort.
 * \see http://www.drdobbs.com/parallel/parallel-merge-sort/229400239
 * \see http://www.drdobbs.com/parallel/parallel-merge/229204454
 * \todo Extend to more n processors using a recursive merge sort algorithm.
 *
 * To Enable TBB variant do #include <tbb/tbb.h> previous to this declaration.
 */
template<class T, class V>
inline void tbb_parallel_radix_sort(std::vector<T>& a,
                                    const uint r = 16,
                                    const bool descending = false,
                                    const bool in_place = false,
                                    const std::function<V(T)> op = identity<T>,
                                    T a_min = std::numeric_limits<T>::max(),
                                    T a_max = std::numeric_limits<T>::min()) {
#ifdef __TBB_tbb_H              // if Intel Threading Building Blocks (TBB) has been #included
    const uint nT = 2;          // number of tasks
    const auto n = a.size();
    const auto s0 = n/nT;        // first size
    const auto s1 = n-n/nT;      // second size
    const auto a0 = a.data();    // first part
    const auto a1 = a.data()+s0; // second part
    tbb::parallel_invoke(
        [&] { radix_sort_n<T,V>(a0,s0, r, descending, in_place, op, a_min,a_max); },
        [&] { radix_sort_n<T,V>(a1,s1, r, descending, in_place, op, a_min,a_max); }
            );
    std::inplace_merge(a0, a1, a1+s1);
#else
    radix_sort(a, r, descending, in_place, op, a_min, a_max); // default to non-parallel
#endif
}
