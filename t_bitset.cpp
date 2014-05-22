#include <iostream>
#include <bitset>

using std::cout;
using std::cin;
using std::endl;

template<size_t n = 32>
void test_bitset_1()
{
    std::bitset<n> x;
    cout << "size of x: " << sizeof(x) << endl;
}

template<size_t n = 32>
void test_bitset_2()
{
    const std::bitset<n> mask(2730ul);
    cout << "mask =      " << mask << endl;

    std::bitset<n> x;

    cout << "Enter a 12-bit bitset in binary: ";
    cout.flush();
    if (cin >> x) {
        cout << "x =        " << x << endl;
        cout << "As ulong:  " << x.to_ulong() << endl;
        cout << "And with mask: " << (x & mask) << endl;
        cout << "Or with mask:  " << (x | mask) << endl;
    }
}

int main(int argc, char *argv[])
{
    test_bitset_1<8>();
    test_bitset_1<16>();
    test_bitset_1<24>();
    test_bitset_1<32>();
    test_bitset_1<40>();
    test_bitset_1<48>();
    test_bitset_1<56>();
    test_bitset_1<64>();

    test_bitset_2<12>();
}
