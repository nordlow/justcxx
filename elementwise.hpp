/*!
 * \file elementwise.hpp
 * \brief Calculate the Minimum and Maximum for each argument Component.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \todo Extra vec.hpp algorithms here.
 */

#pragma once

#include <algorithm>

template<class Ta> Ta elementwise_min(const Ta & a, const Ta & b) {
    Ta c;
    for (size_t i = 0; i < a.size(); i++) {
        c(i) = std::min(a(i), b(i));
    }
    return c;
}

template<class Ta> Ta elementwise_max(const Ta & a, const Ta & b) {
    Ta c;
    for (size_t i = 0; i < a.size(); i++) {
        c(i) = std::max(a(i), b(i));
    }
    return c;
}
