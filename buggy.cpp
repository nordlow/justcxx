// Clang analyzer can discover the bug here:
// clang -c --analyze buggy.cpp

int* g(int* x)
{
    return x;
}

int* h(int* x)
{
    if (*x == 3)
        return x;
}

int* f()
{
    int x = 3;
    int* xp = &x;
    int* y = g(h(xp));
    return y;
}
