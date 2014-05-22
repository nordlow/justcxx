#include "algorithm_x.hpp"
#include "show.hpp"
#include "Timer.hpp"
#include "ackermann.hpp"

#include <iostream>

using std::cout;
using std::cin;
using std::endl;

template<typename T>
void test_ackermann(uint m = 3, uint n = 4)
{
    for (uint i = 0; i < m; i++)
    {
        for (uint j = 0; j < n; j++)
        {
            T a = i, b = j;
            Timer t;
            T c = ackermann(a, b);
            t.stop();
            cout << "ackermann(" << a << ", " << b << ") = " << c
                 << " took " << t << endl;
        }
        cout << endl;
    }
}

int main(int argc, char *argv[])
{
    while (true)
    {
	uint m, n;

	cout << "m: "; cin >> m;
	cout << "n: "; cin >> n;

	if (m == 0 or n == 0)
	{
	    break;
	}

	test_ackermann<double>(m, n);
    }
    return 0;
}
