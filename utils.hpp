/*!
 * \file utils.hpp
 * \brief Various utilities for C-programming.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <cstdlib>
#include <iostream>
extern "C"
{
#include "utils.h"
}

#undef CXX_PNW_NOTE
#define CXX_PNW_NOTE(a)                         \
    { std::cout << GPFUN << ": NOTE: " << a; }
#undef CXX_PNW_WARN
#define CXX_PNW_WARN(a)                         \
    { std::cout << GPFUN << ": WARN: " << a; }
#undef CXX_PNW_WARN2
#define CXX_PNW_WARN2(a, b)                         \
    { std::cout << GPFUN << ": WARN: " << a << b; }
#undef CXX_PNW_ERR
#define CXX_PNW_ERR(a)                          \
    { std::cerr << GPFUN << ": ERROR: " << a; }
#undef CXX_GERROR2
#define CXX_GERROR2(a, b)                               \
    { std::cerr << GPFUN << ": ERROR: " << a << b; }
#undef CXX_GZERR
#define CXX_GZERR(a)                                            \
    { std::cerr << GPFUN << ": ERROR: " << a << " is zero!"; }
#undef CXX_GFATAL
#define CXX_GFATAL(a)                                       \
    { std::cerr << GPFUN << ": FATAL: " << a; abort(); }
#undef CXX_GFIXME
#define CXX_GFIXME(a)                           \
    { std::cerr << GPFUN << ": FIXME: " << a; }
#undef CXX_GCHECK
#define CXX_GCHECK(a)                           \
    { std::cerr << GPFUN << ": CHECK: " << a; }
#undef CXX_PNW_TODO
#define CXX_PNW_TODO(a)                         \
    { std::cerr << GPFUN << ": \todo " << a; }
#ifdef DEBUG_CHECK_INDEX
#define DEBUG_CHECK_RANGE(i, n)                         \
    { if (i >= n) { PERR("Index out of range!\n"); } }
#else
#define DEBUG_CHECK_RANGE(i, n)
#endif
#ifdef DEBUG_CHECK_ZERO
#define enforce_warn(a, b)             \
    if (not a)                                  \
    {                                           \
        std::cerr << GPFUN << ": " << b;        \
    }
#else
#define enforce_warn(a, b)
#endif

inline void handle_main_args_wh(int argc, char * argv[], uint & w, uint & h) {
    switch (argc) {
    case 3:
        w = atoi(argv[1]);
        h = atoi(argv[2]);
        break;
    default: {
        w = 800;
        h = 600;
        break;
    }
    }
}
