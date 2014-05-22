/*! \file t_encode.cpp
 * \brief Test Encoding/Decoding.
 */

#include <iostream>
#include <string>
#include <algorithm>
#include "show.hpp"
#include "show_dynamic_bitset.hpp"
#include "show_binary.hpp"
#include "code_vli.hpp"
#include "memory_x.hpp"

using std::cout;
using std::endl;

int main(int argc, const char * argv[], const char * envp[]) {
    std::vector<int> x;
    //encode(std::cout, x);

    for (uint8_t i = 0; i < 100; i++) {
        cout << "i:"; show_binary(i);
        cout << " lsb:" << pnw::lsb(i+1)
             << " msb:" << pnw::msb(i+1) << endl;

        std::tr2::dynamic_bitset<size_t> bs;

        lvli_encode(bs, i);
        cout << "i=" << i << " bs: " << bs << endl;

        size_t i_;
        lvli_decode(bs, i_);
        cout << "i_=" << i_ << " bs: " << bs << endl;
    }

    for (size_t i = 0; i < 4; i++) {
        std::tr2::dynamic_bitset<size_t> bs;
        bs << static_cast<uint8_t>(i)
           << static_cast<uint16_t>(i)
           << static_cast<uint32_t>(i)
           << static_cast<uint64_t>(i)
            ;
        cout << "bs: " << bs << endl;
    }

    return 0;
}
