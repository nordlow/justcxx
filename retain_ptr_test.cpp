#include <iostream>
#include <memory>
#include "sg14/memory.hpp"

using namespace std;

class Base
{
public:
    Base()
    {
    }
    mutable unsigned int _rc = 1;
};

class Sub : public Base
{
public:
    Sub(int x_)
        : Base(), x(x_)
    {
        cout << __PRETTY_FUNCTION__ << ":" << endl;
    }
    ~Sub() { cout << __PRETTY_FUNCTION__ << ":" << endl; }
public:
    int x;
};

void test_shared()
{
}

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

void test_retain()
{
    cout << "sizeof(Sub): " << sizeof(Sub) << endl;
    cout << "sizeof(RetainSub): " << sizeof(RetainSub) << endl;
    auto sub = RetainSub(new Sub(42));
    auto base = RetainBase(new Base());
    // TODO make work: base = sub;
}

int main(__attribute__((unused)) int argc,
         __attribute__((unused)) const char* argv[],
         __attribute__((unused)) const char* envp[])
{
    test_shared();
    test_retain();
    return 0;
}
