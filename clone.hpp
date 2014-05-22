/*!
 * \file clone.hpp
 * \brief Clone Template
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2009-06-24 15:24
 */

#pragma once

template<typename T>
inline T* clone (const T & p) {
    return new decltype(p)(p);
}

template<typename T>
inline T* clone (const T * & p) {
    if (p) {
        return new decltype(*p)(*p);
    } {
        return NULL;
    }
}
