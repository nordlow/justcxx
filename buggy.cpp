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
