/*! \file polysolve.hpp
 * \brief Solving polynomial equations
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-01-06 19:01
 */

#pragma once

/*  #define EQN_EPS (1e-9) */

template<class T> inline bool is_zero(T x) { return x == 0; }		/* Instead of: x > -EQN_EPS && x < EQN_EPS; */

/*! \name Solving Polynomial Equations.
 * Solving 2nd, 3rd, and 4th degree equations (Graphics Gems p. 404).
 * c[4]*x^4 + c[3]*x^3 + c[2]*x^2 + c[1]*x + c[0] = 0
 * s[0], s[1], ... are the solutions found
 */
/* @{ */
template<class T> inline int polysolve2nd(T c[3], T s[2])
{
    T p, q, D, sqrt_D;
    /* Bring equation to normal form: x^2 + px + q = 0 */
    p = c[1] / (2 * c[2]);
    q = c[0] / c[2];
    D = p * p - q;		/* Discriminant. */
    if (is_zero(D)) {
        s[0] = -p;
        return 1;
    } else if (D < 0) {
        return 0;
    } else {
        sqrt_D = sqrt(D);
        s[0] = -p + sqrt_D;
        s[1] = -p - sqrt_D;
        return 2;
    }
}

template<class T> inline void polysolve3rd(T a, T b, T c, T d,
                                           int *nsolutions, T *x)
{
    T a1 = b / a, a2 = c / a, a3 = d / a;
    T Q = (a1 * a1 - 3.0 * a2) / 9.0;
    T R = (2.0 * a1 * a1 * a1 - 9.0 * a1 * a2 + 27.0 * a3) / 54.0;
    T R2_Q3 = R * R - Q * Q * Q;
    T theta;
    if (R2_Q3 <= 0) {
        *nsolutions = 3;
        theta = acos(R / sqrt(Q * Q * Q));
        x[0] = -2.0 * sqrt(Q) * cos(theta / 3.0) - a1 / 3.0;
        x[1] = -2.0 * sqrt(Q) * cos((theta + 2.0 * M_PI) / 3.0) - a1 / 3.0;
        x[2] = -2.0 * sqrt(Q) * cos((theta + 4.0 * M_PI) / 3.0) - a1 / 3.0;
    } else {
        *nsolutions = 1;
        x[0] = pow(sqrt(R2_Q3) + fabs(R), 1 / 3.0);
        x[0] += Q / x[0];
        x[0] *= (R < 0.0) ? 1 : -1;
        x[0] -= a1 / 3.0;
    }
}
/* @} */
