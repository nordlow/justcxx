// Clang analyzer can discover the bug here:
// clang -c --analyze buggy.cpp

int* g(int* x)
{
    return x;
}

int* h(int* x)
{
    return x;
}

int* f(int i)
{
    int x;
    int* xp = &x;
    int* y = h(g(xp));
    return y;
}
