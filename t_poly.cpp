/*! \file t_poly.cpp
 * \brief Test Polynomial Regression Fitting using Armadillo.
 */

#include <algorithm>
#include <limits>
#include <iostream>
#include "mse.hpp"
#include "poly.hpp"
#include "sinefit.hpp"
// #include "timed.hpp"
#include "enforce.hpp"
#include "show.hpp"

using std::cout;
using std::endl;

template<class T>
void bench_polyfit_armadillo(size_t n)
{
    size_t m = 3;
    using namespace arma;
    Col<T> y; y.randn(n);
    auto x = linspace<arma::Col<T> >(0,1, n);
    auto p = polyfit<T>(y, x, m);
    Col<T> w; w.randn(n);
    auto pw = polyfit<T>(y, x, m, w);
}

template<class T>
void bench_polyfit_armadillo_all()
{
    bench_polyfit_armadillo<T>(10);
}

template<class T>
void test_polyfit_armadillo(size_t m = 3, size_t n = 5)
{
    // MATLAB: m=3; y=[0,1,2,3,2]; w=[2,1,1,1,1]; n=length(y); x=linspace(0,1, n); p=polyfit(x,y, m, [], w); fliplr(p), polyval(p,1)
    using namespace arma;

    auto x = linspace<arma::Col<T> >(0,1, n); // sample points
    Col<T> w = { 2,1,1,1,1 };                 // sample weights
    Col<T> y = { 0,1,2,3,2 };                 // sample values

    auto pS = polyfit<T>(y, x, m);    // normal polynomial
    auto pW = polyfit<T>(y, x, m, w); // weight fit polynomial

    cout << "Standard-Fitted " << m << "-Degree Polynomial pW:" << pS.t();
    cout << "Weighted-Fitted " << m << "-Degree Polynomial wp:" << pW.t();

    // Evaluation
    const T xE = 1;                         // evaluation point

    // Check Standard Result
    {
        // Normal
        const T yP = arma::polyval<T,T>(pS, xE); // predicate value
        cout << "polyval(pS," << xE << "):" << yP << endl << endl;

        if (m == 1) {               // if linear fit
            auto pL = linear_polyfit<T>(y, x);
            auto distPLP = nrmsd(pS, pL);
            enforce_lt(distPLP, 2.0e-8);
            cout << "Linear-Optimized (NRMSD:" << distPLP << ") Fitted Polynomial pL:" << pL.t();
            cout << "pL(" << xE << "):" << arma::polyval<T,T>(pL, xE) << endl << endl;
        }
    }

    // Check Weighted Result
    {
        // Weighted
        const T yP = arma::polyval<T,T>(pW, xE); // predicated value
        cout << "polyval(pW," << xE << "):" << yP << endl << endl;
        switch (m) {
        case 0: {
            const Col<T> p_ = { 1 };  // Matlab result
            const T yP_ = 1;  // Matlab result
            enforce_lt(std::abs(pW[0]-p_[0]), 2e-16);
            enforce_lt(std::abs(yP-yP_), 2e-16);
            break;
        }
        case 1: {
            const Col<T> p_ = { 0.142857142857143,
                                2.742857142857143 };  // Matlab result
            const T yP_ = 2.885714285714286;          // Matlab result
            enforce_lt(nrmsd(pW, p_), 1.5e-7);
            enforce_lt(std::abs(yP-yP_), 2.5e-7);
            break;
        }
        case 2: {
            const Col<T> p_ = { -0.046875000000001,
                                6.537500000000006,
                                -4.250000000000006 };  // Matlab result
            const T yP_ = 2.240624999999999;           // Matlab result
            enforce_lt(nrmsd(pW, p_), 2.6e-7);
            enforce_lt(std::abs(yP-yP_), 4.8e-7);
            break;
        }
        case 3: {
            const Col<T> p_ = { 0.007220216606499,
                                1.367027677496994,
                                11.205776173285198,
                                -10.551143200962697 };  // Matlab result
            const T yP_ = 2.028880866425994;            // Matlab result
            enforce_lt(nrmsd(pW, p_), 4.05e-7);
            enforce_lt(std::abs(yP-yP_), 2.4e-7);
            break;
        }
        }
    }
}

template<class T>
void test_polyfit_armadillo_all()
{
    cout.precision(std::numeric_limits<T>::digits10);
    for (size_t i = 0; i <= 3; ++i) {
        // test_polyfit_armadillo<T>(i);
        test_polyfit_armadillo<T>(i);
    }
}

template<class T>
void test_sinefit_armadillo()
{
    using namespace arma;
    int n = 8;
    Col<T> wt = Col<T>({ 0,1,2,3,4,5,6,7 }) * 2*datum::pi / n; // one period;
    Col<T> y = sin(wt);
    auto p = sinefit3par(y, wt);
    cout << "sinefit3par: " << endl << p << endl << endl;
}

int main(int argc, const char * argv[], const char * envp[]) {
    test_polyfit_armadillo_all<float>();
    test_polyfit_armadillo_all<double>();
    bench_polyfit_armadillo_all<float>();
    bench_polyfit_armadillo_all<double>();
    test_sinefit_armadillo<double>();
    return 0;
}
