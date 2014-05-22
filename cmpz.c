#include "cmpz.h"

void
cmpz_init(cmpz_t rop)
{
  mpz_init(rop->r);
  mpz_init(rop->i);
}

void
cmpz_clear(cmpz_t rop)
{
  mpz_clear(rop->r);
  mpz_clear(rop->i);
}

void
cmpz_init_set(cmpz_t rop, const cmpz_t op)
{
  mpz_init_set(rop->r, op->r);
  mpz_init_set(rop->i, op->i);
}

void
cmpz_init_set_ui(cmpz_t rop, ulong op_r, ulong op_i)
{
  mpz_init_set_ui(rop->r, op_r);
  mpz_init_set_ui(rop->i, op_i);
}

void
cmpz_init_set_si(cmpz_t rop, signed long op_r, signed long op_i)
{
  mpz_init_set_si(rop->r, op_r);
  mpz_init_set_si(rop->i, op_i);
}

void
cmpz_init_set_d(cmpz_t rop, double op_r, double op_i)
{
  mpz_init_set_d(rop->r, op_r);
  mpz_init_set_d(rop->i, op_i);
}

int
cmpz_init_set_str(cmpz_t rop, char *str_r, char *str_i, int base)
{
  int ret;
  ret = 0;
  ret += mpz_init_set_str(rop->r, str_r, base);
  ret += mpz_init_set_str(rop->i, str_i, base);
  return ret;
}

char *
cmpz_get_str(const cmpz_t op, int base)
{
  char *str_r, *str_i, *str;
  size_t len_r, len_i;

  str_r = mpz_get_str(NULL, base, op->r);
  str_i = mpz_get_str(NULL, base, op->i);

  len_r = strlen(str_r); /* Length of string excluding 0 at the end. */
  len_i = strlen(str_i);

  str = (char*)malloc(1 + len_r + 1 + len_i + 1 + 1);

  str[0] = '(';
  strcpy(str + 1, str_r);
  str[1 + len_r] = ',';
  strcpy(str + 1 + len_r + 1, str_i);
  str[1 + len_r + 1 + len_i] = ')';
  str[1 + len_r + 1 + len_i + 1] = '\0';

  free(str_r);
  free(str_i);

  return str;
}

void
cmpz_add(cmpz_t rop, const cmpz_t op1, const cmpz_t op2)
{
  mpz_add(rop->r, op1->r, op2->r);
  mpz_add(rop->i, op1->i, op2->i);
}

void
cmpz_sub(cmpz_t rop, const cmpz_t op1, const cmpz_t op2)
{
  mpz_sub(rop->r, op1->r, op2->r);
  mpz_sub(rop->i, op1->i, op2->i);
}

void
cmpz_mul(cmpz_t rop, const cmpz_t op1, const cmpz_t op2)
{
  if (rop == op1 || rop == op2) {
    mpz_t tmp_r;
    mpz_t tmp_i;
    mpz_t tmp;

    mpz_init(tmp_r);
    mpz_init(tmp_i);
    mpz_init(tmp);

    mpz_mul(tmp_r, op1->r, op2->r);
    mpz_mul(tmp, op1->i, op2->i);
    mpz_sub(tmp_r, tmp_r, tmp);	/* Real part done. */

    mpz_mul(tmp_i, op1->r, op2->i);
    mpz_mul(tmp, op1->i, op2->r);
    mpz_add(tmp_i, tmp_i, tmp);	/* Image part done. */

    mpz_set(rop->r, tmp_r);
    mpz_set(rop->i, tmp_i);

    mpz_clear(tmp_r);
    mpz_clear(tmp_i);
    mpz_clear(tmp);
  } else {
    mpz_t tmp;

    mpz_init(tmp);

    mpz_mul(rop->r, op1->r, op2->r);
    mpz_mul(tmp, op1->i, op2->i);
    mpz_sub(rop->r, rop->r, tmp); /* Real part done. */

    mpz_mul(rop->i, op1->r, op2->i);
    mpz_mul(tmp, op1->i, op2->r);
    mpz_add(rop->i, rop->i, tmp); /* Image part done. */

    mpz_clear(tmp);
  }
}

void
cmpz_neg(cmpz_t rop, const cmpz_t op)
{
  mpz_neg(rop->r, op->r);
  mpz_neg(rop->i, op->i);
}

void
cmpz_conj(cmpz_t rop, const cmpz_t op)
{
  mpz_neg(rop->i, op->i);
}

void
cmpz_norm(mpz_t rop, const cmpz_t op)
{
  mpz_t tmp;
  mpz_init(tmp);

  mpz_mul(rop, op->r, op->r);
  mpz_mul(tmp, op->i, op->i);
  mpz_add(rop, rop, tmp);

  mpz_clear(tmp);
}

size_t
cmpz_out_str(FILE * stream, int base, const cmpz_t op)
{
  size_t n;
  n = 0;
  putc('(', stream);
  n += mpz_out_str(stream, base, op->r);
  putc(',', stream);
  n += mpz_out_str(stream, base, op->i);
  putc(')', stream);
  return n + 2;
}
