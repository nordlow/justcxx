/*! \file t_nelder_mead.cpp
 * \brief Test Nelder-Meade Optimization Method.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-04-09 01:33
 */

#include <iostream>
#include <iomanip>
#include "opt_funs.hpp"
#include "nelder_mead.hpp"

using std::cout;
using std::endl;

/*! Use of \c nelder_mead_min on \c rosenbrock. */
template<class T>
void test_nelder_mead_min_rosenbrock() {
    cout << endl << __FUNCTION__ << endl << endl;

    size_t n = 2;
    T start[] = { -1.2, 1.2 };
    T step[] = { 1.0, 1.0 };
    size_t konvge = 10;
    size_t kcount = 500;

    cout << "Starting point X: ";
    for (size_t i = 0; i < n; i++) { cout << "  " << std::setw(14) << start[i] << "\n"; }

    T ymin = rosenbrock<T>(start);
    cout << "\n" << "  F(X) = " << ymin << "\n";

    T xmin[2];
    size_t icount, ifault, numres;
    const T reqmin = 1.0E-08;
    nelder_mead_min(rosenbrock, n, start, xmin, ymin, reqmin, step,
                     konvge, kcount, icount, numres, ifault);

    cout << "\n  Return code IFAULT = " << ifault << "\n\n";
    cout << "  Estimate of minimizing value X*:\n\n";
    for (size_t i = 0; i < n; i++) { cout << "  " << std::setw(14) << xmin[i] << "\n"; }
    cout << "\n  F(X*) = " << ymin << "\n\n";
    cout << "  Number of iterations = " << icount << "\n";
    cout << "  Number of restarts =   " << numres << "\n";
}

/*! Use \c nelder_mead_min on \c powell. */
template<class T>
void test_nelder_mead_min_powell() {
    size_t i;
    size_t icount;
    size_t ifault;
    size_t kcount;
    size_t konvge;
    size_t n;
    size_t numres;
    T reqmin;
    T *start;
    T *step;
    T *xmin;
    T ymin;

    n = 4;

    start = new T[n];
    step = new T[n];
    xmin = new T[n];

    cout << "\n";
    cout << "TEST02\n";
    cout << "  Apply NELDER_MEAD_MIN to POWELL quartic function.\n";

    start[0] =   3.0;
    start[1] = - 1.0;
    start[2] =   0.0;
    start[3] =   1.0;

    reqmin = 1.0E-08;

    step[0] = 1.0;
    step[1] = 1.0;
    step[2] = 1.0;
    step[3] = 1.0;

    konvge = 10;
    kcount = 500;

    cout << "\n";
    cout << "  Starting point X:\n";
    cout << "\n";
    for (i = 0; i < n; i++) { cout << "  " << std::setw(14) << start[i] << "\n"; }

    ymin = powell(start);

    cout << "\n";
    cout << "  F(X) = " << ymin << "\n";

    nelder_mead_min(powell, n, start, xmin, ymin, reqmin, step,
                       konvge, kcount, icount, numres, ifault);

    cout << "\n";
    cout << "  Return code IFAULT = " << ifault << "\n";
    cout << "\n";
    cout << "  Estimate of minimizing value X*:\n";
    cout << "\n";
    for (i = 0; i < n; i++) { cout << "  " << std::setw(14) << xmin[i] << "\n"; }

    cout << "\n";
    cout << "  F(X*) = " << ymin << "\n";

    cout << "\n";
    cout << "  Number of iterations = " << icount << "\n";
    cout << "  Number of restarts =   " << numres << "\n";

    delete [] start;
    delete [] step;
    delete [] xmin;

    return;
}

