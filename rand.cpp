#include "rand.hpp"

#include <ext/random>

/*! Randomized \p n elements at \p first. */
int32_t* rand_n(int32_t* first, size_t n) { __gnu_cxx::sfmt607 r; std::generate(first, first + n, r); return first + n; }
/*! Randomized \p n elements at \p first. */
int64_t* rand_n(int64_t* first, size_t n) { __gnu_cxx::sfmt607_64 r; std::generate(first, first + n, r); return first + n; }
