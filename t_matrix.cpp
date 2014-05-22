#include <armadillo>
#include "algorithm_x.hpp"
#include "mandelbrot.hpp"
#include "Timer.hpp"

using std::cout;
using std::endl;

template<class T>
void test_matrix()
{
    typedef arma::Mat<T> Mat;

    uint len = 128;
    Mat m(len, len, 2);
    for(uint i = 0; i < len; i++)
    {
        for(uint j = 0; j < len; j++)
        {
            m(i,j) = std::rand();
        }
    }
    Mat m0 = m;

    Timer t;
    uint cnt = 4;
    for(uint i = 0; i < cnt; i++) {
        m.i();
        m.i();
    }
    t.stop();
    cout << t.get_elapsed() /(2*cnt) << "s" << endl;
    float sum = 0;
    for(uint i = 0; i < len; i++)
    {
        for(uint j = 0; j < len; j++)
        {
            sum += pnw::square(m(i,j) - m0(i,j));
        }
    }
    cout << "sum: " << sum << endl;
}

int main(int argc, char *argv[])
{
    typedef float T;
    while (true) {
        test_matrix<T>();
    }
}
