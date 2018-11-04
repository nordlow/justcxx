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
        cout << "dtor" << endl;
    }
public:
    int x;
    mutable unsigned int _refcount = 1;
};

struct S_traits final {
    static void increment (S* x) noexcept { x->_refcount += 1; }
    static void decrement (S* x) noexcept { x->_refcount -= 1; }
};

#include "sg14/memory.hpp"
using RP = sg14::retain_ptr<S, S_traits>;

int main(__attribute__((unused)) int argc,
         __attribute__((unused)) const char * argv[],
         __attribute__((unused)) const char * envp[])
{
    cout << "sizeof(S): " << sizeof(S) << endl;
    cout << "sizeof(RP): " << sizeof(RP) << endl;
    auto rp = RP(new S(42));
    return 0;
}
