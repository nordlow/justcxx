// see: http://stackoverflow.com/questions/1647895/what-does-static-assert-do-and-what-would-you-use-it-for

#include <iostream>
#include <string>

// #include <limits>

// template<typename T, typename U>
// struct Type
// {
//     static_assert(boost::is_base_of<T, Interface>::value);
//     static_assert(std::numeric_limits<U>::is_integer);
// };

// template<typename T>
// struct TypeOfInt
// {
//     BOOST_STATIC_ASSERT(std::numeric_limits<T>::is_integer);
// };

// TypeOfInt toi;

int main(int argc, char * argv[])
{
    const int x = 3;
    static_assert(x > 4, "x is too small!");
    return 0;
}
