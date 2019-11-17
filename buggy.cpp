// Clang analyzer can discover the bug here:
// clang -c --analyze buggy.cpp

int* g(int* x)
{
    return x;
}

int* f()
{
  int x;
  int* xp = &x;
  return g(xp);
}
