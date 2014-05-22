/*! \file mse.cpp
 * \brief Error Measures.
 */

#pragma once
#include "cc_features.h"
#include <cmath>
#include <armadillo>

namespace arma {

/*! Mean-Squared Error (MSE)
 * \see https://en.wikipedia.org/wiki/Mean_squared_error
 */
template<class T> inline pure T mse(const Col<T>& a, const Col<T>& b) { return as_scalar(mean(square(a-b))); }

/*! Root-Mean-Square Deviation (RMSD)
 * \alias rmse
 * \see https://en.wikipedia.org/wiki/Root_mean_square_deviation
 */
template<class T> inline pure T rmsd(const Col<T>& a, const Col<T>& b) { return std::sqrt(mse(a,b)); }

/*! Normalized Root-Mean-Square Deviation (NRMSD)
 * \alias nrmse
 * \see https://en.wikipedia.org/wiki/Root_mean_square_deviation#Normalized_root-mean-square_deviation
 */
template<class T> inline pure T nrmsd(const Col<T>& a, const Col<T>& b) { return rmsd(a,b) / ((std::max(max(a), max(b))) -
                                                                                               (std::min(min(a), min(b)))); }

}
