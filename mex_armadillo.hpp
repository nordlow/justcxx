/*!
 * \file mex_armadillo.hpp
 * \brief Mex C++ Armadillo Library Integration.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-04-09 02:58
 * \see http://www.myoutsourcedbrain.com/2009/08/fast-scientific-computation-in-mex.html
 */

#pragma once

#include "mex_x.hpp"

#include<armadillo>
 
using namespace arma;
 
void convert(mat& A, const mxArray *mxdata)
{
// delete [] A.mem; // don't do this!
    access::rw(A.mem)    = mxGetPr(mxdata);
    access::rw(A.n_rows) = mxGetM(mxdata); // transposed!
    access::rw(A.n_cols) = mxGetN(mxdata);
    access::rw(A.n_elem) = A.n_rows * A.n_cols;
};
 
void restore(mat& A, const double *ptr)
{
    access::rw(A.mem) = ptr;
    access::rw(A.n_rows) = 1;   // transposed!
    access::rw(A.n_cols) = 1;
    access::rw(A.n_elem) = 1;
};
