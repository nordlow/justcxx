#include "algorithm_x.hpp"
#include "show.hpp"

#include "intersect.hpp"

#include <iostream>
#include <list>
#include <vector>

int test_intersect()
{
    int ret = 0;
    using namespace std;
    typedef double T;

    for (uint i = 0; i < 10; i++)
    {
        line2<T> la(i, 1.0,
                    i + 1, 2.0);
        line2<T> lb(1.0, i,
                    2.0, i + 2);

        vec2<T> i = intersect(la, lb);

        if (intersection_point(la, i(0)) ==
            intersection_point(la, i(1))) {
            ret = -1;
        }
    }
    return ret;
}

int main(int argc, char *argv[])
{
    return test_intersect();
}
