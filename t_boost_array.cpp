#include <boost/array.hpp>
#include "make_array.hpp"
#include "Timer.hpp"
#include "csc.hpp"
#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

template<typename T, std::size_t N>
inline T general_dot(const boost::array<T, N> & a,
                     const boost::array<T, N> & b)
{
    T c = 0;
    for (size_t i = 0; i < N; i++)
    {
        c += a[i] * b[i];
    }
    return c;

}

template<typename T>
inline T special_dot(const boost::array<T, 4> & a,
                     const boost::array<T, 4> & b)
{
    return (a[0] * b[0] +
            a[1] * b[1] +
            a[2] * b[2] +
            a[3] * b[3]);
}

template<typename T, std::size_t N>
std::ostream & operator << (std::ostream & os,
                            const boost::array<T, N> & a)
{
    os << '[';
    for (size_t i = 0; i < N; i++)
    {
        os << ' ' << a[i];
    }
    os << ']';
    return os;
}

typedef int S;			//*< Scalar Type.

void test_make_array()
{
    // Note: GCC produces equivalent optimized code for the following three lines.
    //boost::array<int, 2> ai1 = {{1,2}};
    //int ai1[2] = { 1, 2 };
    auto ai1 = boost::make_array(1,2);
    std::cout << "[ " << ai1[0] << ' ' << ai1[1] << "]" << std::endl;

    std::cout << boost::array<const char*, 2>({{"alpha","beta"}}) << std::endl;
    std::cout << boost::make_array_of_common_type(1,2.2) << std::endl;
    std::cout << boost::make_array_of_common_type(1.1,2) << std::endl;
    std::cout << boost::make_array(1,2,3,4,5,6) << std::endl;
    std::cout << boost::make_array(1.1,2.2,3.3) << std::endl;
    std::cout << boost::make_array("alpha", "beta", "gamma", "delta") << std::endl;
    std::cout << boost::make_array(csc("alpha"),csc("beta")) << std::endl;
}

int main(int argc, char * argv[])
{
    test_make_array();

    typedef boost::array<S, 4> T;

    Timer t;

    const unsigned int nloops = 10000000;

    S sum = 0;
    t.restart();
    for (unsigned int i = 0; i < nloops; i++)
    {
        T a; a[0] = a[1] = a[2] = a[3] = static_cast<S>(i);
        T b; b[0] = b[1] = b[2] = b[3] = static_cast<S>(i+1);
        sum += general_dot(a, b);
    }
    t.stop();
    cout << "general: " << t << endl;

    S tum = 0;
    t.restart();
    for (unsigned int i = 0; i < nloops; i++)
    {
        T a; a[0] = a[1] = a[2] = a[3] = static_cast<S>(i);
        T b; b[0] = b[1] = b[2] = b[3] = static_cast<S>(i+1);
        tum += special_dot(a, b);
    }
    t.stop();
    cout << "special: " << t << endl;

    if (sum == tum)
    {
        cout << "Checksums are equal. OK" << endl;
    }
    else
    {
        cout << "Checksums are not equal. NOT OK" << endl;
    }

    return 0;
}
