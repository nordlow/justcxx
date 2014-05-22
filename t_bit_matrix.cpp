#include "algorithm_x.hpp"
#include "hamming_distance.hpp"
#include "show.hpp"
#include "bit_matrix.hpp"

using std::cout;
using std::endl;

void test_bit_matrix()
{
    uint m = 4, n = 2;
    bit_matrix a(m, n);
    bit_matrix b(m, n);

    a.rand();
    b.rand();
    cout << "a: " << a << endl;
    cout << "b: " << b << endl;

    bit_matrix c;
    bit_xor(c, a, b);
    cout << "c: " << c << endl;

    cout << "hamming_distance: " << pnw::hamming_distance(a, b) << endl;
}

int main(int argc, char *argv[])
{
    while (true)
    {
        test_bit_matrix();
    }
}
