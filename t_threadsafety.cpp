#include "atomic_queue.hpp"
#include "threadpool/boost/threadpool.hpp"
#include <boost/lockfree/fifo.hpp>
#include <iostream>

// using namespace boost::threadpool;

using std::cout;
using std::endl;

template<typename T>
void produce(atomic_queue<T> & q, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        T x = i;
        q.push(x);
        cout << "i:" << i << " produced: " << x << endl;
    }
}

template<typename T>
void consume(atomic_queue<T> & q, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        T x = q.wait_pop();
        cout << "i:" << i << " consumed: " << x << endl;
    }
}

int foo(int x)
{
    return x;
}

int main(int argc, char *argv[])
{
    typedef float T;
    atomic_queue<T> q;
    boost::thread foo_thread(foo, 13);
    boost::thread prod_thread(produce<T>, boost::ref(q), 10);
    boost::thread cons_thread(consume<T>, boost::ref(q), 10);
    prod_thread.join();
    cons_thread.join();
    return 0;
}
