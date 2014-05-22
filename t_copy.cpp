#include "algorithm_x.hpp"
#include "show.hpp"

#include <iostream>
#include <list>
#include <vector>

void test_copy()
{
    using namespace std;

    vector<int> b;
    b.push_back(13);
    b.push_back(17);
    cout << b << endl;

    list<int> a;

    pnw::copy(a, b);
    cout << a << endl;

    a.push_back(19);

    pnw::copy(b, a);

    cout << b << endl;
}

int main(int argc, char *argv[])
{
    test_copy();
    return 0;
}
