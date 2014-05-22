/*!
 * \file aligned_vector.hpp
 * \brief std::vector with specified alignment for memory.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2009-05-28 22:54
 * \see http://stackoverflow.com/questions/4965784/g-sse-memory-alignment-on-the-stack
 * \see http://stackoverflow.com/questions/3095856/preventing-unaligned-data-on-the-heap
 */

#pragma once

#include "allocators.hpp"
//#include <boost/aligned_storage.hpp>
#include <vector>

namespace pnw {

template<class T, std::size_t _Alignment = 16>
class aligned_vector : public std::vector<T, AlignmentAllocator<T, _Alignment> >
{
public:
    aligned_vector(size_t __n)
        : std::vector<T, AlignmentAllocator<T, _Alignment> >(__n) {}
    aligned_vector(size_t __n, const T& __value)
        : std::vector<T, AlignmentAllocator<T, _Alignment> >(__n, __value) {}
};

}

// typedef std::tr1::aligned_storage<sizeof(float)*4, 16>::type raw_vector;