/*! Use of \c nelder_mead_min on \c helical. */
template<class T>
void test_nelder_mead_min_helical() {
    size_t i;
    size_t icount;
    size_t ifault;
    size_t kcount;
    size_t konvge;
    size_t n;
    size_t numres;
    T reqmin;
    T *start;
    T *step;
    T *xmin;
    T ymin;

    n = 3;

    start = new T[n];
    step = new T[n];
    xmin = new T[n];

    cout << "\n";
    cout << "TEST03\n";
    cout << "  Apply NELDER_MEAD_MIN to the HELICAL function.\n";

    start[0] = - 1.0;
    start[1] =   0.0;
    start[2] =   0.0;

    reqmin = 1.0E-08;

    step[0] = 1.0;
    step[1] = 1.0;
    step[2] = 1.0;

    konvge = 10;
    kcount = 500;

    cout << "\n";
    cout << "  Starting point X:\n";
    cout << "\n";
    for (i = 0; i < n; i++) { cout << "  " << std::setw(14) << start[i] << "\n"; }

    ymin = helical(start);

    cout << "\n";
    cout << "  F(X) = " << ymin << "\n";

    nelder_mead_min(helical, n, start, xmin, ymin, reqmin, step,
                     konvge, kcount, icount, numres, ifault);

    cout << "\n";
    cout << "  Return code IFAULT = " << ifault << "\n";
    cout << "\n";
    cout << "  Estimate of minimizing value X*:\n";
    cout << "\n";
    for (i = 0; i < n; i++) { cout << "  " << std::setw(14) << xmin[i] << "\n"; }

    cout << "\n";
    cout << "  F(X*) = " << ymin << "\n";

    cout << "\n";
    cout << "  Number of iterations = " << icount << "\n";
    cout << "  Number of restarts =   " << numres << "\n";

    delete [] start;
    delete [] step;
    delete [] xmin;

    return;
}

/*! Use of \c nelder_mead_min on \c quartic. */
template<class T>
void test_nelder_mead_min_quartic() {
    size_t i;
    size_t icount;
    size_t ifault;
    size_t kcount;
    size_t konvge;
    size_t numres;
    T reqmin;
    T ymin;

    size_t n = 10;

    T* start = new T[n];
    T* step = new T[n];
    T* xmin = new T[n];

    cout << "\n";
    cout << "TEST04\n";
    cout << "  Apply NELDER_MEAD_MIN to the QUARTIC function.\n";

    for (i = 0; i < n; i++) { start[i] = 1.0; }
    reqmin = 1.0E-08;
    for (i = 0; i < n; i++) { step[i] = 1.0; }

    konvge = 10;
    kcount = 500;

    cout << "\n";
    cout << "  Starting point X:\n";
    cout << "\n";
    for (i = 0; i < n; i++) { cout << "  " << std::setw(14) << start[i] << "\n"; }

    ymin = quartic(start);

    cout << "\n";
    cout << "  F(X) = " << ymin << "\n";

    nelder_mead_min(quartic, n, start, xmin, ymin, reqmin, step,
                     konvge, kcount, icount, numres, ifault);

    cout << "\n";
    cout << "  Return code IFAULT = " << ifault << "\n";
    cout << "\n";
    cout << "  Estimate of minimizing value X*:\n";
    cout << "\n";
    for (i = 0; i < n; i++) { cout << "  " << std::setw(14) << xmin[i] << "\n"; }

    cout << "\n";
    cout << "  F(X*) = " << ymin << "\n";

    cout << "\n";
    cout << "  Number of iterations = " << icount << "\n";
    cout << "  Number of restarts =   " << numres << "\n";

    delete [] start;
    delete [] step;
    delete [] xmin;
}

template<class T>
int test_nelder_mead_min() {
    test_nelder_mead_min_rosenbrock<T>();
    test_nelder_mead_min_powell<T>();
    test_nelder_mead_min_helical<T>();
    test_nelder_mead_min_quartic<T>();
    return 0;
}

int main(int argc, const char * argv[], const char * envp[])
{
    test_nelder_mead_min<float>();
    test_nelder_mead_min<double>();
    return 0;
}
