#include <memory>
#include <unordered_set>
#include <iostream>
#include <cassert>

using namespace std;

using T = size_t;
using SCT = shared_ptr<const T>; // shared const T

inline bool operator == (const SCT &a, const T *b)
{
    return (a.get() == b);
}
inline bool operator == (const T *a, const SCT &b)
{
    return (a == b.get());
}

void test()
{
    SCT si = nullptr;
    const T *ip = nullptr;
    const bool ok1 = (si == ip);
    const bool ok2 = (ip == si);
}


int main(int argc, const char *argv[], const char *envp[])
{
    T value = 42;
    const auto x = make_shared<T>(value);
    const auto value_heap_ptr = (reinterpret_cast<const size_t *const*>(&x))[0];
    const auto counter_ptr = (reinterpret_cast<const size_t *const*>(&x))[1];
    assert(*value_heap_ptr == value);

    unordered_set<SCT> up;
    // auto it1 = up.find(si);
    // size_t faked_sharedT[sizeof(SCT) / 8] = {0, 0};
    // auto it2 = up.find(*reinterpret_cast<const SCT*>(&faked_sharedT));     // TODO make compile

    return 0;
}
