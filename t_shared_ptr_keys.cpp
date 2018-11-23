#include <memory>
#include <unordered_map>

using Key = int;
using SharedKey = std::shared_ptr<const Key>;

inline bool operator == (const SharedKey &a,
                         const Key *b)
{
    return (a.get() == b);
}

inline bool operator == (const Key *a,
                         const SharedKey &b)
{
    return (a == b.get());
}

int main(int argc, const char *argv[], const char *envp[])
{
    std::unordered_map<SharedKey, double> up;

    SharedKey si = nullptr;
    const Key *ip = nullptr;

    const bool ok1 = (si == ip);
    const bool ok2 = (ip == si);

    auto it1 = up.find(si);

    auto it2 = up.find(ip);     // TODO make compile

    return 0;
}
