#include <iostream>

using namespace std;

class Base
{
public:
    Base(int z) : _z(z)
    {
    }
    int _z = 1;
};

class Sub : public Base
{
public:
    Sub(int x_)
        : Base(42), x(x_)
    {
        cout << __PRETTY_FUNCTION__ << ":" << endl;
    }
    ~Sub() { cout << __PRETTY_FUNCTION__ << ":" << endl; }
public:
    int x;
    mutable unsigned int _rc = 1;
};

struct S_traits final
{
    static void increment(Sub* x) noexcept
    {
        x->_rc += 1;
        cout << __PRETTY_FUNCTION__ << ": rc=" << x->_rc << endl;
    }
    static void decrement(Sub* x) noexcept
    {
        x->_rc -= 1;
        cout << __PRETTY_FUNCTION__ << ": rc=" << x->_rc << endl;
        if (x->_rc == 0)
        {
            delete x;
        }
    }
};

#include "sg14/memory.hpp"
using RP = sg14::retain_ptr<Sub, S_traits>;

int main(__attribute__((unused)) int argc,
         __attribute__((unused)) const char* argv[],
         __attribute__((unused)) const char* envp[])
{
    cout << "sizeof(Sub): " << sizeof(Sub) << endl;
    cout << "sizeof(RP): " << sizeof(RP) << endl;
    auto x = RP(new Sub(42));
    auto y = x;
    return 0;
}
