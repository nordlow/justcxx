/*! \file chash.hpp
 * \brief C++ Wrapper Classes for OpenSSL and Nettle Cryptographic Hash Functions.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2009-04-21 07:29
 *
 * \note Copy Constructor (Ctor) are useful if large amounts of data are to
 * be hashed which only differ in the last few bytes.
 */

#pragma once
#include <cstring>
#include <algorithm>
#include <array>

#ifdef HAVE_OPENSSL_MD5_H
#  include <openssl/md5.h>
#endif
#ifdef HAVE_OPENSSL_SHA_H
#  include <openssl/sha.h>
#endif
#ifdef HAVE_NETTLE_MD5_H
#  include <nettle/md5.h>
#endif
#ifdef HAVE_NETTLE_SHA_H
#  include <nettle/sha.h>
#endif
#ifdef HAVE_NETTLE_SHA2_H
#  include <nettle/sha2.h>
#endif
#ifdef HAVE_NETTLE_SHA3_H
#  include <nettle/sha3.h>
#endif

#ifndef MD5_DIGEST_SIZE
#  define MD5_DIGEST_SIZE (16)
#endif
#ifndef SHA1_DIGEST_SIZE
#  define SHA1_DIGEST_SIZE (20)
#endif
#ifndef SHA224_DIGEST_SIZE
#  define SHA224_DIGEST_SIZE (28)
#endif
#ifndef SHA256_DIGEST_SIZE
#  define SHA256_DIGEST_SIZE (32)
#endif
#ifndef SHA384_DIGEST_SIZE
#define SHA384_DIGEST_SIZE (48)
#endif
#ifndef SHA512_DIGEST_SIZE
#  define SHA512_DIGEST_SIZE (64)
#endif

