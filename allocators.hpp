/*! \file allocators.hpp
 * \brief Custom Allocators.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2009-01-12 16:42
 * \see http://ompf.org/forum/viewtopic.php?f=11&t=686
 * \see http://msdn.microsoft.com/en-us/library/8z34s9c6%28v=vs.80%29.aspx
 */

#pragma once
#include <cstdlib>              // \c size_t
#if defined (__WIN32__) && ! defined (_POSIX_VERSION) // Windows
#  include <malloc.h>           // \c memalign()
#elif defined (__GNUC__)        // GNU
#  include <malloc.h>           // \c memalign()
#else                           // Rest
#endif

/*! Allocator with Specific \em Alignment. */
template<class _Tp, std::size_t N = 16>
class AlignmentAllocator
{
public:
    typedef _Tp value_type;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef _Tp * pointer;
    typedef const _Tp * const_pointer;
    typedef _Tp & reference;
    typedef const _Tp & const_reference;
public:
    inline AlignmentAllocator () throw () { }

    template<class T2>
    inline AlignmentAllocator (const AlignmentAllocator<T2, N> &) throw () { }
    inline ~AlignmentAllocator () throw () { }

    inline pointer adress (reference r) { return &r; }
    inline const_pointer adress (const_reference r) const { return &r; }

    inline pointer allocate (size_type n)
    {
#if defined (__WIN32__) && ! defined (_POSIX_VERSION) // Windows
        return (pointer)_aligned_malloc(N, n*sizeof(value_type));
        //return (pointer)memalign(N, n*sizeof(value_type));
#elif defined (__GNUC__)        // GNU
        return (pointer)memalign(N, n*sizeof(value_type));
#else  // Rest
        return (pointer)_mm_malloc (n*sizeof(value_type), N);
#endif
    }

    inline void deallocate (pointer p, size_type)
    {
#if defined (__WIN32__) && ! defined (_POSIX_VERSION) // Window
        return _aligned_free(p);
#elif defined (__GNUC__)        // GNU
        return free(p);
#else  // Rest
        _mm_free (p);
#endif
    }

    inline void construct (pointer p, const value_type & wert) { new (p) value_type (wert); }
    inline void destroy (pointer p) { p->~value_type (); }

    inline size_type max_size () const throw () { return size_type (-1) / sizeof (value_type);     }

    template<class T2>
    struct rebind { typedef AlignmentAllocator<T2, N> other; };
};
