/*!
 * \file t_ternary_tree.cpp
 * \brief Test Ternary Search Tree Container.
 */

#include <cstddef>              // ptrdiff_t
#include <ternary_tree/structured_set.hpp>
//#include <ternary_tree/structured_map.hpp>
#include <iostream>
#include <string>

void test_ternary_tree()
{
    typedef containers::structured_set<std::string> Set;
    typedef Set::iterator SetIt;
    typedef std::pair<SetIt, SetIt> ItPair;

    Set names;
    names.insert("apps");
    names.insert("applets");
    names.insert("banana");

    //
    std::cout << "The set contains\n    ";
    for (SetIt it = begin(names); it != end(names); ++it)
        std::cout << *it << ", ";

    //
    ItPair p = names.prefix_range("app");
    std::cout << "prefix_range(\"app\") returns:\n  ";
    while (p.first != p.second) {
        std::cout << *p.first++ << ", ";
    }

    //
    std::cout << "equal_range(\"app\") returns ";
    p = names.equal_range("app");
    if (p.first == p.second)
        std::cout << "empty range\n";
    std::cout << "p.second points to " << *p.second;
}

int main(int argc, const char * argv[], const char * envp[])
{
    test_ternary_tree();
    return 0;
}
