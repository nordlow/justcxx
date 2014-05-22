#include "algorithm_x.hpp"
#include "show.hpp"

using std::cout;
using std::endl;

void test_byte()
{
    const uint n = 4;
    uint x = 0x12345678, y = 0x87654321;

    for (uint i = 0; i < n; i++) {
        pnw::set_byte(x, i, 10 + i);
        pnw::copy_byte(y, n-1-i, x, i);
    }

    for (uint i = 0; i < n; i++) {
        cout << "x byte " << i << ": "
             << static_cast<int>(pnw::get_byte(x, i)) << " and "
             << "y byte " << i << ": "
             << static_cast<int>(pnw::get_byte(y, i)) << endl;
    }
}

int main(int argc, char *argv[])
{
    test_byte();
    return 0;
}
