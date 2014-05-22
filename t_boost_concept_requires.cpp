#include <iostream>
#include <string>
#include <algorithm>
#include "boost/concept/requires.hpp"

template<typename I>
BOOST_CONCEPT_REQUIRES(
    ((Mutable_RandomAccessIterator<I>))
    ((LessThanComparable<typename Mutable_RandomAccessIterator<I>::value_type>)),
    (void)) // return to_base_type
    sort(I begin, I end)
{
    std::sort(begin, end);
}
int main(int argc, char * argv[], char * envp[])
    int a[] = { 1, 4, 3, 2, 6, 5 };
    sort(a, a + 6);
    return 0;
}
