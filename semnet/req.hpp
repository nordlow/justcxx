/*! \file req.hpp
 * \brief Net Pattern Requirement.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "patt.hpp"
#include "../cbufn.h"

/*! Pattern Requirement (TypeCode).
 * \todo Merge and Reuse \c LOP_t.
 */
typedef enum {
    PREQ_EQ,			///< Equality.
    PREQ_NEQ,			///< Non - Equality.
}  __attribute__ ((packed)) PREQ_t;

inline const char *
PREQ_getName(PREQ_t preqT)
{
    const char * str = nullptr;
    switch (preqT) {
    case PREQ_EQ: str  = "EQ"; break;
    case PREQ_NEQ: str = "NEQ"; break;
    }
    return str;
}

class File;

/*! Pattern Specifier/Description. */
class Req
{
public:
    Req(PREQ_t preqT) { init(preqT); }
private:
    void init(PREQ_t preqT) { m_preqT = preqT; }
private:
    PREQ_t m_preqT;
};