namespace chash {

typedef enum {
    CHASH_MD5,
    CHASH_SHA1_160,

    CHASH_SHA2_224,
    CHASH_SHA2_256,
    CHASH_SHA2_384,
    CHASH_SHA2_512,

    CHASH_SHA3_224,
    CHASH_SHA3_256,
    CHASH_SHA3_384,
    CHASH_SHA3_512,

    CHASH_undefined_,
} chashid;

uint32_t    chashid_get_digest_size(chashid hid);
const char* chashid_get_name_static(chashid hid);

/*! Determine Hash \em Type based on \em Content Size \p csize. */
chashid chashid_from_size(uint32_t csize);

// Forward Declarations
namespace nettle {
class CHash_MD5;
class SHA1_160;
class SHA2_224;
class SHA2_256;
class SHA2_384;
class SHA2_512;
class SHA3_224;
class SHA3_256;
class SHA3_384;
class SHA3_512;
}
namespace openssl {
class SHA1_160;
class SHA2_224;
class SHA2_256;
class SHA2_384;
class SHA2_512;
}
class CDigest160;
class CDigest256;
class CDigest384;
class CDigest512;

/*! Hash Digest of length \p N bytes.
 * \todo: Alignment support for array.
 */
template<std::size_t N>
class CDigest : public std::array<unsigned char,N>
{
public:
    /// Default Constructor.
    // TODO: C++11 inherit constuctor: CDigest() { std::fill(data(), data() + size(), 0); }
    /// Construct directly from Digest \p dig.
    CDigest() { std::fill_n(this->data(), N, 0); }
    CDigest(const unsigned char dig[N]) { set_data(dig); }
protected:
    /// Set Digest Data Byte Array to \p dig.
    void set_data(const unsigned char dig[N]) { std::copy(dig, dig+N, this->data()); }
};

#if 0
template<std::size_t N, std::size_t alignment = 16>
class CDigest_Old
{
public:
    /// Default Constructor.
    CDigest_Old() { std::fill_n(m_dig, N, 0); }
    /// Construct directly from Digest \p dig.
    CDigest_Old(const unsigned char dig[N]) { set_data(dig); }
    /// Destructor.
    ~CDigest_Old() {}
    /// Get Digest.
    const unsigned char * data() const { return m_dig; }
    /// Get Digest Size in bytes.
    size_t size() const { return N; }
protected:
    /// Set Digest Data Byte Array to \p dig.
    void set_data(const unsigned char dig[N]) { std::copy(dig, dig+N, m_dig); }
    /// Get Digest.
    unsigned char * data() { return m_dig; }
private:
    unsigned char m_dig[N] __attribute__((aligned(alignment)));
};
#endif

/// TODO: Construct Directly from Finalization of \c SHA1_N.
// CDigest() { std::fill(m_dig, m_dig + N, 0); }

/*! Hash Digest SHA-1. */
class CDigest160 : public CDigest<SHA1_DIGEST_SIZE>
{
    friend class nettle::SHA1_160;
    friend class openssl::SHA1_160;
public:
    // TODO: Use C++11 Inheriting Constructor when available in GCC: using CDigest<SHA1_DIGEST_SIZE>::CDigest<SHA1_DIGEST_SIZE>;
    /// Construct directly from Digest \p dig.
    CDigest160(const unsigned char dig[SHA1_DIGEST_SIZE]) : CDigest<SHA1_DIGEST_SIZE>(dig) {}
    CDigest160(nettle::SHA1_160& chash);
    CDigest160(openssl::SHA1_160& chash);
};

/*! Hash Digest SHA-224. */
class CDigest224 : public CDigest<SHA224_DIGEST_SIZE>
{
    friend class nettle::SHA2_224;
    friend class nettle::SHA3_224;
    friend class openssl::SHA2_224;
public:
    // TODO: Use C++11 Inheriting Constructor when available in GCC: using CDigest<SHA1_DIGEST_SIZE>::CDigest<SHA1_DIGEST_SIZE>;
    /// Construct directly from Digest \p dig.
    CDigest224(const unsigned char dig[SHA224_DIGEST_SIZE]) : CDigest<SHA224_DIGEST_SIZE>(dig) {}
    CDigest224(nettle::SHA2_224& chash);
    CDigest224(openssl::SHA2_224& chash);
};

/*! Hash Digest SHA-256. */
class CDigest256 : public CDigest<SHA256_DIGEST_SIZE>
{
    friend class nettle::SHA2_256;
    friend class nettle::SHA3_256;
    friend class openssl::SHA2_256;
public:
    // TODO: Use C++11 Inheriting Constructor when available in GCC: using CDigest<SHA1_DIGEST_SIZE>::CDigest<SHA1_DIGEST_SIZE>;
    /// Construct directly from Digest \p dig.
    CDigest256(const unsigned char dig[SHA256_DIGEST_SIZE]) : CDigest<SHA256_DIGEST_SIZE>(dig) {}
    CDigest256(nettle::SHA2_256& chash);
    CDigest256(openssl::SHA2_256& chash);
};

/*! Hash Digest SHA-384. */
class CDigest384 : public CDigest<SHA384_DIGEST_SIZE>
{
    friend class nettle::SHA2_384;
    friend class nettle::SHA3_384;
    friend class openssl::SHA2_384;
public:
    // TODO: Use C++11 Inheriting Constructor when available in GCC: using CDigest<SHA1_DIGEST_SIZE>::CDigest<SHA1_DIGEST_SIZE>;
    /// Construct directly from Digest \p dig.
    CDigest384(const unsigned char dig[SHA384_DIGEST_SIZE]) : CDigest<SHA384_DIGEST_SIZE>(dig) {}
    CDigest384(nettle::SHA2_384& chash);
    CDigest384(openssl::SHA2_384& chash);
};

/*! Hash Digest SHA-512. */
class CDigest512 : public CDigest<SHA512_DIGEST_SIZE>
{
    friend class nettle::SHA2_512;
    friend class nettle::SHA3_512;
    friend class openssl::SHA2_512;
public:
    // TODO: Use C++11 Inheriting Constructor when available in GCC: using CDigest<SHA1_DIGEST_SIZE>::CDigest<SHA1_DIGEST_SIZE>;
    /// Construct directly from Digest \p dig.
    CDigest512(const unsigned char dig[SHA512_DIGEST_SIZE]) : CDigest<SHA512_DIGEST_SIZE>(dig) {}
    CDigest512(openssl::SHA2_512& chash);
};

namespace nettle {

#if HAVE_NETTLE_MD5_H
/*!
 * MD5
 * Wraps Nettle's (libnettle) MD5 interface.
 */
class CHash_MD5
{
public:
    /// Default Ctor.
    CHash_MD5() { init(); }
    /// Construct and digest \p data of size \p len.
    CHash_MD5(const uint8_t* data, size_t len) { init(); update(data, len); }
    /// Construct and digest C-String \p cstr.
    CHash_MD5(const char* cstr) { init(); update((uint8_t*)cstr, strlen(cstr)); }
    /// Copy Ctor.
    CHash_MD5(const CHash_MD5& in) : m_ctx(in.m_ctx) { } // just copy the internal state
    /// Update \p data of size \p len.
    void update(const uint8_t* data, size_t len) { md5_update(&m_ctx, len, reinterpret_cast<const uint8_t*>(data)); }
    /// Final(ize) into MD5 Message Digest \p md of length \c MD5_DIGEST_SIZE bytes.
    void final(uint8_t md[MD5_DIGEST_SIZE]) { md5_digest(&m_ctx, MD5_DIGEST_SIZE, md); }
    /// Dtor.
    ~CHash_MD5() {}
protected:
    /// Initialize/Reset State.
    void init() { md5_init(&m_ctx); }
private:
    struct md5_ctx m_ctx;    /// Internal Context.
};
#endif

#if HAVE_NETTLE_SHA_H
/*! SHA-1.
 * Wraps Nettle's (libnettle) SHA1 interface.
 */
class SHA1_160
{
public:
    /// Default Ctor.
    SHA1_160() { init(); }
    /// Construct and digest \p data of size \p len.
    SHA1_160(const uint8_t* data, size_t len) { init(); update(data, len); }
    /// Construct and digest C-String \p cstr.
    SHA1_160(const char* cstr) { init(); update((uint8_t*)cstr, strlen(cstr)); }
    /// Copy Ctor.
    SHA1_160(const SHA1_160& in) : m_ctx(in.m_ctx) { } // just copy the internal state
    /// Update \p data of size \p len.
    void update(const uint8_t* data, size_t len) { sha1_update(&m_ctx, len, reinterpret_cast<const uint8_t*>(data)); }
    /// Final(ize) into SHA-1 Message Digest \p md of length \c SHA1_DIGEST_SIZE bytes.
    void final(uint8_t md[SHA1_DIGEST_SIZE]) { sha1_digest(&m_ctx, SHA1_DIGEST_SIZE, md); }
    /// Final(ize) into SHA-256 Message Digest \p md of length \c SHA256_DIGEST_SIZE bytes.
    void final(CDigest160& cdig) { sha1_digest(&m_ctx, SHA1_DIGEST_SIZE, cdig.data()); }
    /// Dtor.
    ~SHA1_160() {}
protected:
    /// Initialize/Reset State.
    void init() { sha1_init(&m_ctx); }
private:
    struct sha1_ctx m_ctx;    /// Internal Context.
};

/*! SHA2-224.
 * Wraps Nettle's (libnettle) SHA2-224 interface.
 */
class SHA2_224
{
public:
    /// Default Ctor.
    SHA2_224() { init(); }
    /// Construct and digest \p data of size \p len.
    SHA2_224(const uint8_t* data, size_t len) { init(); update(data, len); }
    /// Construct and digest C-String \p cstr.
    SHA2_224(const char* cstr) { init(); update((uint8_t*)cstr, strlen(cstr)); }
    /// Copy Ctor.
    SHA2_224(const SHA2_224& in) : m_ctx(in.m_ctx) { } // just copy the internal state
    /// Update \p data of size \p len.
    void update(const uint8_t* data, size_t len) { sha224_update(&m_ctx, len, data); }
    /// Final(ize) into SHA-224 Message Digest \p md of length \c SHA224_DIGEST_SIZE bytes.
    void final(uint8_t md[SHA224_DIGEST_SIZE]) { sha224_digest(&m_ctx, SHA224_DIGEST_SIZE, md); }
    /// Final(ize) into SHA-224 Message Digest \p md of length \c SHA224_DIGEST_SIZE bytes.
    void final(CDigest224& cdig) { sha224_digest(&m_ctx, SHA224_DIGEST_SIZE, cdig.data()); }
    /// Dtor.
    ~SHA2_224() {}
protected:
    /// Initialize/Reset State.
    void init() { sha224_init(&m_ctx); }
private:
    struct sha224_ctx m_ctx;    /// Internal Context.
};

/*! SHA2-256.
 * Wraps Nettle's (libnettle) SHA2-256 interface.
 */
class SHA2_256
{
public:
    /// Default Ctor.
    SHA2_256() { init(); }
    /// Construct and digest \p data of size \p len.
    SHA2_256(const uint8_t* data, size_t len) { init(); update(data, len); }
    /// Construct and digest C-String \p cstr.
    SHA2_256(const char* cstr) { init(); update((uint8_t*)cstr, strlen(cstr)); }
    /// Copy Ctor.
    SHA2_256(const SHA2_256& in) : m_ctx(in.m_ctx) { } // just copy the internal state
    /// Update \p data of size \p len.
    void update(const uint8_t* data, size_t len) { sha256_update(&m_ctx, len, data); }
    /// Final(ize) into SHA-256 Message Digest \p md of length \c SHA256_DIGEST_SIZE bytes.
    void final(uint8_t md[SHA256_DIGEST_SIZE]) { sha256_digest(&m_ctx, SHA256_DIGEST_SIZE, md); }
    /// Final(ize) into SHA-256 Message Digest \p md of length \c SHA256_DIGEST_SIZE bytes.
    void final(CDigest256& cdig) { sha256_digest(&m_ctx, SHA256_DIGEST_SIZE, cdig.data()); }
    /// Dtor.
    ~SHA2_256() {}
protected:
    /// Initialize/Reset State.
    void init() { sha256_init(&m_ctx); }
private:
    struct sha256_ctx m_ctx;    /// Internal Context.
};

/*! SHA2-384.
 * Wraps Nettle's (libnettle) SHA2-384 interface.
 */
class SHA2_384
{
public:
    /// Default Ctor.
    SHA2_384() { init(); }
    /// Construct and digest \p data of size \p len.
    SHA2_384(const uint8_t* data, size_t len) { init(); update(data, len); }
    /// Construct and digest C-String \p cstr.
    SHA2_384(const char* cstr) { init(); update((uint8_t*)cstr, strlen(cstr)); }
    /// Copy Ctor.
    SHA2_384(const SHA2_384& in) : m_ctx(in.m_ctx) { } // just copy the internal state
    /// Update \p data of size \p len.
    void update(const uint8_t* data, size_t len) { sha384_update(&m_ctx, len, data); }
    /// Final(ize) into SHA-384 Message Digest \p md of length \c SHA384_DIGEST_SIZE bytes.
    void final(uint8_t md[SHA384_DIGEST_SIZE]) { sha384_digest(&m_ctx, SHA384_DIGEST_SIZE, md); }
    /// Final(ize) into SHA-384 Message Digest \p md of length \c SHA384_DIGEST_SIZE bytes.
    void final(CDigest384& cdig) { sha384_digest(&m_ctx, SHA384_DIGEST_SIZE, cdig.data()); }
    /// Dtor.
    ~SHA2_384() {}
protected:
    /// Initialize/Reset State.
    void init() { sha384_init(&m_ctx); }
private:
    struct sha384_ctx m_ctx;    /// Internal Context.
};

/*! SHA2-512.
 * Wraps Nettle's (libnettle) SHA2-512 interface.
 */
class SHA2_512
{
public:
    /// Default Ctor.
    SHA2_512() { init(); }
    /// Construct and digest \p data of size \p len.
    SHA2_512(const uint8_t* data, size_t len) { init(); update(data, len); }
    /// Construct and digest C-String \p cstr.
    SHA2_512(const char* cstr) { init(); update((uint8_t*)cstr, strlen(cstr)); }
    /// Copy Ctor.
    SHA2_512(const SHA2_512& in) : m_ctx(in.m_ctx) { } // just copy the internal state
    /// Update \p data of size \p len.
    void update(const uint8_t* data, size_t len) { sha512_update(&m_ctx, len, data); }
    /// Final(ize) into SHA3-512 Message Digest \p md of length \c SHA512_DIGEST_SIZE bytes.
    void final(uint8_t md[SHA512_DIGEST_SIZE]) { sha512_digest(&m_ctx, SHA512_DIGEST_SIZE, md); }
    /// Final(ize) into SHA3-512 Message Digest \p md of length \c SHA512_DIGEST_SIZE bytes.
    void final(CDigest512& cdig) { sha512_digest(&m_ctx, SHA512_DIGEST_SIZE, cdig.data()); }
    /// Dtor.
    ~SHA2_512() {}
protected:
    /// Initialize/Reset State.
    void init() { sha512_init(&m_ctx); }
private:
    struct sha512_ctx m_ctx;    /// Internal Context.
};

/*! SHA3-224.
 * Wraps Nettle's (libnettle) SHA3_224 interface.
 */
class SHA3_224
{
public:
    /// Default Ctor.
    SHA3_224() { init(); }
    /// Construct and digest \p data of size \p len.
    SHA3_224(const uint8_t* data, size_t len) { init(); update(data, len); }
    /// Construct and digest C-String \p cstr.
    SHA3_224(const char* cstr) { init(); update((uint8_t*)cstr, strlen(cstr)); }
    /// Copy Ctor.
    SHA3_224(const SHA3_224& in) : m_ctx(in.m_ctx) { } // just copy the internal state
    /// Update \p data of size \p len.
    void update(const uint8_t* data, size_t len) { sha3_224_update(&m_ctx, len, data); }
    /// Final(ize) into SHA3-224 Message Digest \p md of length \c SHA224_DIGEST_SIZE bytes.
    void final(uint8_t md[SHA224_DIGEST_SIZE]) { sha3_224_digest(&m_ctx, SHA224_DIGEST_SIZE, md); }
    /// Final(ize) into SHA3-224 Message Digest \p md of length \c SHA224_DIGEST_SIZE bytes.
    void final(CDigest224& cdig) { sha3_224_digest(&m_ctx, SHA224_DIGEST_SIZE, cdig.data()); }
    /// Dtor.
    ~SHA3_224() {}
protected:
    /// Initialize/Reset State.
    void init() { sha3_224_init(&m_ctx); }
private:
    struct sha3_224_ctx m_ctx;    /// Internal Context.
};

/*! SHA3-256.
 * Wraps Nettle's (libnettle) SHA3_256 interface.
 */
class SHA3_256
{
public:
    /// Default Ctor.
    SHA3_256() { init(); }
    /// Construct and digest \p data of size \p len.
    SHA3_256(const uint8_t* data, size_t len) { init(); update(data, len); }
    /// Construct and digest C-String \p cstr.
    SHA3_256(const char* cstr) { init(); update((uint8_t*)cstr, strlen(cstr)); }
    /// Copy Ctor.
    SHA3_256(const SHA3_256& in) : m_ctx(in.m_ctx) { } // just copy the internal state
    /// Update \p data of size \p len.
    void update(const uint8_t* data, size_t len) { sha3_256_update(&m_ctx, len, data); }
    /// Final(ize) into SHA3-256 Message Digest \p md of length \c SHA256_DIGEST_SIZE bytes.
    void final(uint8_t md[SHA256_DIGEST_SIZE]) { sha3_256_digest(&m_ctx, SHA256_DIGEST_SIZE, md); }
    /// Final(ize) into SHA3-256 Message Digest \p md of length \c SHA256_DIGEST_SIZE bytes.
    void final(CDigest256& cdig) { sha3_256_digest(&m_ctx, SHA256_DIGEST_SIZE, cdig.data()); }
    /// Dtor.
    ~SHA3_256() {}
protected:
    /// Initialize/Reset State.
    void init() { sha3_256_init(&m_ctx); }
private:
    struct sha3_256_ctx m_ctx;    /// Internal Context.
};

/*! SHA3-384.
 * Wraps Nettle's (libnettle) SHA3_384 interface.
 */
class SHA3_384
{
public:
    /// Default Ctor.
    SHA3_384() { init(); }
    /// Construct and digest \p data of size \p len.
    SHA3_384(const uint8_t* data, size_t len) { init(); update(data, len); }
    /// Construct and digest C-String \p cstr.
    SHA3_384(const char* cstr) { init(); update((uint8_t*)cstr, strlen(cstr)); }
    /// Copy Ctor.
    SHA3_384(const SHA3_384& in) : m_ctx(in.m_ctx) { } // just copy the internal state
    /// Update \p data of size \p len.
    void update(const uint8_t* data, size_t len) { sha3_384_update(&m_ctx, len, data); }
    /// Final(ize) into SHA3-384 Message Digest \p md of length \c SHA384_DIGEST_SIZE bytes.
    void final(uint8_t md[SHA384_DIGEST_SIZE]) { sha3_384_digest(&m_ctx, SHA384_DIGEST_SIZE, md); }
    /// Final(ize) into SHA3-384 Message Digest \p md of length \c SHA384_DIGEST_SIZE bytes.
    void final(CDigest384& cdig) { sha3_384_digest(&m_ctx, SHA384_DIGEST_SIZE, cdig.data()); }
    /// Dtor.
    ~SHA3_384() {}
protected:
    /// Initialize/Reset State.
    void init() { sha3_384_init(&m_ctx); }
private:
    struct sha3_384_ctx m_ctx;    /// Internal Context.
};

/*! SHA3-512.
 * Wraps Nettle's (libnettle) SHA3_512 interface.
 */
class SHA3_512
{
public:
    /// Default Ctor.
    SHA3_512() { init(); }
    /// Construct and digest \p data of size \p len.
    SHA3_512(const uint8_t* data, size_t len) { init(); update(data, len); }
    /// Construct and digest C-String \p cstr.
    SHA3_512(const char* cstr) { init(); update((uint8_t*)cstr, strlen(cstr)); }
    /// Copy Ctor.
    SHA3_512(const SHA3_512& in) : m_ctx(in.m_ctx) { } // just copy the internal state
    /// Update \p data of size \p len.
    void update(const uint8_t* data, size_t len) { sha3_512_update(&m_ctx, len, data); }
    /// Final(ize) into SHA3-512 Message Digest \p md of length \c SHA512_DIGEST_SIZE bytes.
    void final(uint8_t md[SHA512_DIGEST_SIZE]) { sha3_512_digest(&m_ctx, SHA512_DIGEST_SIZE, md); }
    /// Final(ize) into SHA3-512 Message Digest \p md of length \c SHA512_DIGEST_SIZE bytes.
    void final(CDigest512& cdig) { sha3_512_digest(&m_ctx, SHA512_DIGEST_SIZE, cdig.data()); }
    /// Dtor.
    ~SHA3_512() {}
protected:
    /// Initialize/Reset State.
    void init() { sha3_512_init(&m_ctx); }
private:
    struct sha3_512_ctx m_ctx;    /// Internal Context.
};
#endif

}

namespace openssl {

#if HAVE_OPENSSL_EVP_H
#  include <openssl/evp.h>
/*!
 * General Content Hash of type supported by OpenSSL.
 * Wraps OpenSSL's EVP Interface.
 * See: http://www.openssl.org/docs/crypto/EVP_DigestInit.html#
 */
class CHash
{
public:
    /// Default Ctor of digest type \em SHA-1.
    CHash() { init(); }

