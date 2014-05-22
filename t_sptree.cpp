/*! \file t_sptree.cpp
 * \brief Test spstree.
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
extern "C" { extern char *gets(char *__s) { return 0; }; }
#endif

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include "rand.hpp"
#include "geometry/vec.hpp"
#include "geometry/sptree.hpp"
#include "timed.hpp"
#include "show_all.hpp"

using std::cout;
using std::endl;
using std::hex;
using std::dec;

using namespace pnw::ntree;

template<class T, size_t N, class I, bool AdaptDistribution>
void test_tree(const std::vector<particle<T,N> >& pc)
{
    branch<T,N,AdaptDistribution> tree(pc);
}

template<class T, size_t N, class I, bool AdaptDistribution>
int test_tree(int argc, const char * argv[], const char * envp[])
{
    dshowln(N);
    typedef particle<T,N> P;    ///< Particle alias
    typedef std::vector<P> PC;   // Particle Cloud
    typedef leaves<T,N> Leaves;
    typedef branch<T,N,AdaptDistribution> Tree;

    dshowln(sizeof(Leaves));
    dshowln(sizeof(Tree));

    const auto c = rand_n<PC>(2e1);
    Tree tree(c);
    std::cout << tree << std::endl;

    auto res = pnw::timed(10*1024*1024,
                          static_cast<std::chrono::seconds>(5)).call_s(test_tree<T,N,I,AdaptDistribution>);
    cout << res << endl;

    return 0;
}

template<class T, class I, bool AdaptDistribution>
int test_tree_all_dimensions(int argc, const char * argv[], const char * envp[])
{
    test_tree<T,2,I,AdaptDistribution>(argc, argv, envp);
    test_tree<T,3,I,AdaptDistribution>(argc, argv, envp);
    return 0;
}

int main(int argc, const char * argv[], const char * envp[])
{
    std::cout.precision(2);
    test_tree_all_dimensions<float, uint32_t, false>(argc, argv, envp);
    test_tree_all_dimensions<float, uint32_t, true>(argc, argv, envp);
}
