/*! \file t_ranged_and_saturated.cpp
 * \brief Test Ranged and Saturated Wrapper Templates.
 */

#include <iostream>
#include <string>
#include "maybe.hpp"
#include "ranged.hpp"
#include "saturated.hpp"
#include "safe.hpp"
#include "show.hpp"
#include "enforce.hpp"

using std::cout;
using std::endl;
using std::hex;
using std::dec;

template<class R> void predec(R& a) { --a; }
template<class R> void preinc(R& a) { ++a; }
template<class R> void postdec(R& a) { a--; }
template<class R> void postinc(R& a) { a++; }

template<class T>
int test_ranged(int argc,
                const char * argv[],
                const char * envp[])
{
    typedef range<T> L;        // limits
    typedef ranged<T> R;        // ranged
    const auto low = 0.0, high = 10.0;

    const R x(1.0, L(0.0, 10));
    const R y(2.0, 0.0, 10.0);

    dshowln(x);

    enforce_eq(y.min(), 0.0);
    enforce_eq(y.max(), 10.0);

    enforce_eq((x + y).min(), 0.0);
    enforce_eq((x + y).max(), 20.0);

    enforce_eq((x - y).min(),-10.0);
    enforce_eq((x - y).max(),+10.0);

    enforce_eq((x * y).min(),+0.0);
    enforce_eq((x * y).max(),+100.0);

    enforce_eq(min(x, y), 1.0);
    enforce_eq(max(x, y), 2.0);

    dshowln(x);
    dshowln(y);
    dshowln(min(x, y));

    enforce_eq(1.0, 1.0);
    enforce_neq(1.0, 2.0);

    enforce_lt(1.0, 2.0);
    enforce_lte(1.0, 1.0);

    enforce_gt(2.0, 1.0);
    enforce_gte(2.0, 2.0);


    dshowln(y);

    R z(y);
    dshowln(z);

    L lims(low, high);
    R l_(low, lims);
    R h_(high, lims);
    // enforce_pure_thrown(predec<R>, l_);
    // enforce_pure_thrown(preinc<R>, l_);
    // enforce_pure_thrown(postdec<R>, l_);
    // enforce_pure_thrown(postinc<R>, l_);
    // enforce_pure_thrown([](R& a) { a--; }, l_);
    // enforce_pure_thrown([](R& a) { --a; }, l_);
    // enforce_pure_thrown([](R& a) { a++; }, h_);
    // enforce_pure_thrown([](R& a) { ++a; }, h_);

    auto got = false; try { R w(-1.0, 0.0, 10.0); } catch (const std::out_of_range& e) { got = true; }
    enforce(got);               // assure exception was thrown and caught

    return 0;
}

/*! Test Corner Cases of Saturated Arithmetic.
 */
template<class T>
int test_saturated(int argc,
                   const char * argv[],
                   const char * envp[])
{
    typedef saturated<T> S;        // saturated

    S low(S::min());
    S high(S::max());

    enforce_eq(low, low - 1);    // enforce floor saturation
    enforce_eq(high, high + 1);  // enforce ceiling saturation

    const S low_m1 = low - 1;
    enforce_eq(low, low_m1);    // enforce floor saturation

    enforce_eq(--low, S::min()); // enforce floor saturation
    enforce_eq(++high, S::max()); // enforce ceiling saturation

    low--; enforce_eq(low, S::min()); // enforce floor saturation
    high--; enforce_eq(high, S::max()); // enforce ceiling saturation

    S x(1), y(2);
    enforce_eq(x + y, static_cast<S>(3));
    enforce_lt(x, y);
    enforce_lt(x, static_cast<S>(2));
    enforce_lt(y, static_cast<S>(3));
    enforce_gt(static_cast<S>(3), y);
    enforce_eq(y, static_cast<S>(2));
    return 0;
}

int main(int argc,
         const char * argv[],
         const char * envp[])
{
    int ret = 0;

    ret += test_ranged<int>(argc, argv, envp); cout << endl;
    ret += test_ranged<float>(argc, argv, envp); cout << endl;
    ret += test_ranged<double>(argc, argv, envp); cout << endl;

    ret += test_saturated<unsigned int>(argc, argv, envp); cout << endl;
    ret += test_saturated<int>(argc, argv, envp); cout << endl;

    const auto x = make_safe<double,true>(12.0, 0.0, 20.0);
    cout << x << endl;

    return 0;
}
