#include <iostream>
#include <string>
#include <tuple>

template<class T, std::size_t N>
struct TuplePrinter{
    static void print(std::ostream& os, const T& t){
        TuplePrinter<T,N-1>::print(os,t);
        os << ", " << std::get<N-1>(t);
    }
};
template<class T>
struct TuplePrinter<T, 1>{
    static void print(std::ostream& os, const T& t){
        os << std::get<0>(t);
    }
};
// template<class... Args>
// std::ostream& operator<<(std::ostream& os, const std::tuple<Args...>& t){
//     typedef const std::tuple<Args...>& tuple_t;
//     static const int num = sizeof...(Args);
//     os << "(";
//     TuplePrinter<tuple_t,num>::print(os,t);
//     return os << ")";
// }

template<class T, std::size_t N> inline void print(std::ostream& os, const T& t) {
    TuplePrinter<T,N-1>::print(os,t);
    os << ", " << std::get<N-1>(t);
}
template<class T> inline void print(std::ostream& os, const T& t) {
    os << std::get<0>(t);
}
template<class... Args>
std::ostream& operator<<(std::ostream& os, const std::tuple<Args...>& t){
    typedef const std::tuple<Args...>& tuple_t;
    static const int num = sizeof...(Args);
    os << "(";
    print<tuple_t,num>(os,t);
    return os << ")";
}

#if 0
// Delimiters for tuple
template<class... Args>
struct delimiters<std::tuple<class... Args>, char> {
    static const delimiters_values<char> values;
};
template<class... Args>
const delimiters_values<char> delimiters<std::tuple<class... Args>, char>::values = { "(", ", ", ")" };
template<class... Args>
struct delimiters<std::tuple<class... Args>, wchar_t> {
    static const delimiters_values<wchar_t> values;
};
template<class... Args>
const delimiters_values<wchar_t> delimiters<std::tuple<class... Args>, wchar_t>::values = { L"(", L", ", L")" };
template<class... Args>
std::ostream& operator<<(std::ostream& os, const std::tuple<Args...>& t)
{
    typedef const std::tuple<Args...>& tuple_t;
    static const int num = sizeof...(Args);
    if(delimiters<tuple_t,char>::values.prefix != 0)
        os << delimiters<tuple_t,char>::values.prefix;
    TuplePrinter<tuple_t,num>::print(os,t);
    if(delimiters<tuple_t,char>::values.postfix != 0)
        os << delimiters<tuple_t,char>::values.postfix;
    return os;
}
template<class Tuple, std::size_t N>
struct TuplePrinter{
    static void print(std::ostream& os, const Tuple& t){
        TuplePrinter<Tuple,N-1>::print(os,t);
        if(delimiters<tuple_t,char>::values.delimiter != 0)
            os << delimiters<tuple_t,char>::values.delimiter;
        os << std::get<N-1>(t);
        return os;
    }
};
#endif

int main(int argc, char * argv[], char * envp[])
{
    auto a = std::make_tuple(5, "Hello", -0.1);
    std::cout << a << std::endl; // prints: (5, "Hello", -0.1)
    return 0;
}
