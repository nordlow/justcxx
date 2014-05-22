/*! \file nelder_mead.hpp
 * \brief Nelder-Meade Optimization Method.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-04-09 01:33
 *
 * http://aburry.blogspot.se/2006/10/nelder-mead-downhill-simplex-method.html
 * \see http://people.sc.fsu.edu/~jburkardt/cpp_src/asa047/asa047.html
 */

#pragma once
#include "cc_features.h"
// #include <cstdlib>
// #include <ctime>
// #include <cmath>
#include <algorithm>
#include <memory>

// #undef HAVE_C99_VARIABLE_LENGTH_ARRAYS

/*! Minimizes a function \p fn using the Nelder-Meade algorithm.
 *
 *  Discussion:
 *
 *    This routine seeks the minimum value of a user-specified function.
 *
 *    Simplex function minimisation procedure due to Nelder+Mead(1965),
 *    as implemented by O'Neill(1971, Appl.Statist. 20, 338-45), with
 *    subsequent comments by Chambers+Ertel(1974, 23, 250-1), Benyon(1976,
 *    25, 97) and Hill(1978, 27, 380-2)
 *
 *    The function to be minimized must be defined by a function of
 *    the form
 *
 *      function fn (x, f)
 *      T fn
 *      T x(*)
 *
 *    and the name of this subroutine must be declared EXTERNAL in the
 *    calling routine and passed as the argument FN.
 *
 *    This routine does not include a termination test using the
 *    fitting of a quadratic surface.
 *
 *  Parameters:
 *
 * \param[in] fn, T FN (T x[]), the name of the routine which evaluates
 *    the function to be minimized.
 *
 * \param[in], size_t N, the number of variables.
 *
 * \param[in,out], T START[N].  On input, a starting point
 *    for the iteration.  On output, this data may have been overwritten.
 *
 * \param[out], T XMIN[N], the coordinates of the point which
 *    is estimated to minimize the function.
 *
 * \param[out], T YMIN, the minimum value of the function.
 *
 * \param[in], T REQMIN, the terminating limit for the variance
 *    of function values.
 *
 * \param[in], T STEP[N], determines the size and shape of the
 *    initial simplex.  The relative magnitudes of its elements should reflect
 *    the units of the variables.
 *
 * \param[in], size_t KONVGE, the convergence check is carried out
 *    every KONVGE iterations.
 *
 * \param[in], size_t KCOUNT, the maximum number of function
 *    evaluations.
 *
 * \param[out], size_t *ICOUNT, the number of function evaluations
 *    used.
 *
 * \param[out], size_t *NUMRES, the number of restarts.
 *
 * \param[out], size_t *IFAULT, error indicator.
 *    0, no errors detected.
 *    1, REQMIN, N, or KONVGE has an illegal value.
 *    2, iteration terminated because KCOUNT was exceeded without convergence.
 *
 *  Licensing:
 *    This code is distributed under the GNU LGPL license.
 *
 *  Modified:
 *    27 February 2008
 *
 *  Author:
 *    Original FORTRAN77 version by R ONeill.
 *    C++ version by John Burkardt.
 *
 *  Reference:
 *    John Nelder, Roger Mead,
 *    A simplex method for function minimization,
 *    Computer Journal,
 *    Volume 7, 1965, pages 308-313.
 *
 *    R ONeill,
 *    Algorithm AS 47:
 *    Function Minimization Using a Simplex Procedure,
 *    Applied Statistics,
 *    Volume 20, Number 3, 1971, pages 338-345.
 *
 */
