#include "Timer.hpp"
#include <armadillo>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;

double sum(const std::vector<double> & v) {
    double sum = 0;
    auto i = begin(v);
    while (i++ != end(v))
        sum += *i;
    return sum;
}

template<typename T>
T sum(T * v, int n) {
    T sum = 0;
    for (int i = 0; i < n; i++)
        sum += v[i];
    return sum;
}

double c_sum1(double * v, int n) {
    double sum = 0;
    for (int i = 0; i < n; i++)
        sum += v[i];
    return sum;
}

double c_sum2(double * v, int n) {
    double sum = 0;
    for (int i = 0; i < n; i++)
        sum += v[i];
    return sum;
}

int main(int argc, char * argv[]) {
    Timer t;

    arma::Mat<double> m(3, 3);
    m(0,0) = 1;

    std::vector<double> v(100000);
    double s;

    while (true)
    {
        t.restart();
        s = sum(v);
        t.stop();
        cout << "sum: " << s << " and took " <<  t << endl;

        t.restart();
        s = sum<double>(&*begin(v), v.size());
        t.stop();
        cout << "sum<double>: " << s << " and took " <<  t << endl;

        t.restart();
        s = c_sum1(&*begin(v), v.size());
        t.stop();
        cout << "c_sum1: " << s << " and took " <<  t << endl;

        t.restart();
        s = c_sum2(&*begin(v), v.size());
        t.stop();
        cout << "c_sum2: " << s << " and took " <<  t << endl;
    }

    return 0;
}
