#include "cmpf.h"

void
cmpf_init(cmpf_t rop)
{
  mpf_init(rop->r);
  mpf_init(rop->i);
}

void
cmpf_init_set(cmpf_t rop, const cmpf_t op)
{
  mpf_init_set(rop->r, op->r);
  mpf_init_set(rop->i, op->i);
}

void
cmpf_init_set_ui(cmpf_t rop, ulong r, ulong i)
{
  mpf_init_set_ui(rop->r, r);
  mpf_init_set_ui(rop->i, i);
}

void
cmpf_init_set_si(cmpf_t rop, signed long r, signed long i)
{
  mpf_init_set_si(rop->r, r);
  mpf_init_set_si(rop->i, i);
}

void
cmpf_init_set_d(cmpf_t rop, double r, double i)
{
  mpf_init_set_d(rop->r, r);
  mpf_init_set_d(rop->i, i);
}

void
cmpf_clear(cmpf_t rop)
{
  mpf_clear(rop->r);
  mpf_clear(rop->i);
}

/* ---------------------------- Group Separator ---------------------------- */

void
cmpf_add(cmpf_t rop, const cmpf_t op1, const cmpf_t op2)
{
  mpf_add(rop->r, op1->r, op2->r);
  mpf_add(rop->i, op1->i, op2->i);
}

void
cmpf_sub(cmpf_t rop, const cmpf_t op1, const cmpf_t op2)
{
  mpf_sub(rop->r, op1->r, op2->r);
  mpf_sub(rop->i, op1->i, op2->i);
}

void
cmpf_mul(cmpf_t rop, const cmpf_t op1, const cmpf_t op2)
{
  if (rop == op1 || rop == op2) {
    mpf_t tmp_r;
    mpf_t tmp_i;
    mpf_t tmp;

    mpf_init(tmp_r);
    mpf_init(tmp_i);
    mpf_init(tmp);

    mpf_mul(tmp_r, op1->r, op2->r);
    mpf_mul(tmp, op1->i, op2->i);
    mpf_sub(tmp_r, tmp_r, tmp);	/* Real part done. */

    mpf_mul(tmp_i, op1->r, op2->i);
    mpf_mul(tmp, op1->i, op2->r);
    mpf_add(tmp_i, tmp_i, tmp);	/* Image part done. */

    mpf_set(rop->r, tmp_r);
    mpf_set(rop->i, tmp_i);

    mpf_clear(tmp_r);
    mpf_clear(tmp_i);
    mpf_clear(tmp);
  } else {
    mpf_t tmp;

    mpf_init(tmp);

    mpf_mul(rop->r, op1->r, op2->r);
    mpf_mul(tmp, op1->i, op2->i);
    mpf_sub(rop->r, rop->r, tmp);	/* Real part done. */

    mpf_mul(rop->i, op1->r, op2->i);
    mpf_mul(tmp, op1->i, op2->r);
    mpf_add(rop->i, rop->i, tmp);	/* Image part done. */

    mpf_clear(tmp);
  }
}

