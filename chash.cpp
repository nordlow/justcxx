#include "chash.hpp"

namespace chash {

uint32_t chashid_get_digest_size(chashid hid)
{
    size_t sz = 0;
    switch (hid) {
    case CHASH_MD5: sz = MD5_DIGEST_SIZE; break;

    case CHASH_SHA1_160: sz = SHA1_DIGEST_SIZE; break;

    case CHASH_SHA2_224: sz = SHA224_DIGEST_SIZE; break;
    case CHASH_SHA2_256: sz = SHA256_DIGEST_SIZE; break;
    case CHASH_SHA2_384: sz = SHA384_DIGEST_SIZE; break;
    case CHASH_SHA2_512: sz = SHA512_DIGEST_SIZE; break;

    case CHASH_SHA3_224: sz = SHA224_DIGEST_SIZE; break;
    case CHASH_SHA3_256: sz = SHA256_DIGEST_SIZE; break;
    case CHASH_SHA3_384: sz = SHA384_DIGEST_SIZE; break;
    case CHASH_SHA3_512: sz = SHA512_DIGEST_SIZE; break;

    case CHASH_undefined_: break;
    }
    return sz;
}

const static char* g_md5_name = "MD5";
const static char* g_sha1_160_name = "SHA1-160";

const static char* g_sha2_224_name = "SHA2-224";
const static char* g_sha2_256_name = "SHA2-256";
const static char* g_sha2_384_name = "SHA2-384";
const static char* g_sha2_512_name = "SHA2-512";

const static char* g_sha3_224_name = "SHA3-224";
const static char* g_sha3_256_name = "SHA3-256";
const static char* g_sha3_384_name = "SHA3-384";
const static char* g_sha3_512_name = "SHA3-512";

const char* chashid_get_name_static(chashid hid)
{
    const char* ret = NULL;
    switch (hid) {
    case CHASH_MD5: ret = g_md5_name; break;

    case CHASH_SHA1_160: ret = g_sha1_160_name; break;

    case CHASH_SHA2_224: ret = g_sha2_224_name; break;
    case CHASH_SHA2_256: ret = g_sha2_256_name; break;
    case CHASH_SHA2_384: ret = g_sha2_384_name; break;
    case CHASH_SHA2_512: ret = g_sha2_512_name; break;

    case CHASH_SHA3_224: ret = g_sha3_224_name; break;
    case CHASH_SHA3_256: ret = g_sha3_256_name; break;
    case CHASH_SHA3_384: ret = g_sha3_384_name; break;
    case CHASH_SHA3_512: ret = g_sha3_512_name; break;

    case CHASH_undefined_: break;
    }
    return ret;
}

chashid chashid_from_size(uint32_t csize)
{
    chashid hid = CHASH_SHA2_256;    /* default */
    if      (csize < chashid_get_digest_size(CHASH_MD5))    { hid = CHASH_MD5; }
    else if (csize < chashid_get_digest_size(CHASH_SHA1_160))   { hid = CHASH_SHA1_160; }
    else if (csize < chashid_get_digest_size(CHASH_SHA2_256)) { hid = CHASH_SHA2_256; }
    return hid;
}

}
