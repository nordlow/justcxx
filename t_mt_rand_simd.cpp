/*! \file t_mt_rand_gcc.cpp
 * \brief Test SIMD Optimized Mersenne Twister in GCC.
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
extern "C" { extern char *gets(char *__s) { return 0; }; }
#endif

#include <iostream>
#include <string>
#include <algorithm>
#include <ext/random>
#include <chrono>
#include <cstdlib>

using std::cout;
using std::endl;
using std::hex;
using std::dec;

/*! Test Performance of SIMD-Optimized Mersenne Twister.
 */
template<class T>
int test_mersenne_simd(int argc,
                       const char * argv[],
                       const char * envp[])
{
    typedef std::chrono::high_resolution_clock hrc;

    size_t n = 2e7;
    std::vector<T> f(n);

    {
        __gnu_cxx::sfmt607 r;
        auto tA = hrc::now();
        std::generate(begin(f), end(f), r);
        auto tB = hrc::now();
        auto count = std::chrono::duration_cast<std::chrono::milliseconds>(tB - tA).count();
        cout << "__gnu_cxx::sfmt607: " << count << " milliseconds" << endl;
    }

    {
        auto tA = hrc::now();
        std::generate(begin(f), end(f), rand);
        auto tB = hrc::now();
        auto count = std::chrono::duration_cast<std::chrono::milliseconds>(tB - tA).count();
        cout << "rand: " << count << " milliseconds" << endl;
    }

    {
        auto tA = hrc::now();
        std::generate(begin(f), end(f), []() { return 11; });
        auto tB = hrc::now();
        auto count = std::chrono::duration_cast<std::chrono::milliseconds>(tB - tA).count();
        cout << "return constant: " << count << " milliseconds" << endl;
    }

    {
        auto tA = hrc::now();
        std::fill(begin(f), end(f), 11);
        auto tB = hrc::now();
        auto count = std::chrono::duration_cast<std::chrono::milliseconds>(tB - tA).count();
        cout << "fill: " << count << " milliseconds" << endl;
    }

    return 0;
}

int main(int argc, const char * argv[], const char * envp[])
{
    test_mersenne_simd<int32_t>(argc, argv, envp);
    test_mersenne_simd<int64_t>(argc, argv, envp);
}
