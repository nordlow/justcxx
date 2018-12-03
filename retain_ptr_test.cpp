#include <iostream>
#include <memory>
#include <iostream>
#include "sg14/memory.hpp"

using namespace std;

class Base
{
public:
    Base()
    {
    }
    virtual ~Base() = default;  // must be here
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
    using SharedBase = std::shared_ptr<const Base>;
    using SharedSub = std::shared_ptr<const Sub>;
    SharedBase base;
    SharedSub sub;
    base = sub;                 // compiles!
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

void test_retain_assign_from_rvalue()
{
    auto base = RetainBase(new Base());
    base = RetainSub(new Sub(42));
    cout << "done assign" << endl;
}

RetainBase test_return_subclass()
{
    return RetainSub(new Sub(42));
}

int main(__attribute__((unused)) int argc,
         __attribute__((unused)) const char* argv[],
         __attribute__((unused)) const char* envp[])
{
    cout << "sizeof(Sub): " << sizeof(Sub) << endl;
    cout << "sizeof(RetainSub): " << sizeof(RetainSub) << endl;
    test_shared();
    test_retain_assign_from_rvalue();
    test_return_subclass();
    return 0;
}
