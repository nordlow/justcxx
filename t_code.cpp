#include <iostream>
#include "algorithm_x.hpp"
#include "show.hpp"
#include "show_vector.hpp"
#include <geometry/vec.hpp>
#include "enums.hpp"
#include "binary_stream.hpp"
#include "encode.hpp"
#include "decode.hpp"

// ============================================================================

using std::cout;
using std::endl;

template<class T, std::size_t N> inline void encode(std::istream& is, vec<T,N>& a) { encode_elements(is, a); }
template<class T, std::size_t N> inline void decode(std::istream& is, vec<T,N>& a) { decode_elements(is, a); }

void test_encode(std::ostream & os) {
    char   c  = -(1 << 6) + 1;
    uchar  uc =  (1 << 6) + 1;
    short  s  = -(1 << 14) + 1;
    ushort us =  (1 << 14) + 1;
    long   i  = -(1 << 30) + 1;
    ulong  ui =  (1 << 30) + 1;
    float  f  = 1.23;

    std::vector<float> iv;
    for (uint i = 0; i < 3; i++) { iv.push_back((i+1) * 10); }

    vec2<float> fv2(1.2, 3.4);

    std::string str("perno");

    std::vector<bool> bv(17);
    // 0, 1, 0, 1, 0, 1, ...
    for (uint i = 0; i < bv.size(); i++) { bv[i] = i & 1; }

    Enum256 enum256(17);

    cout << static_cast<int>(c) << ' '
         << static_cast<uint>(uc) << ' '
         << s << ' '
         << us << ' '
         << i << ' '
         << ui << ' '
         << f << ' '
         << iv << ' '
         << fv2 << ' '
         << str << ' '
         << bv << ' '
         << static_cast<uint>(enum256.get_value()) << ' '
         << std::endl;

    encode(os, c);
    encode(os, uc);
    encode(os, s);
    encode(os, us);
    encode(os, i);
    encode(os, ui);
    encode(os, f);
    encode(os, iv);
    encode(os, fv2);
    encode(os, str);
    encode(os, bv);

    encode(os, enum256);
}

void test_decode(std::istream & is) {
    char   c  = 0;
    uchar  uc = 0;
    short  s  = 0;
    ushort us = 0;
    long   i  = 0;
    ulong  ui = 0;
    float  f  = 0;
    std::vector<float> iv;
    vec2<float> fv2;
    std::string str;
    std::vector<bool> bv;

    decode(is, c);
    decode(is, uc);
    decode(is, s);
    decode(is, us);
    decode(is, i);
    decode(is, ui);
    decode(is, f);
    decode(is, iv);
    decode(is, fv2);
    decode(is, str);
    decode(is, bv);

    Enum256 enum256(is);

    cout << static_cast<int>(c) << ' '
         << static_cast<uint>(uc) << ' '
         << s << ' '
         << us << ' '
         << i << ' '
         << ui << ' '
         << f << ' '
         << iv << ' '
         << fv2 << ' '
         << str << ' '
         << bv << ' '
         << static_cast<uint>(enum256.get_value()) << ' '
         << endl;
}

void test_bin_fs() {
    bin_ofs os("data.bin"); test_encode(os); os.close();
    bin_ifs is("data.bin"); test_decode(is); is.close();
}

void test_bin_ss() {
    bin_oss os; test_encode(os);
    bin_iss is(os); test_decode(is);
}

int main(int argc, char *argv[]) {
    test_bin_ss();
    test_bin_fs();
    return 0;
}
