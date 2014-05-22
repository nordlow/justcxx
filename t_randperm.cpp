#include "algorithm_x.hpp"
#include "show.hpp"
#include "randperm.hpp"

using std::cout;
using std::endl;

void test_random_permutation()
{
    std::vector<int> a(32);
    randperm(a);
    cout << a << endl;
}

int main(int argc, char *argv[])
{
    test_random_permutation();
}