template<class T>
size_t
nelder_mead_min(T fn (const T* x), size_t n,
                T start[],
                T xmin[],
                T& ymin,
                T reqmin,
                T step[],
                size_t konvge,
                size_t kcount,
                size_t& icount,
                size_t& numres,
                size_t& ifault,
                const T ccoeff = 0.5, // contraction coefficient
                const T ecoeff = 2.0, // extension coefficient
                const T rcoeff = 1.0, // reflection coefficient
                const T eps = 0.001,  // epsilon
                T del = 1.0
                )
{
    // Check the input parameters.
    if (reqmin <= 0.0) { ifault = 1; return -1; }
    if (n < 1)         { ifault = 1; return -1; }
    if (konvge < 1)    { ifault = 1; return -1; }

    // Temporaries
#ifdef HAVE_C99_VARIABLE_LENGTH_ARRAYS
    T p[n*(n+1)];
    T pstar[n];
    T p2star[n];
    T pbar[n];
    T y[n+1];
#elif HAVE_CXX11_UNIQUE_PTR
    std::unique_ptr<T[]> p(new T[n*(n+1)]);
    std::unique_ptr<T[]> pstar(new T[n]);
    std::unique_ptr<T[]> p2star(new T[n]);
    std::unique_ptr<T[]> pbar(new T[n]);
    std::unique_ptr<T[]> y(new T[n+1]);
#else
    T *p = new T[n*(n+1)];
    T *pstar = new T[n];
    T *p2star = new T[n];
    T *pbar = new T[n];
    T *y = new T[n+1];
#endif

    // Reset Counters
    icount = 0;
    numres = 0;

    size_t jcount = konvge;
    const T dn = static_cast<T>(n);
    const size_t nn = n + 1;
    const T dnn = static_cast<T>(nn);
    const T rq = reqmin * dn;

    // Initial or restarted loop.
    while (true) {

        // Construct Simplex
        for (size_t i = 0; i < n; ++i) { p[i+n*n] = start[i]; }
        y[n] = fn(start);
        ++icount;
        for (size_t j = 0; j < n; ++j) {
            const T x = start[j];
            start[j] = start[j] + step[j] * del;
            for (size_t i = 0; i < n; ++i) { p[i+j*n] = start[i]; }
            y[j] = fn(start);
            ++icount;
            start[j] = x;
        }

        // Find highest and lowest Y values.  YMIN = Y(IH) indicates
        // the vertex of the simplex to be replaced.
        T ylo = y[0];
        size_t iL = 0;
        for (size_t i = 1; i < nn; ++i) {
            if (y[i] < ylo) {
                ylo = y[i];
                iL = i;
            }
        }

        // Inner loop.
        while (true)
        {
            if (kcount <= icount) { break; }
            ymin = y[0];

            size_t iH = 0;
            for (size_t i = 1; i < nn; ++i) {
                if (ymin < y[i]) {
                    ymin = y[i];
                    iH = i;
                }
            }

            //  Calculate PBAR, the centroid of the simplex vertices
            //  excepting the vertex with Y value YMIN.
            for (size_t i = 0; i < n; ++i) {
                //T z = pnw::sum(p, p+nn);
                T z = 0;
                for (size_t j = 0; j < nn; ++j) { z += p[i+j*n]; }
                z -= p[i+iH*n]; // excepting the vertex with Y value YMIN.
                pbar[i] = z / dn;

            }

            // Reflection through the centroid.
            for (size_t i = 0; i < n; ++i) {
                pstar[i] = pbar[i] + rcoeff * (pbar[i] - p[i+iH*n]);
            }
            const T ystar = fn(pstar);
            ++icount;

            // Successful reflection, so extension.
            if (ystar < ylo) {
                for (size_t i = 0; i < n; ++i) {
                    p2star[i] = pbar[i] + ecoeff * (pstar[i] - pbar[i]);
                }
                const T y2star = fn(p2star);
                ++icount;

                // Check extension.
                if (ystar < y2star) {
                    for (size_t i = 0; i < n; ++i) {
                        p[i+iH*n] = pstar[i];
                    }
                    y[iH] = ystar;
                }

                // Retain extension or contraction.
                else {
                    for (size_t i = 0; i < n; ++i) {
                        p[i+iH*n] = p2star[i];
                    }
                    y[iH] = y2star;
                }
            }

            //  No extension.
            else {
                size_t k = 0;
                for (size_t i = 0; i < nn; ++i) {
                    if (ystar < y[i]) {
                        ++k;
                    }
                }

                if (1 < k) {
                    for (size_t i = 0; i < n; ++i){
                        p[i+iH*n] = pstar[i];
                    }
                    y[iH] = ystar;
                }

                //  Contraction on the Y(IH) side of the centroid.
                else if (k == 0) {
                    for (size_t i = 0; i < n; ++i) {
                        p2star[i] = pbar[i] + ccoeff * (p[i+iH*n] - pbar[i]);
                    }
                    const T y2star = fn(p2star);
                    ++icount;

                    // Contract the whole simplex.
                    if (y[iH] < y2star) {
                        for (size_t j = 0; j < nn; ++j) {
                            for (size_t i = 0; i < n; ++i) {
                                p[i+j*n] = (p[i+j*n] + p[i+iL*n]) * 0.5;
                                xmin[i] = p[i+j*n];
                            }
                            y[j] = fn(xmin);
                            ++icount;
                        }
                        ylo = y[0];
                        iL = 0;

                        for (size_t i = 1; i < nn; ++i) {
                            if (y[i] < ylo) {
                                ylo = y[i];
                                iL = i;
                            }
                        }
                        continue;
                    }

                    // Retain contraction.
                    else
                    {
                        for (size_t i = 0; i < n; ++i) {
                            p[i+iH*n] = p2star[i];
                        }
                        y[iH] = y2star;
                    }
                }

                // Contraction on the reflection side of the centroid.
                else if (k == 1)
                {
                    for (size_t i = 0; i < n; ++i) {
                        p2star[i] = pbar[i] + ccoeff * (pstar[i] - pbar[i]);
                    }
                    const T y2star = fn(p2star);
                    ++icount;

                    // Retain reflection?
                    if (y2star <= ystar) {
                        for (size_t i = 0; i < n; ++i) {
                            p[i+iH*n] = p2star[i];
                        }
                        y[iH] = y2star;
                    } else {
                        for (size_t i = 0; i < n; ++i) {
                            p[i+iH*n] = pstar[i];
                        }
                        y[iH] = ystar;
                    }
                }
            }

            // Check if YLO improved.
            if (y[iH] < ylo) {
                ylo = y[iH];
                iL = iH;
            }
            --jcount;

            if (0 < jcount) { continue; }

            // Check to see if minimum reached.
            if (icount <= kcount) {
                jcount = konvge;

                T z = std::accumulate(y, y+nn, static_cast<T>(0));
                // T z = 0; for (size_t i = 0; i < nn; ++i) { z += y[i]; }
                const T x = z / dnn;

                z = 0.0;
                for (size_t i = 0; i < nn; ++i) { z += pow(y[i] - x, 2); }
                if (z <= rq) { break; }
            }
        }

        // Factorial tests to check that YMIN is a local minimum.
        for (size_t i = 0; i < n; ++i) { xmin[i] = p[i+iL*n]; }
        ymin = y[iL];

        if (kcount < icount) { ifault = 2; break; }
        ifault = 0;

        for (size_t i = 0; i < n; ++i) {
            del = step[i] * eps;
            xmin[i] += del;
            ++icount;
            if (fn(xmin) < ymin) { ifault = 2; break; }
            xmin[i] -= 2*del;
            ++icount;
            if (fn(xmin) < ymin) { ifault = 2; break; }
            xmin[i] += del;
        }

        if (ifault == 0) { break; }

        // Restart the procedure.
        for (size_t i = 0; i < n; ++i) { start[i] = xmin[i]; }
        del = eps;
        ++numres;
    }

#if !(defined(HAVE_C99_VARIABLE_LENGTH_ARRAYS) || defined(HAVE_CXX11_UNIQUE_PTR))
    delete [] p;
    delete [] pstar;
    delete [] p2star;
    delete [] pbar;
    delete [] y;
#endif
    return 0;
}