    /// Default Ctor of digest type \em named \p name.
    CHash(const char* name) { init(name); }

    /// Construct and update \p data of size \p len.
    CHash(const uint8_t* data, size_t len) { init(); update(data, len); }

    /// Construct and update \p data of size \p len.
    CHash(const char* name,
              const uint8_t* data, size_t len) { init(name); update(data, len); }

    /// Copy Ctor.
    CHash(const CHash& in) {
        EVP_MD_CTX_copy(&m_ctx, &in.m_ctx);      // copy the internal state
    }

    /// Update \p data of size \p len.
    int update(const uint8_t* data, size_t len) {
        return EVP_DigestUpdate(&m_ctx, data, len);
    }

    /// Final(ize) into a Message Digest \p md of length 512-bits (64 bytes).
    int final(uint8_t *md) {
        return EVP_DigestFinal(&m_ctx, md, NULL);
    }

    /// Dtor.
    ~CHash() { EVP_MD_CTX_cleanup(&m_ctx); } // called after a digest context is no longer needed.
protected:
    /// Initialize/Reset State.
    void init() {
        EVP_MD_CTX_init(&m_ctx); // init hash context
        const EVP_MD* md = EVP_sha1(); // default to SHA-1
        EVP_DigestInit_ex(&m_ctx, md, NULL);
    }

