/*!
 * \file substr_match.hpp
 * \brief Sub-String Matching Functions.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

/* ---------------------------- Group Separator ---------------------------- */

/*! Check if \p dat \em begins with \p beg of length \p nE. */
template<class T>
inline pure bool
begins(const T & dat, const char * beg, size_t nE)
{
    return (dat.size() >= nE and dat.compare(0, nE, beg));
}

// /*! Check if \p dat \em begins with \p beg... . */
// template<class Ta, class... Tb>
// inline pure bool
// begins(const Ta & dat, Tb... beg)
// {
//     Tb begA[sizeof...(beg)] = { beg... };
//     return begins(dat, begA, sizeof...(beg));
// }

/*! Check if \p dat \em begins with \p beg. */
template<class T>
inline pure bool
begins(const T & dat, char beg)
{
    return (dat.size() >= 1 and *(begin(dat)) == beg);
}

/*! Check if \p dat \em begins with \p beg. */
template<class Ta, class Tb>
inline pure bool
begins(const Ta & dat, Tb begA, Tb begB)
{
    return (dat.size() >= 1 and
            dat[0] == begA and
            dat[1] == begB);
}

/*! Check if \p dat \em begins with \p beg. */
template<class Ta, class Tb>
inline pure bool
begins(const Ta & dat, Tb begA, Tb begB, Tb begC)
{
    return (dat.size() >= 1 and
            dat[0] == begA and
            dat[1] == begB and
            dat[2] == begC);
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Check if \p dat \em ends with \p end of length \p nE. */
template<class T>
inline pure bool
ends(const T & dat, const char * end, size_t nE)
{
    size_t sz = dat.size();
    return (sz >= nE and dat.compare(sz - nE, nE, end));
}

/*! Check if \p dat \em ends with \p end. */
template<class T>
inline pure bool
ends(const T & dat, char end_)
{
    size_t sz = dat.size();
    return (sz >= 1 and *(dat.end()-1) == end_);
}

/* ---------------------------- Group Separator ---------------------------- */
