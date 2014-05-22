#include "timefmt.hpp"
#include "show.hpp"

using std::cout;
using std::cin;
using std::cerr;
using std::endl;

void test_dhms()
{
    uint d, h, m, s;
    uint x;
    cout << "enter seconds: ";
    cin >> x;
    timefmt_conv_S_to_DHMS(d, h, m, s, x);
    cout << d << ":"
	 << h << ":"
	 << m << ":"
	 << s << endl;
}

void test_mun()
{
    uint m, u, n;
    uint x;
    cout << "enter nanoseconds: ";
    cin >> x;
    timefmt_conv_N_to_MUN(m, u, n, x);
    cout << m << ":"
	 << u << ":"
	 << n << endl;
}

void test_hmsu()
{
    uint h, m, s, u;

    cout << "UTC time:   ";
    daytime_HMSU(h, m, s, u, false);
    cout << h << ":"
	 << m << ":"
	 << s << "."
	 << u << endl;

    cout << "Local time: ";
    daytime_HMSU(h, m, s, u, true);
    cout << h << ":"
	 << m << ":"
	 << s << "."
	 << u << endl;
}

int main(int argc, char ** argv)
{
    while (true)
    {
	test_dhms();
	test_mun();
	test_hmsu();
    }
    return 0;
}
