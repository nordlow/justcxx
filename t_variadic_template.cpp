/// $ cat q.cpp && g++ -std=gnu++0x q.cpp && ./a.out

// How would you define print() to iterate forward from 0..N,
// where N is sizeof... (Types), printing each argument X
// I'm unsure about what you mean by iterate here. AFAIK, you can't iterate
// over the elements in a parameter pack directly, everything is done with
// recursion that terminates with one or more special cases.

#include <iostream>

using std::cout;
using std::endl;

template<class... R>
void print_impl (unsigned index, const R&... values);

template<class T>
void print_impl (unsigned index, const T& value) {
    cout << index << ':' << value << '\n';
}

template<class T, class... R>
void print_impl (unsigned index, const T& value, const R&... values) {
    print_impl (index, value);
    print_impl (index + 1, values...);
}

template<class... R>
void print (const R&... values) {
    print_impl (0, values...);
}

int main(int argc, char *argv[])
{
    print (1, 'a', 3.14f, "hello");
    return 0;
}
