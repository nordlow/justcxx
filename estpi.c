#include "estpi.h"

/* \cite{crecipes} 20.6 p. 915
 * X_0 = \sqrt{2}
 * \pi_0 = 2 + \sqrt{2}
 * Y_0 = \root{4}{2}
 *
 * X_{i+1} = \frac{1}{2} ( \sqrt{X_i} + \frac{ 1 } { \sqrt{X_i} } )
 * \pi_{i+1} = \pi_{i+1} (\frac{ X_{i+1} + 1 } { Y_{i} + 1 })
 * Y_{i+1} = \frac{Y_i \sqrt{X_{i+1}} + \frac{1} {\sqrt{X_{i+1}}}} {Y_i + 1}
 */

#ifdef HAVE_LIBGMP
int
mpf_estimate_pi(mpf_t pi)
{
  int i;
  mpf_t x, y;			/* Iteration variables. */
  mpf_t inv, xp1, yp1, sqrtx, oldpi;	/* Loop temporaries. */
  uint precision;

  precision = mpf_get_prec(pi);

  /* Set start values. */
  mpf_init2(x, precision);
  mpf_sqrt_ui(x, 2);		/* x0 = sqrt (2) */
  mpf_set(pi, x);
  mpf_add_ui(pi, pi, 2);	/* pi0 = sqrt (2) + 2 */
  mpf_init2(y, precision);
  mpf_sqrt(y, x);		/* y0 = sqrt (sqrt (2)) */

  /* Init temporaries. */
  mpf_init2(inv, precision);
  mpf_init2(xp1, precision);
  mpf_init2(yp1, precision);
  mpf_init2(sqrtx, precision);
  mpf_init2(oldpi, precision);

  /* Iterate. */
  i = 0;
  do {
    /* x(i+1) = (sqrt (x) + 1 / sqrt (x)) / 2 */
    mpf_sqrt(x, x);
    mpf_ui_div(inv, 1, x);
    mpf_add(x, x, inv);
    mpf_div_ui(x, x, 2);

    /* Save current pi. */
    mpf_set(oldpi, pi);

    /* pi(i+1) = pi(i) * (x(i+1) + 1) / (y(i) + 1)  */
    mpf_add_ui(xp1, x, 1);
    mpf_add_ui(yp1, y, 1);
    mpf_mul(pi, pi, xp1);
    mpf_div(pi, pi, yp1);

    /* y(i+1) = (y(i) * sqrt (x(i + 1)) + 1 / sqrt (x)) / (y(i) + 1) */
    mpf_sqrt(sqrtx, x);
    mpf_ui_div(inv, 1, sqrtx);
    mpf_mul(y, y, sqrtx);
    mpf_add(y, y, inv);
    mpf_div(y, y, yp1);

    i++;
  } while (mpf_cmp(pi, oldpi) != 0);	/* Until pi has converged. */

  /* Clear temporaries. */
  mpf_clear(x);
  mpf_clear(y);
  mpf_clear(inv);
  mpf_clear(xp1);
  mpf_clear(yp1);
  mpf_clear(sqrtx);
  mpf_clear(oldpi);

  return i;			/* Return the number of iterations needed. */
}
#endif
