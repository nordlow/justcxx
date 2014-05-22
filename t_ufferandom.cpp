#include <cstdio>
#include <cstdlib>
#include <climits>
#include <algorithm>

void slump(int a, int b)
{
    int m = 4, n = 16;
    int c = b - a;
    int min = INT_MAX, max = INT_MIN;
    for (int i = 0; i < m; i++)
    {
	for (int j = 0; j < n; j++)
	{
	    int x = a + rand() % (c+1);
	    printf("%04x,", x);
	    min = std::min(min, x);
	    max = std::max(max, x);
	}
	printf("\n");
    }
    printf("min:%d max:%d\n", min, max);
}

int main(int argc, char *argv[])
{
    slump(8800, 12399);
    printf("\n");
    slump(24000, 28399);
    return 0;
}
