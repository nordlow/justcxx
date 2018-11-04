#include <iostream>
#include <string>

using std::cout;
using std::endl;

class S
{
public:
    S(int x_) : x(x_) {}
    ~S()
    {
        cout << __PRETTY_FUNCTION__ << ":" << endl;
    }
public:
    int x;
    mutable unsigned int _rc = 1;
};

struct S_traits final
{
    static void increment (S* x) noexcept
    {
        cout << __PRETTY_FUNCTION__ << ":" << endl;
        x->_rc += 1;
    }
    static void decrement (S* x) noexcept
    {
        x->_rc -= 1;
        cout << __PRETTY_FUNCTION__ << ": rc=" << x->_rc << endl;
        if (x->_rc == 0) { delete x; }
    }
};

#include "sg14/memory.hpp"
using SP = sg14::retain_ptr<S, S_traits>;

int main(__attribute__((unused)) int argc,
         __attribute__((unused)) const char * argv[],
         __attribute__((unused)) const char * envp[])
{
    cout << "sizeof(S): " << sizeof(S) << endl;
    cout << "sizeof(SP): " << sizeof(SP) << endl;
    {
        auto rp = SP(new S(42));
    }
    return 0;
}
