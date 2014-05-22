#include "udunits.hpp"

using std::cout;
using std::endl;

int test_udunits()
{
    using namespace ud;
    int ret = 0;

    System sys;

    const Unit m(sys, "meter");
    const Unit m1000 = scale(1000,m);
    const Unit m2(sys, "meter");
    const Unit s(sys, "second");
    const Unit c = m * m2;
    const Unit d = m / m2;
    const Unit mps = m/s;

    cout
        << "m:" << " [" << m << "]" << endl
        << "m1000:" << " [" << m1000 << "]" << endl
        << "m2:" << " [" << m2 << "]" << endl
        << "s:" << " [" << s << "]" << endl
        << "c:" << " [" << c << "]" << endl
        << "d:" << " [" << d << "]" << endl
        << "mps:" << " [" << mps << "]" << endl
        ;

    if (m == m2) {
        cout << "m == m2" << endl;
    } else {
        cout << "m != m2" << endl;
    }

    if (m == c) {
        cout << "m == c" << endl;
    } else {
        cout << "m != c" << endl;
    }

    return ret;
}

int main(int argc, char * argv[])
{
    test_udunits();
    return 0;
}
