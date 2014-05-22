#pragma once
#include <cstdlib>

template<class C> void shuffle(C& a)
{
    for (size_t i = 0; i < a.size(); i++) {
        std::swap(a[i], a[std::rand() % a.size()]);
    }
}
