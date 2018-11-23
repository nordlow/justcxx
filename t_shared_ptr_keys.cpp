#include <memory>
#include <unordered_set>
#include <iostream>
#include <cassert>

using namespace std;

using T = size_t;
using SharedT = std::shared_ptr<const T>;

SharedT makeT(int x)
{
    return std::make_shared<T>(x);
}

inline bool operator == (const SharedT &a,
                         const T *b)
{
    return (a.get() == b);
}

inline bool operator == (const T *a,
                         const SharedT &b)
{
    return (a == b.get());
}

int main(int argc, const char *argv[], const char *envp[])
{
    T value = 42;
    const auto x = makeT(value);
    const auto first = (reinterpret_cast<const size_t *const*>(&x))[0];
    const auto second = (reinterpret_cast<const size_t *const*>(&x))[1];
    assert(*first == value);    // value first
    cout << *second << endl;

    std::unordered_set<SharedT> up;

    SharedT si = nullptr;
    const T *ip = nullptr;

    const bool ok1 = (si == ip);
    const bool ok2 = (ip == si);

    auto it1 = up.find(si);

    size_t faked_sharedT[sizeof(SharedT) / 8] = {0, 0};

    auto it2 = up.find(*reinterpret_cast<const SharedT*>(&faked_sharedT));     // TODO make compile

    return 0;
}
