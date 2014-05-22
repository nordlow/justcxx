/*! \file sinefit.hpp
 * \brief Fit Sine Wave to Data.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-01-07 00:16
 * \see http://www.mathworks.com/matlabcentral/fileexchange/23214-four-parameter-sinefit
 * \see http://www.weizmann.ac.il/matlab/techdoc/ref/lscov.html
 */

#pragma once
#include "enforce.hpp"
#include "cc_features.h"
#include <armadillo>

namespace arma {

/*! 3-parameter Fit of y = sin(qt).
 * Used to create an initial guess for \c sinefit4par().
 * \param[in] y Sample Values
 * \param[in] wt Sample Times
 *
 * %x0=inv(D0.'*D0)*(D0.'*y);
 */
template<class T> pure Mat<T> sinefit3par(const Col<T>& y,
                                          const Col<T>& wt)
{
    typedef Col<T> C;           // column vector
    typedef Mat<T> M;           // matrix
    auto y_n = y.n_rows;
    auto wt_n = wt.n_rows;
    enforce_eq(y_n, wt_n);      // dimensions must match
    auto D0 = join_rows(join_rows(cos(wt),
                                  sin(wt)),
                        ones<C>(wt_n));
    // if isreal(y)
    M Q,R; qr(Q,R,D0);
    return solve(R, (Q.st() * y));
    // if is complex(y)
    // return lscov(D0,y);
}

#if HAVE_CXX11_EXTERN_TEMPLATES
extern template Mat<float> sinefit3par(const Col<float>& y,
                                       const Col<float>& wt);
extern template Mat<double> sinefit3par(const Col<double>& y,
                                        const Col<double>& wt);
#endif

}
