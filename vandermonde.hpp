/*! \file vandermonde.hpp
 * \brief Construct Vandermonde Matrix.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \todo Support C++11 variants with std::function
 * \date 2012-04-17 02:35
 */

#pragma once
#include <algorithm>
#include <armadillo>
#include "enforce.hpp"

// template<class C> typename C::iterator begin(C& a) { return a.begin(); }
// template<class C> typename C::iterator end(C& a) { return a.end(); }
// template<class C> typename C::const_iterator begin(const C& a) { return a.begin(); }
// template<class C> typename C::const_iterator end(const C& a) { return a.end(); }

namespace arma {

/*! Return Vandermonde Matrix of \p x optionally weighted by \p w.
 * \param[in] x is data vector.
 * \param[in] w is weight vector if non-empty (defaulted to empty)
 * \see https://en.wikipedia.org/wiki/Vandermonde_matrix
 * \todo Use C++11 std::for_each and non-member \c begin \c end.
 * */
template<class T>
inline pure
Mat<T> vandermonde(const Col<T>& x, size_t n, size_t m,
                   const Col<T>& w = Col<T>()
                   )
{
    Mat<T> V(n, m+1);
    V.col(0) = ones<Col<T> >(n); // \todo We want this to be V.col(0) = 1; or even better V(all,0) = 1;

    // weights
    const auto ws = w.size();
    if (ws != 0) {                // if weights supplied
        enforce_eq(ws, x.size()); // pre-condition
        V = diagmat(w) * V;     // apply weight
    }

    switch (m) {
    case 0:                     // no x coefficients for fitting to constant
        break;
    case 1:
        V.col(1) = x;
        break;
    case 2:
        V.col(1) = x;
        V.col(2) = square(x);   // elementwise squaring of each member in x
        break;
    case 3:
        V.col(1) = x;
        V.col(2) = square(x);
        V.col(3) = V.col(2) % x;
        break;
    case 4:
        V.col(1) = x;
        V.col(2) = square(x);
        V.col(3) = V.col(2) % x;
        V.col(4) = square(V.col(2));
        break;
    case 5:
        V.col(1) = x;
        V.col(2) = square(x);
        V.col(3) = V.col(2) % x;
        V.col(4) = square(V.col(2));
        V.col(5) = V.col(4) % x;
        break;
    case 6:
        V.col(1) = x;
        V.col(2) = square(x);
        V.col(3) = V.col(2) % x;
        V.col(4) = square(V.col(2));
        V.col(5) = V.col(4) % x;
        V.col(6) = square(V.col(3));
        break;
    case 7:
        V.col(1) = x;
        V.col(2) = square(x);
        V.col(3) = V.col(2) % x;
        V.col(4) = square(V.col(2));
        V.col(5) = V.col(4) % x;
        V.col(6) = square(V.col(3));
        V.col(7) = V.col(6) % x;
        break;
    case 8:
        V.col(1) = x;
        V.col(2) = square(x);
        V.col(3) = V.col(2) % x;
        V.col(4) = square(V.col(2));
        V.col(5) = V.col(4) % x;
        V.col(6) = square(V.col(3));
        V.col(7) = V.col(6) % x;
        V.col(8) = square(V.col(4));
        break;
    default:
        for (size_t j = 1; j != m+1; ++j) { V.col(j) = pow(x, j); }
        // std::for_each(V.begin_col(1), V.end_col(n-1),
        //               [n](double col) {
        //                   *col = ones<Col<T>>(n);
        //               });
        break;
    }
    return V;
}

}
