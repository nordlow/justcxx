/*! \file clamp.hpp
 * \brief Clamping Specified Range
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2008-12-03 13:40
 */

#pragma once
#include <algorithm>

namespace pnw
{

/*! \name Clamping
 * Clamp argument to specified lower value, upper value or inside range.
 */
/// \{

/*! Get the argument \p x clamped to the \em lower limit \p l. */
template<typename T> inline spure T lower_clamp(const T& l,
                                                 const T& x) { return std::max(l, x); }

/*! Get argument \p x clamped between the \em upper limit \p h. */
template<typename T> inline spure T upper_clamp(const T& x,
                                                 const T& h) { return std::min(x, h); }

/*! Get argument \p x clamped between the \em lower limit \p l and the
 * \em higher limit \p h, that is return x truncated (inclusively) into the \em range
 * [l h].
 */
template<typename T>
inline spure T clamp(const T& l,
                     const T& x,
                     const T& h) { return upper_clamp(lower_clamp(l, x), h); }

/// \}

}
