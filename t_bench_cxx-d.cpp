/*! \file t_bench_cxx-d.cpp
 * \brief C++ Test Program.
 * \see http://stackoverflow.com/questions/5142366/how-fast-is-d-compared-to-c
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
extern "C" { extern char *gets(char *__s) { return 0; }; }
#endif

#include <iostream>
#include <random>
#include <chrono>
#include <string>
#include <vector>
#include <array>

typedef std::chrono::duration<long, std::ratio<1, 1000>> millisecs;
template<typename _T>
long time_since(std::chrono::time_point<_T>& time) {
    long tm = std::chrono::duration_cast<millisecs>( std::chrono::system_clock::now() - time).count();
    time = std::chrono::system_clock::now();
    return tm;
}

const long N = 20000;
const int size = 10;

typedef int value_type;
typedef long long result_type;
typedef std::vector<value_type> vector_t;
typedef typename vector_t::size_type size_type;

inline value_type scalar_product(const vector_t& x, const vector_t& y) {
    value_type res = 0;
    size_type siz = x.size();
    for (size_type i = 0; i < siz; ++i)
        res += x[i] * y[i];
    return res;
}

int main() {
    auto tm_before = std::chrono::system_clock::now();

    // 1. allocate and fill randomly many short vectors
    vector_t* xs = new vector_t [N];
    for (int i = 0; i < N; ++i) {
        xs[i] = vector_t(size);
    }
    std::cerr << "allocation: " << time_since(tm_before) << " ms" << std::endl;

    std::mt19937 rnd_engine;
    std::uniform_int_distribution<value_type> runif_gen(-1000, 1000);
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < size; ++j)
            xs[i][j] = runif_gen(rnd_engine);
    std::cerr << "random generation: " << time_since(tm_before) << " ms" << std::endl;

    // 2. compute all pairwise scalar products:
    time_since(tm_before);
    result_type avg = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            avg += scalar_product(xs[i], xs[j]);
    avg = avg / N*N;
    auto time = time_since(tm_before);
    std::cout << "result: " << avg << std::endl;
    std::cout << "time: " << time << " ms" << std::endl;
}
