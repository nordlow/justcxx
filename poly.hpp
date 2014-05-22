/*! \file poly.hpp
 * \brief Operations on Polynomials.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-04-16 11:04
 *
 * (Weighted) Polynomial Fitting and Evaluation.
 *
 * \todo Specialize linear fitting using https://hbfs.wordpress.com/2012/04/24/introduction-to-gradient-descent/
 * \todo Add nulling of coefficients
 *
 * \see http://mathworld.wolfram.com/LeastSquaresFitting.html
 * \see https://hbfs.wordpress.com/2012/04/24/introduction-to-gradient-descent/
 * \see http://www.boost.org/doc/libs/1_49_0/libs/math/doc/sf_and_dist/html/math_toolkit/toolkit/internals2/polynomials.html
 * \see http://www.boost.org/doc/libs/1_49_0/libs/math/doc/sf_and_dist/html/math_toolkit/toolkit/internals1/rational.html
 * \see MATLAB's polyfit, polyval
 */

#pragma once

#include "cc_features.h"
#include <algorithm>
#include <armadillo>
#include "cc_features.h"
#include "math_x.hpp"
#include "vandermonde.hpp"

namespace arma {

/*! Return \p m:th Degree Polynomial to \p n number of samples to \p y at \x.
 * Result can be used directly by \c polyval().
 *
 * \param[in] y Sample Values.
 * \param[in] x Sample Positions.
 * \param[in] m Degree of Polynomial.
 * \param[in] w Sample Weights.
 * \param[in] slow Flags for Slow Calculation.
 *
 * \see https://en.wikipedia.org/wiki/Polynomial_regression#Matrix_form_and_calculation_of_estimates
 */
template<class T>
inline pure Col<T> polyfit(const Col<T>& y,
                           const Col<T>& x,
                           size_t m,
                           const Col<T>& w = Col<T>(), // default to non-weighted
                           bool slow = false)
{
    const auto n = y.size();
    if (m >= n) { throw std::length_error("m >= n is not allowed"); }
    return solve(vandermonde(x, n, m, w),
                 w.size() ? w % y : y, slow);
}

/*! Return \p Linear Polynomial to \p n number of samples to \p y at \x.
 * Faster than \c polyfit.
 * \see https://hbfs.wordpress.com/2012/04/24/introduction-to-gradient-descent/
 */
template<class T>
inline pure Col<T> linear_polyfit(const Col<T>& y,
                                  const Col<T>& x)
{
    const auto n = y.size();
    const T xy = as_scalar(dot(x,y)); // x . y
    const T xs = as_scalar(sum(x));   // x sum
    const T ys = as_scalar(sum(y));   // y sum
    const T ssx = as_scalar(sum(square(x))); // square sum x
    const T sxs = std::sqr(xs);
    const T a = ((ys * ssx - xs*xy) /
                 (n*ssx - sxs));
    const T b = ((n*xy - xs*ys) /
                 (n*ssx - sxs));
    return Col<T>{a,b};
}

template<class T>
inline pure Col<T> adaptive_polyfit(const Col<T>& y,
                                    const Col<T>& x,
                                    size_t m,
                                    const Col<T>& w = Col<T>(), // default to non-weighted
                                    bool slow = false) {
    if (m == 0) {
        if (w.size() == 0 or    // if either no weights
            w == 1) {           // are all equal to 1
            return mean(y);     // just mean
        } else {
            return mean(y % w);   // weighted mean
        }
    } else if (m == 1 and w.size() == 0) {
        return linear_polyfit(y, x);
    } else {
        return polyfit(y, x, m, Col<T>(), slow);
    }
}

#if HAVE_CXX11_EXTERN_TEMPLATES
extern template pure Col<float> polyfit<float>(const Col<float>& y,
                                               const Col<float>& x,
                                               size_t m,
                                               const Col<float>& w = Col<float>(), // default to non-weighted
                                               bool slow = false);
extern template pure Col<double> polyfit<double>(const Col<double>& y,
                                                 const Col<double>& x,
                                                 size_t m,
                                                 const Col<double>& w = Col<double>(), // default to non-weighted
                                                 bool slow = false);
extern template pure Col<float> linear_polyfit<float>(const Col<float>& y, const Col<float>& x);
extern template pure Col<double> linear_polyfit<double>(const Col<double>& y, const Col<double>& x);
#endif

/*! Return Matlab-Formatted (Higest Degree Coefficients First) \p m:th Degree
 *  Polynomial to \p n number of samples to \p y at \x. */
template<class T> inline pure Col<T> polyfit_matlab(const Col<T>& y,
                                                    const Col<T>& x,
                                                    size_t m,
                                                    const Col<T>& w = Col<T>(), // default to non-weighted
                                                    bool slow = false)
{
    auto a = polyfit(y,x,m,w,slow); std::reverse(begin(a), end(a)); return a;
}
}

#include <boost/math/tools/polynomial.hpp>

namespace arma {
/*!
 * Evaluate Polynomial \p a at position \p x.
 */
template<class T, class U> inline pure T polyval(const Col<T>& a, U x)
{
    return boost::math::tools::evaluate_polynomial(a.memptr(), x, a.size());
}
}
