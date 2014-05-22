/*! \file t_eval.cpp
 * \brief Test eval()
 * \see http://functionalcpp.wordpress.com/2013/08/03/generalized-function-evaluation/
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
extern "C" { extern char *gets(char *__s) { return 0; }; }
#endif

#include "algorithm_x.hpp"
#include "enforce.hpp"

/*! Example Functor.
 */
struct Bloop {
    int a = 10;
    int operator() () { return a; }
    int operator() (int n) { return a+n; }
    int triple() { return a*3; }
};

int add_one(int n) { return n+1; }

int main()
{
    using pnw::eval;

    // free function
    enforce_eq(eval(add_one,0), 1);

    // lambda function
    enforce_eq(eval([](int n){return n+1;},1), 2);

    // functor
    enforce_eq(eval(bloop), 10);
    enforce_eq(eval(bloop, 4), 14);

    // member function
    enforce_eq(eval(&Bloop::triple,&bloop), 30);

    // member object
    enforce_eq(eval(&Bloop::a,&bloop), 10);

    return 0;
}
