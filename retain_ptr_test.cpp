class S
{
    S(int x_) : x(x_) {}
public:
    int x;
};

#include "sg14/memory.hpp"
using RP = sg14::retain_ptr<S>;

// RP f()
// {
//     return RP(nullptr);
// }

#include <iostream>
#include <string>

using std::cout;
using std::endl;

int main(__attribute__((unused)) int argc,
         __attribute__((unused)) const char * argv[],
         __attribute__((unused)) const char * envp[])
{
    cout << sizeof(S) << endl;
    cout << sizeof(RP) << endl;
    return 0;
}
