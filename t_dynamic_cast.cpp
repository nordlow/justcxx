/*! \file t_dynamic_cast.cpp
 * \brief Test C++ dynamic_cast
 */

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::hex;
using std::dec;

class A {
public:
    virtual ~A() {}
protected:
};

class B : public A {
public:
    virtual ~B() {}
protected:
};

int main(int argc, const char* argv[], const char* envp[])
{
    A* b = new B();
    if (B*a = dynamic_cast<B*>(b)) {
        cout << a << endl;
    }
    if (auto a = dynamic_cast<B*>(b)) {
        cout << a << endl;
    }
    if (const auto a = dynamic_cast<B*>(b)) {
        cout << a << endl;
    }

    return 0;
}