void
cmpf_div(cmpf_t rop, const cmpf_t op1, const cmpf_t op2)
{
  PTEST("\n");
  if (rop == op1 || rop == op2) {
  } else {
    mpf_t rabs, iabs, q, d;

    mpf_init(rabs);
    mpf_init(iabs);
    mpf_init(q);
    mpf_init(d);

    mpf_abs(rabs, op2->r);
    mpf_abs(iabs, op2->i);

    if (mpf_cmp_ui(op2->r, 0) == 0 && mpf_cmp_ui(op2->i, 0) == 0) {
      leprintf("%s: Division by zero.\n", __FUNCTION__);
    } else if (mpf_cmp_ui(op2->r, 0) == 0) {
      mpf_div(rop->r, op1->i, op2->i);
      mpf_div(rop->i, op1->r, op2->i);
      mpf_neg(rop->i, rop->i);
    } else if (mpf_cmp_ui(op2->i, 0) == 0) {
      mpf_div(rop->r, op1->r, op2->r);
      mpf_div(rop->i, op1->i, op2->r);
    } else if (mpf_cmp(rabs, iabs) >= 0) {
      /* q = op2->i / op2->r; */
      mpf_div(q, op2->i, op2->r);

      /* d = op2->r + q * op2->i; */
      mpf_mul(d, q, op2->i);
      mpf_add(d, d, op2->r);

      /* rop->r = (op1->r + q * op1->i) / d; */
      mpf_mul(rop->r, q, op1->i);
      mpf_add(rop->r, rop->r, op1->r);
      mpf_div(rop->r, rop->r, d);

      /* rop->i = (op1->i - q * op1->r) / d; */
      mpf_mul(rop->i, q, op1->r);
      mpf_sub(rop->i, op1->i, rop->i);
      mpf_div(rop->i, rop->i, d);
    } else {
      /* q = op2->r / op2->i; */
      mpf_div(q, op2->r, op2->i);

      /* d = op2->i + q * op2->r; */
      mpf_mul(d, q, op2->r);
      mpf_add(d, d, op2->i);

      /* rop->r = (op1->i + q * op1->r) / d; */
      mpf_mul(rop->r, q, op1->r);
      mpf_add(rop->r, rop->r, op1->i);
      mpf_div(rop->r, rop->r, d);

      /* rop->i = (op1->i * q - op1->r) / d; */
      mpf_mul(rop->i, q, op1->i);
      mpf_sub(rop->i, rop->i, op1->r);
      mpf_div(rop->i, rop->i, d);
    }

    mpf_clear(rabs);
    mpf_clear(iabs);
    mpf_clear(q);
    mpf_clear(d);
  }
}

void
cmpf_neg(cmpf_t rop, const cmpf_t op)
{
  mpf_neg(rop->r, op->r);
  mpf_neg(rop->i, op->i);
}

void
cmpf_conj(cmpf_t rop, const cmpf_t op)
{
  mpf_neg(rop->i, op->i);
}

void
cmpf_norm(mpf_t rop, const cmpf_t op)
{
  mpf_t r, i, q;

  mpf_init(r);
  mpf_init(i);
  mpf_init(q);

  mpf_abs(r, op->r);
  mpf_abs(i, op->i);

  if (mpf_cmp_ui(r, 0) == 0) {
    mpf_set(rop, i);		/* Optimize and avoid zero division. */
  } else if (mpf_cmp_ui(r, 0) == 0) {
    mpf_set(rop, r);		/* Optimize and avoid zero division. */
  } else if (mpf_cmp(r, i) >= 0) {
    mpf_div(q, i, r);
    mpf_mul(q, q, q);
    mpf_add_ui(q, q, 1);
    mpf_sqrt(q, q);
    mpf_mul(rop, r, q);
  } else {
    mpf_div(q, r, i);
    mpf_mul(q, q, q);
    mpf_add_ui(q, q, 1);
    mpf_sqrt(q, q);
    mpf_mul(rop, i, q);
  }

  mpf_clear(r);
  mpf_clear(i);
  mpf_clear(q);
}

void
cmpf_norm2(mpf_t rop, const cmpf_t op)
{
  mpf_t tmp;

  mpf_init(tmp);

  mpf_mul(rop, op->r, op->r);
  mpf_mul(tmp, op->i, op->i);
  mpf_add(rop, rop, tmp);

  mpf_clear(tmp);
}

/* ---------------------------- Group Separator ---------------------------- */

size_t
cmpf_out_str(FILE * stream, int base, size_t ndigits, const cmpf_t op)
{
  size_t n;
  n = 0;
  putc('(', stream);
  n += mpf_out_str(stream, base, ndigits, op->r);
  putc(',', stream);
  n += mpf_out_str(stream, base, ndigits, op->i);
  putc(')', stream);
  return n + 2;
}
