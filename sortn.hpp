/*! \file sortn.hpp
 * \brief Sort \em fixed number of explicit arguments.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \todo Integrate with C++ library in libstdc++ and libc++ (libcxx) std::sort()
 */

#pragma once
#include <utility>
#include "permn.hpp"

/*! \em Assign-Sort-4 \p a, \p b, \p c and \p d into \p k, \p l, \p m and \p n .
 * \complexity[time] (\em Small!): 5 CMP, 4 MOV
 * \complexity[code] (\em Large!): 23 CMP, 24*4 MOV
 * \note Uses value-semantic on input arguments to enable move-semantics.
 */
template<class T> void asort4(T a, T b, T c, T d,
                              T& k, T& l, T& m, T& n) {
    if (c < d)
        if (b < d)
            if (b < c)
                if (a < c)
                    if (a < b)
                    { k=a; l=b; m=c; n=d; }
                    else
                    { k=b; l=a; m=c; n=d; }
                else
                    if (a < d)
                    { k=b; l=c; m=a; n=d; }
                    else
                    { k=b; l=c; m=d; n=a; }
            else
                if (a < b)
                    if (a < c)
                    { k=a; l=c; m=b; n=d; }
                    else
                    { k=c; l=a; m=b; n=d; }
                else
                    if (a < d)
                    { k=c; l=b; m=a; n=d; }
                    else
                    { k=c; l=b; m=d; n=a; }
        else
            if (a < d)
                if (a < c)
                { k=a; l=c; m=d; n=b; }
                else
                { k=c; l=a; m=d; n=b; }
            else
                if (a < b)
                { k=c; l=d; m=a; n=b; }
                else
                { k=c; l=d; m=b; n=a; }
    else
        if (b < c)
            if (b < d)
                if (a < d)
                    if (a < b)
                    { k=a; l=b; m=d; n=c; }
                    else
                    { k=b; l=a; m=d; n=c; }
                else
                    if (a < c)
                    { k=b; l=d; m=a; n=c; }
                    else
                    { k=b; l=d; m=c; n=a; }
            else
                if (a < b)
                    if (a < d)
                    { k=a; l=d; m=b; n=c; }
                    else
                    { k=d; l=a; m=b; n=c; }
                else
                    if (a < c)
                    { k=d; l=b; m=a; n=c; }
                    else
                    { k=d; l=b; m=c; n=a; }
        else
            if (a < c)
                if (a < d)
                { k=a; l=d; m=c; n=b; }
                else
                { k=d; l=a; m=c; n=b; }
            else
                if (a < b)
                { k=d; l=c; m=a; n=b; }
                else
                { k=d; l=c; m=b; n=a; }
}

/*! In-Place-Sort-2 \p a and \p b.
 * \complexity[time]: 1 CMP, [0 or 2] MOV
 * \complexity[code]: 1 CMP, 3 MOV
 */
template<class T> inline void ip_sort(T& a, T& b) {
    if (a > b) std::swap(a,b);
}
#if defined (_GLIBCXX_ARRAY) ///< If <array> was included
namespace std {
template<class T> std::array<T,2>& sort(std::array<T,2>& a) { ip_sort(a[0], a[1]); }
}
#endif

/*! In-Place-Sort-3 \p a, \p b and \p c.
 * \complexity[time]: 3 CMP
 * \complexity[code]: 1 CMP, 3 MOV
 */
template<class T> void ip_sort(T& a, T& b, T& c)
{
    if (b < c)
        if (a < c)
            if (a < b)
            { /* already sorted */ }
            else
            { std::swap(a,b); }
        else
        { perm3_231(a,b,c); }
    else
        if (a < b)
            if (a < c)
            { std::swap(b,c); }
            else
            { perm3_312(a,b,c); }
        else
        { std::swap(a,c); }
}
#if defined (_GLIBCXX_ARRAY) ///< If <array> was included
namespace std {
template<class T> std::array<T,3>& sort(std::array<T,3>& a) { ip_sort(a[0], a[1], a[2]); }
}
#endif

/*! In-Place-Sort-4 \p a, \p b, \p c and \p d.
 *        a b c d
 * TODO: FIXME: Fails for input { 1, 3, 4, 2 }
 */
