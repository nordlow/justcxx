#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>

using std::cout;
using std::endl;
using std::ios;

/// Prime Generator Single Threaded.
template<typename T>
class Primes_Single
{
public:
    void generate(const tbb::blocked_range<T> & r)
    {
        bool is_prime = false;
        for (T x = begin(r); x < end(r); x += 2) {
            is_prime = true;

            for (T i = 3; i <= std::sqrt((double) x); i += 2) {
                if (x % i == 0) {
                    is_prime = false;
                    continue;
                }
            }
            if (is_prime) { m_primes.push_back(x); } // output
        }
        cout << "Found " << m_primes.size() << " number of primes" << endl;
    }
protected:
    std::vector<T> m_primes;    ///< Gathered Primes.
};

/// Prime Generator TBB.
template<typename T>
class Prime_TBB
{
public:
    void test_prime(T num) const
    {
        bool is_prime = true;
        for (T i = 3; i <= std::sqrt((double) num); i += 2) {
            if (num % i == 0) {
                is_prime = false;
                continue;
            }
        }
        if (is_prime) { cout << num << (m_primes.size() % 10 == 0 ? endl : ','); } // output
    }
    void operator() (const tbb::blocked_range<T> & r) const
    {
        for (T i = begin(r); i < end(r); i += 2)
        {
            test_prime(i);
        }
    }
    void generate(tbb::blocked_range<T> r)
    {
        Prime_TBB prime_tbb;
        parallel_for(r, prime_tbb);
    }
protected:
    std::vector<T> m_primes;    ///< Gathered Primes.
};

int main()
{
    tbb::task_scheduler_init init;
    static tbb::tick_count tB, tE;   // tick begin and end

    typedef size_t T;
    Primes_Single<T> p1;
    Prime_TBB<T> p2;

    cout.setf(ios::fixed);
    cout.setf(ios::showpoint);
    cout.precision(2);

    const T gran = 100;               // granularity

    static const int limit = 1000000;

    tbb::blocked_range<T> r(3, limit, gran);
    tB = tbb::tick_count::now();
    p1.generate(r);
    tE = tbb::tick_count::now();
    cout << (tE - tB).seconds() * gran << " ms" << endl;

    tB = tbb::tick_count::now();
    p2.generate(r);
    tE = tbb::tick_count::now();
    cout << (tE - tB).seconds() * gran << " ms" << endl;

    return 0;
}
