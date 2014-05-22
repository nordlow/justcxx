#include <vector>
#include <iostream>

#include "box.hpp"
#include "unite.hpp"
#include "show.hpp"

using std::cout;
using std::endl;

typedef std::vector<int> Vec;

void test_unite()
{
    box2<int> a(0,0,1,1), b(1,1,2,2);
    cout << a << endl;
    cout << b << endl;
    cout << unite(a,b) << endl;
}

void test_ewise()
{
    vec2<int> a(1,2), b(2,3);

    cout << a << endl;
    cout << b << endl;

    vec2<int> c = min(a, b);
    vec2<int> d = max(a, b);

    cout << c << endl;
    cout << d << endl;
}

int main(int argc, char *argv[])
{
    test_unite();
    test_ewise();
    return 0;
}
