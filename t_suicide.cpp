#include <iostream>

class Suicidal
{
public:
    // NOTE: can only commit suicide on Suicidal objects created by new
    void commit() { delete this; }
private:
    int x;
};

void test_suicide()
{
    Suicidal * a = new Suicidal;
    a->commit();
}

int main(int argc, char *argv[])
{
    test_suicide();
    return 0;
}
