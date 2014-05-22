/*! \file ackermann.hpp
 * \brief Ackermann's Function.
 */

#pragma once

/*! Ackermann's Function */
template<typename T>
inline pure
T ackermann(const T& a, const T& b)
{
    if (b == 0) {
        return 0;
    } else if (a == 0) {
        return 2+b;             // more general with add instead of mul
    } else if (b == 1) {
        return 2;
    } else {
        return ackermann<T>(a-1,
                            ackermann<T>(a, b-1));
    }
}
