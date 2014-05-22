/*! \file t_armadillo.cpp
 * \brief Test Armadillo Matrix Library.
 * \see http://stackoverflow.com/questions/8978742/armadillo-blas-lapack-linking-error
 * \see t_eigen.cpp
 */

#include <iostream>
#include <string>
#include <armadillo>
#include "algorithm_x.hpp"

using std::cout;
using std::endl;

using namespace arma;

template<class C> typename C::const_iterator begin(const C& a) { return a.begin(); }
template<class C> typename C::const_iterator end(const C& a) { return a.end(); }

#if 0
#include "units.hpp"
template<class U, class T>
void test_armadillo_boost_units()
{
    Mat<boost::units::quantity<U, T> > length_mat;
}
#endif

template<class T>
void test_armadillo_rand()
{
    vec  v = randu<vec>(5);
    pnw::sort(v);
    /* std::sort(v.begin(), v.end()); */
    cout << "v" << endl << v << endl;

    mat  A = randu<mat>(5,6);
    cout << "A" << endl << A << endl;

    cube Q = randu<cube>(5,6,7);
    cout << "Q" << endl << Q << endl;
}

template<class T>
void test_armadillo_hist(size_t n = 100)
{
    vec  v  = randn<vec>(n); // Gaussian distribution
    uvec h1 = hist(v, n/100+1);
    uvec h2 = hist(v, linspace<vec>(-2,2,11));
    cout << "v" << endl << v << endl;
    cout << "h1" << endl << h1 << endl;
    cout << "h2" << endl << h2 << endl;
}

template<class T>
void test_armadillo1()
{
    mat A = randu<mat>(4,5);
    mat B = randu<mat>(4,5);
    cout << A*B.t() << endl;
}

template<class T>
void test_armadillo2()
{
    // cout << "2.0 * pi = " << 2.0 * datum::pi << endl
    //      << "speed of light = " << datum::c_0 << endl
    //      << "log_max for floats = " << fdatum::log_max << endl
    //      << "log_max for doubles = " << datum::log_max << endl;
}

int main(int argc, const char * argv[], const char * envp[]) {
    typedef double T;
    test_armadillo_rand<T>();
    test_armadillo_hist<T>();
    test_armadillo1<T>();
    test_armadillo2<T>();
    //test_armadillo_boost_units<boost::units::si::length, double>();
    return 0;
}
