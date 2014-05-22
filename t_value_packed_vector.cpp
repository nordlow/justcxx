#include "value_packed_vector.hpp"

template<typename T>
void test_value_packed_vector(void)
{
    value_packed_vector<T> a;

    std::vector<T> d;

    for (uint i = 0; i < 256; i++) {
	d.push_back(i);
    }
    a.push_back(d);
    a.clear();
    d.clear();

    for (uint i = 0; i < 65536; i++) {
	a.push_back(i);
    }
    a.push_back(d);
    a.clear();
    d.clear();

    for (uint i = 0; i < 100000; i++) {
	a.push_back(i);
    }
    a.push_back(d);
    a.clear();
    d.clear();
}

int main(int argc, char *argv[])
{
    test_value_packed_vector<uint64_t>();
    return 0;
}
