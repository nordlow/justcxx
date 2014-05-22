/*! \file t_unique_ptr.cpp
 * \brief Test C++(11) Memory Allocation using std::unique_ptr.
 */

#include <iostream>
#include <string>
#include <memory>

using std::cout;
using std::endl;

/*! Dummy Internal Allocation Class. */
class X {
public:
    X() : x(nullptr) { x = new int[2]; cout << "X()" << endl; }
    ~X() { delete[] x; cout << "~X()" << endl; }
private:
    int *x;
};

template<class T>
void test_unique_ptr()
{
    const size_t n = 2;
    std::unique_ptr<X[]> x(new X[n]);
}

int main(int argc, const char * argv[], const char * envp[])
{
    test_unique_ptr<float>();
    return 0;
}
