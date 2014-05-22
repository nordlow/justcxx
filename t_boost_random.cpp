#include <iostream>

#include "boost/random.hpp"
#include "boost/generator_iterator.hpp"
#include "rand.hpp"

using std::cout;
using std::endl;

template<typename RNG>
int test_boost_random(int count)
{
    RNG rng;
    boost::uniform_int<int> int1to6(1, 6); // Equally probable to get 1,2,3,4,5,6
    boost::variate_generator<RNG, boost::uniform_int<>> die(rng, int1to6);

    for (int i = 0; i < count; i++) {
        int n  = die();        // throw a die
        cout << n << ' ';
    }
    cout << endl;

    {
        boost::uniform_int<int> int1to6(1, 6); // Equally probable to get 1,2,3,4,5,6
        boost::variate_generator<RNG, boost::uniform_int<>> die(rng, int1to6);
        int x = rand<>(die);
    }
}

int main(int argc, char * argv[])
{
}
