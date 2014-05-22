/*!
 * \file t_async.cpp
 * \brief Test C++11 async on parallel sum algorithm.
 */

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <future>
#include <unistd.h>

template<typename RAIt>
int parallel_sum(RAIt beg, RAIt end)
{
    typename RAIt::difference_type len = end-beg;
    if (len < 1000) {
        return std::accumulate(beg, end, 0);
    }
    RAIt mid = beg + len/2;
    auto handle = std::async(std::launch::async, parallel_sum<RAIt>, mid, end); // returns future
    int sum = parallel_sum(beg, mid);
    return sum + handle.get();  // get future
}

void delay(size_t i, useconds_t usec)
{
    using std::cout;
    using std::endl;
    usleep(usec);
    cout << "Sleep " << i << " done" << endl;
}

int main(int argc, const char * argv[], const char * envp[])
{
    const size_t n = 16;
    const size_t nCPUS = 4;
    const useconds_t usec = 500*1000;
    for (size_t i = 0; i != n; i++) {
        std::async(std::launch::async | std::launch::deferred, delay, i,  usec);
    }

    usleep(n * usec / nCPUS);

    std::vector<int> v(10000, 1);
    std::cout << "The sum is " << parallel_sum(v.begin(), v.end()) << '\n';

    return 0;
}
