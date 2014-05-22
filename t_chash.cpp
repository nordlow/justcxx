#include <chrono>
#include <array>
#include "chash.hpp"
#include "enforce.hpp"
#include "timed.hpp"
#include "show_all.hpp"

typedef std::chrono::high_resolution_clock hrc;

/*! Show Benchmark Result. */
template<class Time, class T, size_t N>
void view(std::ostream& os,
          const char* name,
          const std::array<T,N>& dig,
          Time duration)
{
    auto count = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    os << name << " took " << count << " milliseconds" << std::endl;
}

void test_chash(bool show = false, size_t data_length = 4*1024*1024)
{
    typedef uint8_t T;
    using std::cout;
    using std::endl;
    using std::array;
    using namespace chash;

    T mdat[data_length]; const size_t mlen = sizeof(mdat); // 4 Megabytes of data
    auto fct = 111;                                         // fill constant
    pnw::timed tm; tm.m_n_tries = 5;

    // \todo: Call tests in async

    // SHA-1
    {
        array<T,SHA1_DIGEST_SIZE> dA; dA.fill(fct);
        view(cout, "SHA1-160 Nettle ",       dA, tm.call([&mdat, mlen, &dA]() { nettle::SHA1_160 ch(mdat, mlen); ch.final(dA.data()); }));
        array<T,SHA1_DIGEST_SIZE> dB; dB.fill(fct);
        view(cout, "SHA1-160 OpenSSL (EVP)", dB, tm.call([&mdat, mlen, &dB]() { openssl::CHash ch(mdat, mlen); ch.final(dB.data()); }));
        array<T,SHA1_DIGEST_SIZE> dC; dC.fill(fct);
        view(cout, "SHA1-160 OpenSSL",       dC, tm.call([&mdat, mlen, &dC]() { openssl::SHA1_160 ch_ssl(mdat, mlen); ch_ssl.final(dC.data()); }));
        enforce_eq3(dA, dB, dC);
    }
    cout << endl;

    // SHA-224
    {
        array<T,SHA224_DIGEST_SIZE> dA; dA.fill(fct);
        if (show) { view(cout, "SHA2-224 Nettle ",  dA, tm.call([&mdat, mlen, &dA]() { nettle::SHA2_224 ch(mdat, mlen); ch.final(dA.data());})); }
        array<T,SHA224_DIGEST_SIZE> dB; dB.fill(fct);
        if (show) { view(cout, "SHA2-224 OpenSSL ", dB, tm.call([&mdat, mlen, &dB]() { openssl::SHA2_224 ch(mdat, mlen); ch.final(dB.data()); })); }
        array<T,SHA224_DIGEST_SIZE> dC; dC.fill(fct);
        if (show) { view(cout, "SHA3-224 Nettle ",  dC, tm.call([&mdat, mlen, &dC]() { nettle::SHA3_224 ch(mdat, mlen); ch.final(dC.data()); })); }
        enforce_eq(dA, dB);
    }
    cout << endl;

    // SHA-256
    {
        array<T,SHA256_DIGEST_SIZE> dA; dA.fill(fct);
        if (show) { view(cout, "SHA2-256 Nettle ",  dA, tm.call([&mdat, mlen, &dA]() { nettle::SHA2_256 ch(mdat, mlen); ch.final(dA.data());})); }
        array<T,SHA256_DIGEST_SIZE> dB; dB.fill(fct);
        if (show) { view(cout, "SHA2-256 OpenSSL ",  dB, tm.call([&mdat, mlen, &dB]() { openssl::SHA2_256 ch(mdat, mlen); ch.final(dB.data());})); }
        array<T,SHA256_DIGEST_SIZE> dC; dC.fill(fct);
        if (show) { view(cout, "SHA3-256 OpenSSL ",  dC, tm.call([&mdat, mlen, &dC]() { nettle::SHA3_256 ch(mdat, mlen); ch.final(dC.data());})); }
        enforce_eq(dA, dB);
    }
    cout << endl;

    // SHA-384
    {
        array<T,SHA384_DIGEST_SIZE> dA; dA.fill(fct);
        if (show) { view(cout, "SHA2-384 Nettle ",  dA, tm.call([&mdat, mlen, &dA]() { nettle::SHA2_384 ch(mdat, mlen); ch.final(dA.data());})); }
        array<T,SHA384_DIGEST_SIZE> dB; dB.fill(fct);
        if (show) { view(cout, "SHA2-384 OpenSSL ",  dB, tm.call([&mdat, mlen, &dB]() { openssl::SHA2_384 ch(mdat, mlen); ch.final(dB.data());})); }
        array<T,SHA384_DIGEST_SIZE> dC; dC.fill(fct);
        if (show) { view(cout, "SHA3-384 OpenSSL ",  dC, tm.call([&mdat, mlen, &dC]() { nettle::SHA3_384 ch(mdat, mlen); ch.final(dC.data());})); }
        enforce_eq(dA, dB);
    }
    cout << endl;

    // SHA-512
    {
        array<T,SHA512_DIGEST_SIZE> dA; dA.fill(fct);
        if (show) { view(cout, "SHA2-512 Nettle ",  dA, tm.call([&mdat, mlen, &dA]() { nettle::SHA2_512 ch(mdat, mlen); ch.final(dA.data());})); }
        array<T,SHA512_DIGEST_SIZE> dB; dB.fill(fct);
        if (show) { view(cout, "SHA2-512 OpenSSL ",  dB, tm.call([&mdat, mlen, &dB]() { openssl::SHA2_512 ch(mdat, mlen); ch.final(dB.data());})); }
        array<T,SHA512_DIGEST_SIZE> dC; dC.fill(fct);
        if (show) { view(cout, "SHA3-512 OpenSSL ",  dC, tm.call([&mdat, mlen, &dC]() { nettle::SHA3_512 ch(mdat, mlen); ch.final(dC.data());})); }
        enforce_eq(dA, dB);
    }
    cout << endl;
}

int main(int argc, char* argv[])
{
    test_chash(true);
    return 0;
}
