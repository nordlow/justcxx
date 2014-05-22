/*! \file sign.hpp
 * \brief Calculations of Different Kinds of Signs.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2008-12-03 13:27
 */

#pragma once

namespace pnw
{

/*! \name Signs
 * Different kinds of signs.
 */
/// \{
/*! Integer Sign of \p a.
 * Return either -1, 0, or 1 depending on whether \p a is less than,
 * equal to or larger than zero correspondingly. Same as Java's \c BigInteger.signum().
 * \keywords signum, sign
 */
template<class T>
inline pure int sgn(const T& a)
{
    if (a  < 0) return -1;
    if (a == 0) return  0;
    if (a  > 0) return +1;
    return 0;
}
/*! \em Compare Sign.
 * Return either -1, 0, or 1 depending on whether \p a is less than,
 * equal to or larger than \p b correspondingly.
 * \keywords compare, signum, sign
 */
template<class T>
inline pure int cmpsgn(const T& a, const T& b)
{
    if (a  < b) return -1;
    if (a == b) return  0;
    if (a  > b) return +1;
    return 0;
}
/*! \em Binary Sign, that is -1 if \p a < 0, 0 otherwise.
 * \keywords binary, signum, sign
 */
template<class T>
inline pure T bsgn(const T& a)
{
    if (a < 0) { return -1; }
    else       { return  0; }
}
/// \}

/*! Check if \p a and \p b have the same signs. */
/// \{
/*! Return true iff \p a and \p b have \em same signs.
 * \see http://graphics.stanford.edu/~seander/bithacks.html#DetectOppositeSigns
 */
template<class T>
inline bool same_signs(const T & a, const T & b) { return (a * b) >= 0; }
inline bool same_signs(char a, char b) { return (a ^ b) >= 0; }
inline bool same_signs(short a, short b) { return (a ^ b) >= 0; }
inline bool same_signs(int a, int b) { return (a ^ b) >= 0; }
inline bool same_signs(long a, long b) { return (a ^ b) >= 0; }
inline bool same_signs(long long a, long long b) { return (a ^ b) >= 0; }
/// \}

/*! Return true iff \p a and \p b have \em opposite signs.
 * \see http://graphics.stanford.edu/~seander/bithacks.html#DetectOppositeSigns
 */
/// \{
template<class T> inline bool opposite_signs(const T & a, const T & b) { return not same_signs(a, b); }
/// \}

}
