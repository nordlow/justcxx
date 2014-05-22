#if defined(HAVE_CONFIG_H)
#  include "../config.h"
#endif

#ifdef HAVE_GMP_H
#  include <gmp.h>
#endif

#include "../chv.h"
#include "../chv_gmp.h"
#include "../utils.h"
#include "../stdio_x.h"

/*!
 * Test most of the functions that can be applied to the \c chv_t.
 *
 * Illustrates how the concept is used.
 */
int
test_chv(int little_endian_flag)
{
    chv_t chv;

    int8_t   c   = +123;
    uint8_t  uc  = -123;

    int16_t  s   = +12345;
    uint16_t us  = -12345;

    int      i   = +1234567890;
    uint     ui  = -1234567890;

    int64_t  ll  = 0x0011223344556677LL;
    uint64_t ull = 0x7766554433221100ULL;

    size_t   sz  = UINT32_MAX;

    float    f   = 1/3.0f;
    double   d   = 1/3.0;

    const char *cstr  = "pelle";
    char       *cstr1 = NULL;

#ifdef HAVE_GMP_H
    mpz_t mpz; mpz_init_set_str(mpz, "012345657890123456578901234565789012345657890123456578901234565789", 10);
    mpq_t mpq; mpq_init(mpq); mpq_set_str(mpq, "3/5", 10);
#endif

    printf("%d %u  %d %u  %d %u  %lld %llu  %zu  %g %g ",
           c, uc, s, us, i, ui, ll, ull, sz, f, d);
    printf("%s ", cstr);
    mpz_out_str (stdout, 10, mpz); printf(" ");
    mpq_out_str (stdout, 10, mpq); printf(" ");
    endline();

    chv_init(&chv);

    if (little_endian_flag) {
        chv_enc_s8(&chv, &c); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_enc_u8(&chv, &uc); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_enc_s16le(&chv, &s); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_enc_u16le(&chv, &us); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_enc_s32le(&chv, &i); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_enc_u32le(&chv, &ui); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_enc_s64le(&chv, &ll); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_enc_u64le(&chv, &ull); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_enc_sizele(&chv, &sz); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_enc_f32le(&chv, &f); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_enc_f64le(&chv, &d); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_enc_cstr_lengthU32le(&chv, cstr); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
    } else {
        chv_enc_s8(&chv, &c); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_enc_u8(&chv, &uc); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_enc_s16be(&chv, &s); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_enc_u16be(&chv, &us); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_enc_s32be(&chv, &i); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_enc_u32be(&chv, &ui); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_enc_s64be(&chv, &ll); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_enc_u64be(&chv, &ull); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_enc_sizebe(&chv, &sz); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_enc_f32be(&chv, &f); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_enc_f64be(&chv, &d); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_enc_cstr_lengthU32be(&chv, cstr); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
    }
#ifdef HAVE_GMP_H
    chv_enc_mpz(&chv, mpz, little_endian_flag);
    chv_enc_mpq(&chv, mpq, little_endian_flag);
#endif

    chv_save(&chv, "chv_test.bin"); // save to file

    // reset variables
    c = 0; uc = 0;
    s = 0; us = 0;
    i = 0; ui = 0;
    ll = 0; ull = 0;
    f = 0; d = 0;
    cstr = NULL;
    cstr1 = NULL;
    mpz_clear(mpz);
    mpq_clear(mpq);

    // allocate variables
    mpz_init(mpz);
    mpq_init(mpq);

    chv_load(&chv, "chv_test.bin"); // load from file

    if (little_endian_flag) {
        chv_dec_s8(&chv, &c); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_dec_u8(&chv, &uc); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_dec_s16le(&chv, &s); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_dec_u16le(&chv, &us); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_dec_s32le(&chv, &i); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_dec_u32le(&chv, &ui); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_dec_s64le(&chv, &ll); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_dec_u64le(&chv, &ull); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_dec_sizele(&chv, &sz); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_dec_f32le(&chv, &f); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_dec_f64le(&chv, &d); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_dec_cstr_lengthU32le(&chv, &cstr1); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
    } else {
        chv_dec_s8(&chv, &c); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_dec_u8(&chv, &uc); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_dec_s16be(&chv, &s); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_dec_u16be(&chv, &us); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_dec_s32be(&chv, &i); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_dec_u32be(&chv, &ui); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_dec_s64be(&chv, &ll); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_dec_u64be(&chv, &ull); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_dec_sizebe(&chv, &sz); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_dec_f32be(&chv, &f); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
        chv_dec_f64be(&chv, &d); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");

        chv_dec_cstr_lengthU32be(&chv, &cstr1); chv_fprint_x_quoted_chars(stdout, &chv); printf("\n");
    }
#ifdef HAVE_GMP_H
    chv_dec_mpz(&chv, mpz, little_endian_flag);
    chv_dec_mpq(&chv, mpq, little_endian_flag);
#endif

    printf("%d %u  %d %u  %d %u  %lld %llu  %zu  %g %g ",
           c, uc, s, us, i, ui, ll, ull, sz, f, d);
    printf("%s ", cstr1);
    mpz_out_str(stdout, 10, mpz); printf(" ");
    mpq_out_str(stdout, 10, mpq); printf(" ");
    endline();

    free(cstr1);
    mpz_clear(mpz);
    mpq_clear(mpq);

    chv_clear(&chv);

    printf("\n");

    return 0;
}

int
main(int argc, char *argv[])
{
    test_chv(0);
    test_chv(1);
    return 1;
}