    /// Initialize/Reset State.
    void init(const char* name) {
        EVP_MD_CTX_init(&m_ctx); // init hash context
        const EVP_MD* md = EVP_get_digestbyname(name); // lookup hash by name
        EVP_DigestInit_ex(&m_ctx, md, NULL);
    }
private:
    EVP_MD_CTX m_ctx;           ///< Internal Hash (Digestion) State.
};
#endif

#if HAVE_OPENSSL_SHA_H
#  include <openssl/sha.h>

/*!
 * Wrapper Class for OpenSSL's \em SHA-1 interface.
 */
class SHA1_160
{
public:
    /// Default Ctor.
    SHA1_160() { init(); }
    /// Construct and digest \p data of size \p len.
    SHA1_160(const uint8_t* data, size_t len) { init(); update(data, len); }
    /// Copy Ctor.
    SHA1_160(const SHA1_160& in) : m_hs(in.m_hs) { } // just copy the internal state
    /// Update \p data of size \p len.
    int update(const uint8_t* data, size_t len) { return SHA1_Update(&m_hs, data, len); }
    /// Final(ize) into SHA-1 Message Digest \p md of length 160-bits (20 bytes).
    int final(uint8_t md[SHA1_DIGEST_SIZE]) { return SHA1_Final(md, &m_hs); }
    /// Final(ize) into SHA-1 Message Digest \p md of length 160-bits (20 bytes).
    void final(CDigest160& cdig) { SHA1_Final(cdig.data(), &m_hs); }
    /// Dtor.
    ~SHA1_160() { /* final(NULL); */ } // NULL means place result in a static buffer
protected:
    /// Initialize/Reset State.
    void init() { SHA1_Init(&m_hs); }
private:
    SHA_CTX m_hs; ///< Internal Hash (Digestion) State.
};

/*!
 * Wrapper Class for OpenSSL's \em SHA-224 interface.
 */
class SHA2_224
{
public:
    /// Default Ctor.
    SHA2_224() { init(); }
    /// Construct and digest \p data of size \p len.
    SHA2_224(const uint8_t* data, size_t len) { init(); update(data, len); }
    /// Copy Ctor.
    SHA2_224(const SHA2_224& in) : m_hs(in.m_hs) { } // just copy the internal state
    /// Update \p data of size \p len.
    int update(const uint8_t* data, size_t len) { return SHA224_Update(&m_hs, data, len); }
    /// Final(ize) into SHA-224 Message Digest \p md of length 224-bits (64 bytes).
    int final(uint8_t md[SHA224_DIGEST_SIZE]) { return SHA224_Final(md, &m_hs); }
    /// Dtor.
    ~SHA2_224() { /* final(NULL); */ } // NULL means place result in a static buffer
protected:
    /// Initialize/Reset State.
    void init() { SHA224_Init(&m_hs); }
private:
    SHA256_CTX m_hs; ///< Internal Hash (Digestion) State.
};

/*!
 * Wrapper Class for OpenSSL's \em SHA-256 interface.
 */
class SHA2_256
{
public:
    /// Default Ctor.
    SHA2_256() { init(); }
    /// Construct and digest \p data of size \p len.
    SHA2_256(const uint8_t* data, size_t len) { init(); update(data, len); }
    /// Copy Ctor.
    SHA2_256(const SHA2_256& in) : m_hs(in.m_hs) { } // just copy the internal state
    /// Update \p data of size \p len.
    int update(const uint8_t* data, size_t len) { return SHA256_Update(&m_hs, data, len); }
    /// Final(ize) into SHA-256 Message Digest \p md of length 256-bits (64 bytes).
    int final(uint8_t md[SHA256_DIGEST_SIZE]) { return SHA256_Final(md, &m_hs); }
    /// Final(ize) into SHA-256 Message Digest \p md of length 256-bits (64 bytes).
    void final(CDigest256& cdig) { SHA256_Final(cdig.data(), &m_hs); }
    /// Dtor.
    ~SHA2_256() { /* final(NULL); */ } // NULL means place result in a static buffer
protected:
    /// Initialize/Reset State.
    void init() { SHA256_Init(&m_hs); }
private:
    SHA256_CTX m_hs; ///< Internal Hash (Digestion) State.
};

/*!
 * Wrapper Class for OpenSSL's \em SHA-384 interface.
 */
class SHA2_384
{
public:
    /// Default Ctor.
    SHA2_384() { init(); }
    /// Construct and digest \p data of size \p len.
    SHA2_384(const uint8_t* data, size_t len) { init(); update(data, len); }
    /// Copy Ctor.
    SHA2_384(const SHA2_384& in) : m_hs(in.m_hs) { } // just copy the internal state
    /// Update \p data of size \p len.
    int update(const uint8_t* data, size_t len) { return SHA384_Update(&m_hs, data, len); }
    /// Final(ize) into SHA-384 Message Digest \p md of length 384-bits (64 bytes).
    int final(uint8_t md[SHA384_DIGEST_SIZE]) { return SHA384_Final(md, &m_hs); }
    /// Dtor.
    ~SHA2_384() { /* final(NULL); */ } // NULL means place result in a static buffer
protected:
    /// Initialize/Reset State.
    void init() { SHA384_Init(&m_hs); }
private:
    SHA512_CTX m_hs; ///< Internal Hash (Digestion) State.
};

/*!
 * Wrapper Class for OpenSSL's \em SHA-512 interface.
 */
class SHA2_512
{
public:
    /// Default Ctor.
    SHA2_512() { init(); }
    /// Construct and update \p data of size \p len.
    SHA2_512(const uint8_t* data, size_t len) { init(); update(data, len); }
    /// Copy Ctor.
    SHA2_512(const SHA2_512& in) : m_hs(in.m_hs) { } // just copy the internal state
    /// Update \p data of size \p len.
    int update(const uint8_t* data, size_t len) { return SHA512_Update(&m_hs, data, len); }
    /// Final(ize) into SHA-512 Message Digest \p md of length 512-bits (64 bytes).
    int final(uint8_t md[SHA512_DIGEST_SIZE]) { return SHA512_Final(md, &m_hs); }
    /// Final(ize) into SHA-512 Message Digest \p md of length 512-bits (64 bytes).
    void final(CDigest512& cdig) { SHA512_Final(cdig.data(), &m_hs); }
    /// Dtor.
    ~SHA2_512() { /* final(NULL); */ } // NULL means place result in a static buffer
protected:
    /// Initialize/Reset State.
    void init() { SHA512_Init(&m_hs); }
private:
    SHA512_CTX m_hs; ///< Internal Hash (Digestion) State.
};
#endif

}

inline CDigest160::CDigest160(nettle::SHA1_160& chash) { chash.final(*this); }
inline CDigest160::CDigest160(openssl::SHA1_160& chash) { chash.final(*this); }

inline CDigest256::CDigest256(nettle::SHA2_256& chash) { chash.final(*this); }
inline CDigest256::CDigest256(openssl::SHA2_256& chash) { chash.final(*this); }

inline CDigest512::CDigest512(openssl::SHA2_512& chash) { chash.final(*this); }

}
