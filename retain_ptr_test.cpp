#include <iostream>
#include "sg14/memory.hpp"

using namespace std;

class Base
{
public:
    Base(int z) : _z(z)
    {
    }
    int _z = 1;
    mutable unsigned int _rc = 1;
};

struct Base_traits final
{
    static void increment(const Base* x) noexcept
    {
        x->_rc += 1;
        cout << __PRETTY_FUNCTION__ << ": rc=" << x->_rc << endl;
    }
    static void decrement(const Base* x) noexcept
    {
        x->_rc -= 1;
        cout << __PRETTY_FUNCTION__ << ": rc=" << x->_rc << endl;
        if (x->_rc == 0)
        {
            delete x;
        }
    }
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
};

struct Sub_traits final
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

using RetainBase = sg14::retain_ptr<const Base, Base_traits>;
using RetainSub = sg14::retain_ptr<const Sub, Base_traits>;

int main(__attribute__((unused)) int argc,
         __attribute__((unused)) const char* argv[],
         __attribute__((unused)) const char* envp[])
{
    cout << "sizeof(Sub): " << sizeof(Sub) << endl;
    cout << "sizeof(RetainSub): " << sizeof(RetainSub) << endl;

    auto sub = RetainSub(new Sub(42));
    auto base = RetainBase(new Base(42));

    return 0;
}