template<class T> void ip_sort(T& a, T& b, T& c, T& d) {
    if (c < d)
        if (b < d)
            if (b < c)
                if (a < c)
                    if (a < b)
                    { /* already sorted */ }
                    else
                    { std::swap(a,b); }
                else
                    if (a < d)
                    { perm3_231(a,b,c); }
                    else
                    { perm4_2341(a,b,c,d); }
            else
                if (a < b)
                    if (a < c)
                    { std::swap(b,c); }
                    else
                    { perm3_312(a,b,c); }
                else
                    if (a < d)
                    { std::swap(a,c); }
                    else
                    { perm3_231(a,c,d); }
        else
            if (a < d)
                if (a < c)
                { perm3_231(b,c,d); }
                else
                { perm4_3142(a,b,c,d); }
            else
                if (a < b)
                { perm4_3412(a,b,c,d); }
                else
                { perm4_3421(a,b,c,d); }
    else
        if (b < c)
            if (b < d)
                if (a < d)
                    if (a < b)
                    { std::swap(c,d); }
                    else
                    { perm4_2143(a,b,c,d); }
                else
                    if (a < c)
                    { perm4_2413(a,b,c,d); }
                    else
                    { perm3_231(a,b,d); }
            else
                if (a < b)
                    if (a < d)
                    { perm3_312(b,c,d); }
                    else
                    { perm4_4123(a,b,c,d); }
                else
                    if (a < c)
                    { perm3_312(a,c,d); }
                    else
                    { std::swap(a,d); }
        else
            if (a < c)
                if (a < d)
                { std::swap(b,d); }
                else
                { perm3_312(a,b,d); }
            else
                if (a < b)
                { perm4_4312(a,b,c,d); }
                else
                { perm4_4321(a,b,c,d); }
}
#if defined (_GLIBCXX_ARRAY) ///< If <array> was included
namespace std {
template<class T> std::array<T,4>& sort(std::array<T,4>& a) { ip_sort(a[0], a[1], a[2], a[3]); }
}
#endif

#include <iterator>
#include "are_ordered.hpp"

/*! \em In-Place Sort the array at \p a of length \p n.
 * \return \c true if sort was possible, \c false otherwise.
 */
template<class T> bool ip_sort(T* a, size_t n) {
    bool rval = false;
    switch (n) {
    case 0:
    case 1: /* do nothing */ rval = true; break;
    case 2: ip_sort(a[0], a[1]); rval = true; break;
    case 3: ip_sort(a[0], a[1], a[2]); rval = true; break;
    case 4: ip_sort(a[0], a[1], a[2], a[3]); rval = true; break;
    }
    return rval;
}

/*! Test \em In-Place Sort for \p n number of \p T-typed elements. */
#include <iostream>
#include <algorithm>
template<class T, size_t n> int test_ip_sort(bool show = false) {
    int rval = 0;
    T x[n];
    for (size_t i = 0; i < n; i++) { x[i] = i+1; }
    do {
        if (show) { std::copy(x, x+n, std::ostream_iterator<T>(std::cout, ",")); std::cout << " => "; }
        T xc[n];
        std::copy(x, x+n, xc); // copy [x,x+n] to xc
        switch (n) {
        case 2:
            ip_sort(xc[0], xc[1]);
            rval = (pnw::are_ordered(xc[0], xc[1]) == true);
            break;
        case 3:
            ip_sort(xc[0], xc[1], xc[2]);
            rval = (pnw::are_ordered(xc[0], xc[1], xc[2]) == true);
            break;
        case 4:
            ip_sort(xc[0], xc[1], xc[2], xc[3]);
            rval = (pnw::are_ordered(xc[0], xc[1], xc[2], xc[3]) == true);
            break;
        }
        if (show) { std::copy(xc, xc+n, std::ostream_iterator<T>(std::cout, ",")); }
        if (show) { std::cout << std::endl; }
    } while (std::next_permutation(x, x+n));
    return rval;
}

/*! Check \em In-Place Sort for \p T-typed elements. */
template<class T> int test_ip_sort(bool show = false) {
    int rval = 0;
    test_ip_sort<T,2>(show);
    test_ip_sort<T,3>(show);
    test_ip_sort<T,4>(show);
    return rval;
}
