/*!
 * \file code_gmp.hpp
 * \brief Encode/Decode GNU MP (GMP) types.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2009-05-25 12:41
 */

#pragma once

#include "code_ctypes.hpp"

#ifdef HAVE_GMP_H
#  include <gmp.h>

/*!
 * Encode an \c mpz_t from \p a into \p os.
 */
inline void encode_mpz(std::ostream & os, const mpz_t & a)
{
    void * rop = NULL;		// no preallocated memory
    size_t count;
    int order = 1;         // 1:most sign. word first, -1:least sign. word first
    int size = 4;
    int endian = 1;           // 1:BIG_ENDIAN, -1:LITTLE_ENDIAN, 0:NATIVE_ENDIAN
    size_t nails = 0;         // number of unused MSBs in each word

    void * data = mpz_export(rop, &count, order, size, endian, nails, a);

    size_t n = count; encode(os, n);

    if (n) {
        os.write(static_cast<const char *>(data), n * size);
    }

    free(data);
}

/*!
 * Decode an \c mpz_t from \p is into \p a.
 */
inline void decode_mpz(std::istream & is, mpz_t & a)
{
    size_t n; ::decode(is, n);

    if (n) {
        size_t count = n;
        int order = 1; // 1:most sign. word first, -1:least sign. word first
        int size = 4;
        int endian = 1;  // 1:BIG_ENDIAN, -1:LITTLE_ENDIAN, 0:NATIVE_ENDIAN
        size_t nails = 0;		// number of unused MSBs in each word
        size_t len = count * size;

        void * data = malloc(len);

        is.read(reinterpret_cast<char*>(data), len);
        mpz_import(a, count, order, size, endian, nails, data);
        free(data);
    } else {
        // zero it
        mpz_set_ui(a, 0);
    }
}

#endif
