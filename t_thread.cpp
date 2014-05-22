// \see http://blog.scalebit.com/2011/04/compiling-gcc-46-in-ubuntu-1010-for-c11.html
#include <thread>
#include <iostream>

int main(int argc, char** argv)
{
    using namespace std;
    auto t = new thread([] { cout << "Hello, World!\n"; });
    t->join();
    delete t;
    return 0;
}
