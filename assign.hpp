/*!
 * \file assign.hpp
 * \brief Syntatic sugar templates that simplify explicit assignments of
 * STL container elements.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * Currently takes 1 to 12 explicit arguments.
 */

#pragma once

#include "algorithm_x.hpp"
#include "enforce.hpp"

namespace pnw {

/// Assign 1 element.
template<class T>
inline void assign(std::vector<T> & a,
                   const T & b0)
{
    enforce_eq(a.size(), 1);
    a.resize(1);

    a[0] = b0;
}

/// Assign 2 elements.
template<class T>
inline void assign(std::vector<T> & a,
                   const T & b0,
                   const T & b1)
{
    enforce_eq(a.size(), 2);
    a.resize(2);

    a[0] = b0;
    a[1] = b1;
}

/// Assign 3 elements.
template<class T>
inline void assign(std::vector<T> & a,
                   const T & b0,
                   const T & b1,
                   const T & b2)
{
    enforce_eq(a.size(), 3);
    a.resize(3);

    a[0] = b0;
    a[1] = b1;
    a[2] = b2;
}

/// Assign 4 elements.
template<class T>
inline void assign(std::vector<T> & a,
                   const T & b0,
                   const T & b1,
                   const T & b2,
                   const T & b3)
{
    enforce_eq(a.size(), 4);
    a.resize(4);

    a[0] = b0;
    a[1] = b1;
    a[2] = b2;
    a[3] = b3;
}

/// Assign 5 elements.
template<class T>
inline void assign(std::vector<T> & a,
                   const T & b0,
                   const T & b1,
                   const T & b2,
                   const T & b3,
                   const T & b4)
{
    enforce_eq(a.size(), 5);
    a.resize(5);

    a[0] = b0;
    a[1] = b1;
    a[2] = b2;
    a[3] = b3;
    a[4] = b4;
}

/// Assign 6 elements.
template<class T>
inline void assign(std::vector<T> & a,
                   const T & b0,
                   const T & b1,
                   const T & b2,
                   const T & b3,
                   const T & b4,
                   const T & b5)
{
    enforce_eq(a.size(), 6);
    a.resize(6);

    a[0] = b0;
    a[1] = b1;
    a[2] = b2;
    a[3] = b3;
    a[4] = b4;
    a[5] = b5;
}

/// Assign 7 elements.
template<class T>
inline void assign(std::vector<T> & a,
                   const T & b0,
                   const T & b1,
                   const T & b2,
                   const T & b3,
                   const T & b4,
                   const T & b5,
                   const T & b6)
{
    enforce_eq(a.size(), 7);
    a.resize(7);

    a[0] = b0;
    a[1] = b1;
    a[2] = b2;
    a[3] = b3;
    a[4] = b4;
    a[5] = b5;
    a[6] = b6;
}

/// Assign 8 elements.
template<class T>
inline void assign(std::vector<T> & a,
                   const T & b0,
                   const T & b1,
                   const T & b2,
                   const T & b3,
                   const T & b4,
                   const T & b5,
                   const T & b6,
                   const T & b7)
{
    enforce_eq(a.size(), 8);
    a.resize(8);

    a[0] = b0;
    a[1] = b1;
    a[2] = b2;
    a[3] = b3;
    a[4] = b4;
    a[5] = b5;
    a[6] = b6;
    a[7] = b7;
}

/// Assign 9 elements.
template<class T>
inline void assign(std::vector<T> & a,
                   const T & b0,
                   const T & b1,
                   const T & b2,
                   const T & b3,
                   const T & b4,
                   const T & b5,
                   const T & b6,
                   const T & b7,
                   const T & b8)
{
    enforce_eq(a.size(), 9);
    a.resize(9);

    a[0] = b0;
    a[1] = b1;
    a[2] = b2;
    a[3] = b3;
    a[4] = b4;
    a[5] = b5;
    a[6] = b6;
    a[7] = b7;
    a[8] = b8;
}

/// Assign 10 elements.
template<class T>
inline void assign(std::vector<T> & a,
                   const T & b0,
                   const T & b1,
                   const T & b2,
                   const T & b3,
                   const T & b4,
                   const T & b5,
                   const T & b6,
                   const T & b7,
                   const T & b8,
                   const T & b9)
{
    enforce_eq(a.size(), 10);
    a.resize(10);

    a[0] = b0;
    a[1] = b1;
    a[2] = b2;
    a[3] = b3;
    a[4] = b4;
    a[5] = b5;
    a[6] = b6;
    a[7] = b7;
    a[8] = b8;
    a[9] = b9;
}

/// Assign 11 elements.
template<class T>
inline void assign(std::vector<T> & a,
                   const T & b0,
                   const T & b1,
                   const T & b2,
                   const T & b3,
                   const T & b4,
                   const T & b5,
                   const T & b6,
                   const T & b7,
                   const T & b8,
                   const T & b9,
                   const T & b10)
{
    enforce_eq(a.size(), 11);
    a.resize(11);

    a[0] = b0;
    a[1] = b1;
    a[2] = b2;
    a[3] = b3;
    a[4] = b4;
    a[5] = b5;
    a[6] = b6;
    a[7] = b7;
    a[8] = b8;
    a[9] = b9;
    a[10] = b10;
}

/// Assign 12 elements.
template<class T>
inline void assign(std::vector<T> & a,
                   const T & b0,
                   const T & b1,
                   const T & b2,
                   const T & b3,
                   const T & b4,
                   const T & b5,
                   const T & b6,
                   const T & b7,
                   const T & b8,
                   const T & b9,
                   const T & b10,
                   const T & b11)
{
    enforce_eq(a.size(), 12);
    a.resize(12);

    a[0] = b0;
    a[1] = b1;
    a[2] = b2;
    a[3] = b3;
    a[4] = b4;
    a[5] = b5;
    a[6] = b6;
    a[7] = b7;
    a[8] = b8;
    a[9] = b9;
    a[10] = b10;
    a[11] = b11;
}

}
