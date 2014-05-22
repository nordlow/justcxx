#include "sigmoids.hpp"
#include "show.hpp"

#include <cstdlib>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

int main(int argc, char ** argv)
{
    cout << logistic_function(3) << endl;
    return 0;
}
