/*!
 * \file t_array.cpp
 * \brief Tests various aspects of using the vector-like template classes.
 */

#include "aligned_vector.hpp"
#include "multi_resolution.hpp"
#include <iostream>
#include <vector>
#include <iomanip>

void show(std::vector<float> v,
          pnw::aligned_vector<float, 8> av8,
          pnw::aligned_vector<float, 16> av16)
{
    using std::cout;
    using std::endl;
    cout << "v.data(): " << v.data() << endl;
    cout << "av8.data(): " << av8.data() << endl;
    cout << "av16.data(): " << av16.data() << endl;
}

int main(int argc, char * argv[])
{
    using std::cout;
    using std::endl;

    typedef float T;
    const T a = 11;
    const T b = 143;

    size_t n = 31;
    if (argc == 1+0) {
        cout << "usage: " << argv[0] << endl;
    }
    std::vector<float> v(n, a); v.push_back(b);
    pnw::aligned_vector<float, 8> av8(n,a); av8.push_back(b);
    pnw::aligned_vector<float, 16> av16(n,a); av16.push_back(b);

    show(v, av8, av16);
    pnw::multi_resolution::vector<float> mrv(n, a); mrv.push_back(b);
    cout << mrv << "mrv.data(): " << mrv.data() << endl;
}
