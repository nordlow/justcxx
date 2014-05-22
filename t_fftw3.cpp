/*!
 * \file t_fftw3.cpp
 * \brief
 */

#include <iostream>
#include <string>
#include <complex>
#include <fftw3.h>

using std::cout;
using std::endl;

union stdfftw {
    std::complex< double > stdc;
    fftw_complex           fftw;
};

int main(int argc, const char * argv[], const char * envp[])
{
    std::complex<double> x(1,0);
    stdfftw u;
    u.stdc = x;
    fftw_complex fx = u.fftw;
    return 0;
}
