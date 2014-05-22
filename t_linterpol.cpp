#include <iostream>
#include <vector>
#include <armadillo>

#include "linfilt.hpp"
#include "ramp.hpp"
#include "show_all.hpp"
#include "interpolate.hpp"
#include "cyclic_interpolate.hpp"

using namespace std;

void test_linear_interpolate_1d_sample(double j) {
    std::vector<double> x(16);
    ramp(x, 0, 1);
    double y = cyclic_linear_interpolate_1d_sample(x, j, INTERPOLATE_EDGE_CYCLIC);
    dshowln(x);
    dshowln(y);
}

void test_linear_interpolate_2d_sample(double i, double j) {
    const uint m = 4, n = 4;
    arma::Mat<double> x(m, n);
    // ramp(x, 0, 1, 1);
    double y = cyclic_linear_interpolate_2d_sample(x, i, j, INTERPOLATE_EDGE_CYCLIC);
    dshowln(x);
    dshowln(y);
}

void test2(uint i, uint p) {
    uint a = 0;
    uint b = 1024;
    uint c = linear_interpolate_1d_binary_power_norm(a, b, i, p);
    cout << "a: " << a << endl;
    cout << "b: " << b << endl;
    cout << "c: " << c << endl;
}

void loop2() {
    while (true) {
	uint i, p;
	cout << "i:"; cin >> i;
	cout << "p:"; cin >> p;
	test2(i, p);
	cout << endl;
    }
}

void loop1d() {
    while (true) {
	double j;
	cout << "j:"; cin >> j;
	test_linear_interpolate_1d_sample(j);
	cout << endl;
    }
}

void loop2d() {
    while (true) {
	double i, j;
	cout << "i:"; cin >> i;
	cout << "j:"; cin >> j;
	test_linear_interpolate_2d_sample(i, j);
	cout << endl;
    }
}

int main(int argc, char * argv[]) {
    loop2d();
    return 0;
}
