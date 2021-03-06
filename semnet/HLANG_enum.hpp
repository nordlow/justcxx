/*!
 * \file HLANG_enum.hpp
 * \brief Human Language Type Code.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-09-19 13:19
 */

#pragma once


/*! \em Human Language.
 * \todo merge with \c DFMT_t
 */
typedef enum {
    HLANG_any_,
    HLANG_EN,                   ///< English.
    HLANG_SV,                   ///< Swedish.
    HLANG_C,                    ///< C
    HLANG_CXX,                  ///< C++
    HLANG_undefined_,         ///< Undefined.
} __attribute__ ((packed)) HLANG_t;
