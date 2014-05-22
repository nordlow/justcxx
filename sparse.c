#include "sparse.h"
#include "convert.h"

#include <stdlib.h>

/*! If needed, make room for length non-zero elements. */
void
CSRd_reserve(CSRd out, const int new_len)
{
  if (out->length == 0) {
#if NDEBUG
    leprintf("allocating %p to %d elements.\n", out, new_len);
#endif
    out->j = intarray_malloc((new_len));
    out->a = darray_malloc((new_len));
    out->length = new_len;
  } else if (out->length < new_len) {
#if NDEBUG
    leprintf("reallocating %p from %d to %d elements.\n",
	    out, out->length, new_len);
#endif
    out->j = intarray_realloc(out->j, new_len);
    out->a = darray_realloc(out->a, new_len);
    out->length = new_len;
  }
}

void
CSRd_reserve_block(CSRd out, int new_len)
{
  if (out->length < new_len)
    CSRd_reserve(out, int_binceil(new_len));
}

/*! Constructs a matrix with the same structure as in. */
void
CSRd_make_same(CSRd out, const CSRd in)
{
  int n;

  n = CSRd_nnz(in);

  intarray_copy(out->i, in->i, in->h + 1);

  /* Out has the same nnz as in. */
  CSRd_reserve_block(out, n);
  intarray_copy(out->j, in->j, n);
}

/*! Minimize the memory consumption needed for out's non-zero elements. */
void
CSRd_compress(CSRd out)
{
  int n;
  n = CSRd_nnz(out);
  if (out->length > n) {
#if NDEBUG
    leprintf("reallocating %p from %d to %d elements.\n",
	    out, out->length, n);
#endif
    out->j = intarray_realloc(out->j, n);
    out->a = darray_realloc(out->a, n);
    out->length = n;
  }
}

void
CSRd_init_zeros(CSRd out, int w, int h)
{
  out->w = w;
  out->h = h;

  out->i = intarray_malloc((out->h + 1));

  out->length = 0;

  CSRd_zeros(out);
}

/*!
 * Be sure that you have understood the underlying storage technique
 * of CSRd before you use this function.
 */
void
CSRd_init_from_arrays(CSRd out, int w, int h,
		      const int *i, const int *j, const double *a)
{
  int n;

  out->w = w;
  out->h = h;

  out->i = intarray_malloc((h + 1));
  intarray_copy(out->i, i, (h + 1));

  out->j = NULL;
  out->a = NULL;
  out->length = 0;

  n = CSRd_nnz(out);

  /* Out has the same nnz as in. */
  CSRd_reserve_block(out, n);

  intarray_copy(out->j, j, n);
  darray_copy(out->a, a, n);
}

void
CSRd_zeros(CSRd out)
{
  intarray_zeros(out->i, out->h + 1);

  if (out->length > 0) {
    free(out->j);
    free(out->a);
    out->length = 0;
  }
}

void
CSRd_ones(CSRd out)
{
  int x, y, w, h, n;

  /* Shorthands */
  w = out->w;
  h = out->h;

  CSRd_reserve_block(out, w * h);

  n = 0;
  for (y = 0; y < h; y++) {
    out->i[y] = w * y;		/* Rows start at integer multiples of w. */
    for (x = 0; x < w; x++, n++)
      out->j[n] = x;		/* Write column index. */
  }
  out->i[h] = w * h;

  darray_ones(out->a, w * h);
}

void
CSRd_ramp(CSRd out, double start, double step)
{
  int x, y, w, h, n;

  /* Shorthands */
  w = out->w;
  h = out->h;

  CSRd_reserve_block(out, w * h);

  n = 0;
  for (y = 0; y < h; y++) {
    out->i[y] = w * y;		/* Rows start at integer multiples of w. */
    for (x = 0; x < w; x++) {
      out->j[n] = x;		/* Write x as column index. */
      out->a[n] = start + n * step;	/* Write natural. */
      n++;			/* Next index and natural, please. */
    }
  }
  out->i[h] = w * h;
}

void
CSRd_eye(CSRd out)
{
  int y, w, h, min_side;

  /* Shorthands */
  w = out->w;
  h = out->h;

  min_side = MIN2(w, h);

  CSRd_reserve_block(out, min_side);

  /* Set the ones. */
  for (y = 0; y < min_side; y++) {
    out->i[y] = y;
    out->j[y] = y;
    out->a[y] = 1.0;
  }

  /* Set the rest of the row start indexes. */
  for (y = min_side; y < h + 1; y++)
    out->i[y] = min_side;
}

void
CSRd_rand(CSRd out)
{
  int w, h;
  double **dblm;

  /* Shorthands */
  w = out->w;
  h = out->h;

  dblm = doubleAA_calloc(h, w);
  dmatrix_rangerand(dblm, w, h, 0.0, 1.0);
  CSRd_from_dblm(out, (const double **) dblm);

  dmatrix_free(dblm);
}

void
CSRd_sparse_ones(CSRd out, const CSRd in)
{
  int c, n;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (!check_pointwise_unary_dims(out, in)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  n = CSRd_nnz(in);

  CSRd_make_same(out, in);

  for (c = 0; c < n; c++)
    out->a[c] = 1.0;
}

void
CSRd_clear(CSRd out)
{
  /* \TODO: Remove later... */
  out->w = 0;
  out->h = 0;

  free(out->i);

  if (out->length > 0) {
    free(out->j);
    free(out->a);
  }

  out->length = 0;
}

void
CSRd_copy(CSRd out, const CSRd in)
{
  int n;

  n = CSRd_nnz(in);

  /* Out has the same nnz as in. */
  CSRd_reserve_block(out, n);

  intarray_copy(out->j, in->j, n);
  darray_copy(out->a, in->a, n);
}

void
dmatrix_from_dcsr(double **out, const CSRd in)
{
  int x, y, w, h, i, i_next;

  /* Shorthands */
  w = in->w;
  h = in->h;

  /* First row always starts at index 0. */
  i = 0;

  for (y = 0; y < h; y++) {
    i_next = in->i[y + 1];
    for (x = 0; x < w; x++)
      if (i < i_next && in->j[i] == x) {
	out[y][x] = in->a[i];
	i++;
      } else
	out[y][x] = 0.0;
  }
}

void
CSRd_from_dblm(CSRd out, const double **in)
{
  int x, y, w, h, o;
  double value;

  /* Shorthands */
  w = out->w;
  h = out->h;

  CSRd_reserve_block(out, w * h);

  /* First row always starts at index 0. */
  o = 0;

  for (y = 0; y < h; y++) {
    out->i[y] = o;
    for (x = 0; x < w; x++) {
      value = in[y][x];
      if (value != 0.0) {
	out->j[o] = x;
	out->a[o] = value;
	o++;
      }
    }
  }

  out->i[h] = o;
}

int
CSRd_toteq(const CSRd in1, const CSRd in2)
{
  int h, n1, n2;

#if NDEBUG
  if (in1->w != in2->w || in1->h != in2->h) {
    leprintf("argument dimension mismatch.\n");
    return -1;
  }
#endif

  /* Shorthands */
  h = in1->h;
  n1 = CSRd_nnz(in1);
  n2 = CSRd_nnz(in2);

  /* Check if in1 and in2 has the same number of non-zero elements. */
  if (n1 != n2)
    return FALSE;

  /* Check if the nnz's of in1 and in2 are equally distributed over the
     rows. */
  if (memcmp(in1->i, in2->i, (h + 1) * sizeof(int)) != 0)
    return FALSE;

  /* Check if the column indexes */
  if (memcmp(in1->j, in2->j, n1 * sizeof(int)) != 0)
    return FALSE;

  /* and their corresponding values are equal. */
  if (memcmp(in1->a, in2->a, n1 * sizeof(double)) != 0)
    return FALSE;

  return TRUE;
}

void
CSRd_densePrint(FILE * stream, const CSRd in, char *name)
{
  double **dblm;

  dblm = doubleAA_malloc(in->h, in->w);
  dmatrix_from_dcsr(dblm, in);
  fprintf(stream, "%s:", name);
  dmatrix_fprint(stream, (const double **) dblm, in->w, in->h);
  dmatrix_free(dblm);
}

void
CSRd_print(FILE * stream, const CSRd in, char *name)
{
  int y, h, i, i_next;

  fprintf(stream, "%s (width:%d height:%d type:double) =\n\n",
	  name, in->w, in->h);

  /* Shorthands */
  h = in->h;

  /* First row always starts at index 0. */
  i = 0;

  if (CSRd_nnz(in) == 0)
    fprintf(stream, "  All zero sparse.\n");
  else
    for (y = 0; y < h; y++) {
      i_next = in->i[y + 1];
      while (i < i_next) {
	fprintf(stream, "  (%d, %d)  %f\n", in->j[i], y, in->a[i]);
	i++;
      }
    }
  putc('\n', stream);
}

void
CSRd_internalPrint(FILE * stream, const CSRd in, char *name)
{
  int i, n;

  n = CSRd_nnz(in);

  fprintf(stream, "w:%d h:%d nnz:%d length:%d\n",
	  in->w, in->h, n, in->length);

  fprintf(stream, "i: ");
  for (i = 0; i < in->h + 1; i++)
    fprintf(stream, "%d ", in->i[i]);

  fprintf(stream, "\nj: ");
  for (i = 0; i < n; i++)
    fprintf(stream, "%d ", in->j[i]);

  fprintf(stream, "\na: ");
  for (i = 0; i < n; i++)
    fprintf(stream, "%6.2f ", in->a[i]);

  fprintf(stream, "\n\n");
}

void
CSRd_x_concat(CSRd out, const CSRd in1, const CSRd in2)
{
  int o, i1, i2, i1_next, i2_next, length, y, h;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (out == in1 || out == in2) {
    leprintf("out cannot equal in1 nor in2.\n");
    return;
  }
  if (out->h != in1->h || out->h != in2->h || out->w != in1->w + in2->w) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  /* Shorthands. */
  h = out->h;

  /* Make room for result matrix. */
  CSRd_reserve_block(out, CSRd_nnz(in1) + CSRd_nnz(in2));

  /* First row always starts at index 0. */
  o = 0;
  i1 = 0;
  i2 = 0;

  /* For all rows. */
  for (y = 0; y < h; y++) {

    /* Write start index to the current out row. */
    out->i[y] = o;

    /* Shorthand for the indexes to the next in rows. */
    i1_next = in1->i[y + 1];
    i2_next = in2->i[y + 1];

    /* Write left part of row. */
    length = i1_next - i1;
    if (length > 0) {
      intarray_copy(out->j + o, in1->j + i1, length);
      darray_copy(out->a + o, in1->a + i1, length);
      o += length;
      i1 = i1_next;
    }

    /* Write right part of row. */
    length = i2_next - i2;
    if (length > 0) {
      intarray_add2_int(out->j + o, in2->j + i2, in1->w, length);
      darray_copy(out->a + o, in2->a + i2, length);
      o += length;
      i2 = i2_next;
    }

  }

  /* Write start index to the (y + 1):th out row. */
  out->i[h] = o;
}

void
CSRd_y_concat(CSRd out, const CSRd in1, const CSRd in2)
{
  int n1, n2, h1, h2;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (out == in1 || out == in2) {
    leprintf("CSRd_y_concat: out cannot equal in1 nor in2.\n");
    return;
  }
  if (out->w != in1->w || out->w != in2->w || out->h != in1->h + in2->h) {
    leprintf("CSRd_y_concat: Argument dimension mismatch.\n");
    return;
  }
#endif

  /* Shorthands. */
  h1 = in1->h;
  h2 = in2->h;
  n1 = CSRd_nnz(in1);
  n2 = CSRd_nnz(in2);

  /* Out has the same nnz as in1 and in2 together. */
  CSRd_reserve_block(out, n1 + n2);

  /* Upper part. */
  intarray_copy(out->i, in1->i, h1);
  intarray_copy(out->j, in1->j, n1);
  darray_copy(out->a, in1->a, n1);

  /* Lower part. */
  intarray_add2_int(out->i + h1, in2->i, n1, h2 + 1);
  intarray_copy(out->j + n1, in2->j, n2);
  darray_copy(out->a + n1, in2->a, n2);
}

void
CSRd_getRegion(CSRd out, const CSRd in, int x0, int y0)
{
  int out_y, out_w, out_h, o, i, i_next;
  const int bsearch_limit = 16;

#if NDEBUG
  if (x0 < 0 || y0 < 0 || x0 + out->w > in->w || y0 + out->h > in->h)
    leprintf("CSRd_getRegion: Argument dimension mismatch.\n");
#endif

  /* Shorthands. */
  out_w = out->w;
  out_h = out->h;

  /* First out row always starts at index 0. */
  o = 0;

  /* First in row could start elsewhere. */
  i = in->i[y0];

  /* For all rows. */
  for (out_y = 0; out_y < out_h; out_y++) {

    /* Write start index to the current out row. */
    out->i[out_y] = o;

    /* Shorthand for the indexes to the next in rows. */
    i_next = in->i[out_y + y0 + 1];

    if (i_next - i > bsearch_limit) {
      /* Use binary search to find boundary indexes. Faster for larger
       * matrices. */

      int start, length;

      /* Find start */
      start = intarray_bsearch((const int *) in->j + i,
			   i_next - i, x0);
      if (start < 0)
	start = -(1 + start);
      i += start;

      /* and length of region in current row. */
      length = intarray_bsearch((const int *) in->j + i,
			    i_next - i, x0 + out_w);
      if (length < 0)
	length = -(1 + length);

      /* Copy the region. */
      if (length > 0) {
	CSRd_reserve_block(out, o + length);
	intarray_sub2_int(out->j + o, in->j + i, x0, length);
	darray_copy(out->a + o, in->a + i, length);
	o += length;
      }

      i = i_next;
    } else {
      /* Use linear search to find boundary indexes. Faster for smaller
       * matrices because of less overhead. */

      /* For all non-zero elements in current in row. */
      while (i < i_next) {
	/* If the in element lies in the right horizontal out
	   region. */
	if (in->j[i] >= x0 && in->j[i] < (x0 + out_w)) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in->j[i] - x0;
	  out->a[o] = in->a[i];
	  o++;
	}
	i++;
      }
    }

  }

  /* Write start index to the (y + 1):th out row. */
  out->i[out_h] = o;
}

void
CSRd_transpose(CSRd out, const CSRd in)
{
  int i, i_next, front_i, in_y, n;
  int *intarray_temp;			/* Temp for column indexes. */

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (out == in) {
    leprintf("CSRd_transpose: Cannot handle out == in.\n");
    return;
  }
  if (out->w != in->h || out->h != in->w) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  n = CSRd_nnz(in);

  /* Out has the same nnz as in. */
  CSRd_reserve_block(out, n);

  /* Construct a histogram (intarray_temp) of the column indexes of in. */
  intarray_temp = intarray_calloc(in->w + 1);	/* calloc zeros all bins. */
  for (i = 0; i < n; i++)
    intarray_temp[in->j[i]]++;

  /* Set start indexes to the beginning of the rows of out. */
  front_i = 0;
  for (i = 0; i < out->h; i++) {
    out->i[i] = front_i;
    front_i += intarray_temp[i];
  }
  out->i[out->h] = front_i;	/* Finally set n. */

  /* Set start offsets for the first empty positions in each out row. */
  intarray_copy(intarray_temp, out->i, out->h + 1);

  /* First row always starts at index 0. */
  i = 0;

  /* For all in rows. */
  for (in_y = 0; in_y < in->h; in_y++) {

    /* Shorthands for the indexes to the next in rows. */
    i_next = in->i[in_y + 1];

    /* For all non-zero elements in current in row. */
    while (i < i_next) {

      /* Read from in and write to out. */
      out->j[intarray_temp[in->j[i]]] = in_y;
      out->a[intarray_temp[in->j[i]]] = in->a[i];
      intarray_temp[in->j[i]]++;

      i++;			/* Next element, please. */
    }
  }

  /* Free intarray_temp. */
  free(intarray_temp);
}

/*! This routine is slower than the above but could be useful in some other
   situations. */
#if 0
void
CSRd_transpose1(CSRd out, const CSRd in)
{
  int i, i_next, in_y, out_y, n;

  /* \TODO: Change names to l, j, a. */
  int *row_columns;
  int *row_lengths;
  int **j_rows;
  double **a_rows;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (out == in) {
    leprintf("cannot handle out == in.\n");
    return;
  }
  if (out->w != in->h || out->h != in->w) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  /* Set up expandable row buffers and their corresponding columns and
     lengths. */
  j_rows = malloc(sizeof(int *) * out->h);
  a_rows = malloc(sizeof(double *) * out->h);
  row_columns = intarray_malloc((out->h));
  row_lengths = intarray_malloc((out->h));
  for (out_y = 0; out_y < out->h; out_y++) {
    j_rows[out_y] = intarray_malloc((1));
    a_rows[out_y] = darray_malloc((1));
    row_columns[out_y] = 0;
    row_lengths[out_y] = 1;
  }

  i = 0;
  /* For all in rows. */
  for (in_y = 0; in_y < in->h; in_y++) {

    /* Shorthands for the indexes to the next in rows. */
    i_next = in->i[in_y + 1];

    /* For all non-zero elements in current in row. */
    while (i < i_next) {
      int out_row = in->j[i];
      j_rows[out_row][row_columns[out_row]] = in_y;
      a_rows[out_row][row_columns[out_row]] = in->a[i];
      row_columns[out_row]++;
      if (row_columns[out_row] == row_lengths[out_row]) {
	row_lengths[out_row] *= 2;
	j_rows[out_row] = intarray_realloc(j_rows[out_row], row_lengths[out_row]);
	a_rows[out_row] = darray_realloc(a_rows[out_row], row_lengths[out_row]);
      }
      i++;
    }
  }

  n = CSRd_nnz(in);

  /* Out has the same nnz as in. */
  CSRd_reserve_block(out, n);

  /* Copy everything to out. */
  i = 0;
  for (out_y = 0; out_y < out->h; out_y++) {
    out->i[out_y] = i;
    intarray_copy(out->j + i, j_rows[out_y], row_columns[out_y]);
    darray_copy(out->a + i, a_rows[out_y], row_columns[out_y]);
    i += row_columns[out_y];
  }
  out->i[out_y] = i;

  /* Free. */
  for (i = 0; i < out->w; i++) {
    free(j_rows[i]);
    free(a_rows[i]);
  }
  free(j_rows);
  free(a_rows);
  free(row_columns);
  free(row_lengths);
}
#endif

void
intarray_find_dcsr(int *out, const CSRd in)
{
  int i, i_next, y, w, h;

  /* Shorthands. */
  w = in->w;
  h = in->h;

  /* First row always starts at index 0. */
  i = 0;

  /* For all rows. */
  for (y = 0; y < h; y++) {

    /* Shorthands for the indexes to the next in rows. */
    i_next = in->i[y + 1];

    /* Reuse. */
    intarray_add2_int(out + i, in->j + i, w * y, i_next - i);
    i = i_next;
  }
}

void
intarray_find_xycoords_dcsr(int *out_x, int *out_y, const CSRd in)
{
  int i, i_next, y, w, h;

  /* Shorthands. */
  w = in->w;
  h = in->h;

  /* First row always starts at index 0. */
  i = 0;

  /* For all rows. */
  for (y = 0; y < h; y++) {

    /* Shorthands for the indexes to the next in rows. */
    i_next = in->i[y + 1];

    /* Reuse. */
    intarray_copy(out_x + i, in->j + i, i_next - i);
    intarray_convert_int(out_y + i, y, i_next - i);

    i = i_next;
  }
}

#if NDEBUG
int
check_pointwise_unary_dims(CSRd out, const CSRd in)
{
  return out->w == in->w && out->h == in->h;
}
#endif

void
CSRd_elementwise_neg(CSRd out, const CSRd in)
{
  int n;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (!check_pointwise_unary_dims(out, in)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  n = CSRd_nnz(in);

  CSRd_make_same(out, in);

  /* Reuse array function. */
  darray_neg(out->a, in->a, n);
}

void
CSRd_elementwise_abs(CSRd out, const CSRd in)
{
  int n;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (!check_pointwise_unary_dims(out, in)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  n = CSRd_nnz(in);

  CSRd_make_same(out, in);

  /* Reuse array function. */
  darray_abs(out->a, in->a, n);
}

void
CSRd_elementwise_inv(CSRd out, const CSRd in)
{
  int n;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (!check_pointwise_unary_dims(out, in)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  n = CSRd_nnz(in);

  CSRd_make_same(out, in);

  /* Reuse array function. */
  darray_inv(out->a, in->a, n);
}

void
CSRd_elementwise_mul_dbl(CSRd out, const CSRd in, const double scalar)
{
  int n;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (!check_pointwise_unary_dims(out, in)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  if (scalar == 0.0) {
    CSRd_zeros(out);
    return;
  }

  n = CSRd_nnz(in);

  CSRd_make_same(out, in);

  darray_mul2_dbl(out->a, in->a, scalar, n);
}

void
CSRd_elementwise_div_dbl(CSRd out, const CSRd in, const double scalar)
{
  int n;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (!check_pointwise_unary_dims(out, in)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  if (scalar == 1.0)
    return;
  else if (scalar == 0.0) {
    leprintf("divison by zero.\n");
    return;
  }

  n = CSRd_nnz(in);

  CSRd_make_same(out, in);

  darray_div2_dbl(out->a, in->a, scalar, n);
}

#if NDEBUG
int
check_pointwise_binary_dims(CSRd out, const CSRd in1, const CSRd in2)
{
  return
    out->w == in1->w && out->w == in2->w &&
    out->h == in1->h && out->h == in2->h;
}
#endif

void
CSRd_elementwise_add(CSRd out, const CSRd in1, const CSRd in2)
{
  int y, w, h;
  int o, i1, i2, i1_next, i2_next, rest_length;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (out == in1 || out == in2) {
    leprintf("out cannot equal in1 nor in2.\n");
    return;
  }
  if (!check_pointwise_binary_dims(out, in1, in2)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  /* Shorthands. */
  w = in1->w;
  h = in1->h;

  /* First row always starts at index 0. */
  o = 0;
  i1 = 0;
  i2 = 0;

  /* For all rows. */
  for (y = 0; y < h; y++) {

    /* Write start index to the current out row. */
    out->i[y] = o;

    /* Shorthands for the indexes to the next in rows. */
    i1_next = in1->i[y + 1];
    i2_next = in2->i[y + 1];

    /* The loop where the real arithmetic is done. */
    while (i1 < i1_next && i2 < i2_next) {
      if (in1->j[i1] == in2->j[i2]) {
	double sum = in1->a[i1] + in2->a[i2];	/* Add. */
	if (sum != 0.0) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in1->j[i1];
	  out->a[o] = sum;
	  o++;
	}
	i1++;
	i2++;
      } else if (in1->j[i1] < in2->j[i2]) {
	CSRd_reserve_block(out, o + 1);
	out->j[o] = in1->j[i1];
	out->a[o] = in1->a[i1];	/* Copy. */
	i1++;
	o++;
      } else if (in1->j[i1] > in2->j[i2]) {
	CSRd_reserve_block(out, o + 1);
	out->j[o] = in2->j[i2];
	out->a[o] = in2->a[i2];	/* Copy. */
	i2++;
	o++;
      }
    }

    /* Copy the rest. */
    if (i1 < i1_next) {
      rest_length = i1_next - i1;
      CSRd_reserve_block(out, o + rest_length);
      intarray_copy(out->j + o, in1->j + i1, rest_length);
      darray_copy(out->a + o, in1->a + i1, rest_length);
      i1 += rest_length;
      o += rest_length;
    } else if (i2 < i2_next) {
      rest_length = i2_next - i2;
      CSRd_reserve_block(out, o + rest_length);
      intarray_copy(out->j + o, in2->j + i2, rest_length);
      darray_copy(out->a + o, in2->a + i2, rest_length);
      i2 += rest_length;
      o += rest_length;
    }

  }

  /* Write start index to the (y + 1):th out row. */
  out->i[h] = o;
}

void
CSRd_elementwise_sub(CSRd out, const CSRd in1, const CSRd in2)
{
  int y, w, h;
  int o, i1, i2, i1_next, i2_next, rest_length;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (out == in1 || out == in2) {
    leprintf("out cannot equal in1 nor in2.\n");
    return;
  }
  if (!check_pointwise_binary_dims(out, in1, in2)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  /* Shorthands. */
  w = in1->w;
  h = in1->h;

  /* First row always starts at index 0. */
  o = 0;
  i1 = 0;
  i2 = 0;

  /* For all rows. */
  for (y = 0; y < h; y++) {

    /* Write start index to the current out row. */
    out->i[y] = o;

    /* Shorthands for the indexes to the next in rows. */
    i1_next = in1->i[y + 1];
    i2_next = in2->i[y + 1];

    /* The loop where the real arithmetic is done. */
    while (i1 < i1_next && i2 < i2_next) {
      if (in1->j[i1] == in2->j[i2]) {
	double sum = in1->a[i1] - in2->a[i2];	/* Subtract. */
	if (sum != 0.0) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in1->j[i1];
	  out->a[o] = sum;
	  o++;
	}
	i1++;
	i2++;
      } else if (in1->j[i1] < in2->j[i2]) {
	CSRd_reserve_block(out, o + 1);
	out->j[o] = in1->j[i1];
	out->a[o] = in1->a[i1];	/* Copy. */
	i1++;
	o++;
      } else if (in1->j[i1] > in2->j[i2]) {
	CSRd_reserve_block(out, o + 1);
	out->j[o] = in2->j[i2];
	out->a[o] = -in2->a[i2];	/* Negate. */
	i2++;
	o++;
      }
    }

    if (i1 < i1_next) {
      rest_length = i1_next - i1;

      CSRd_reserve_block(out, o + rest_length);

      /* Copy the rest of the indexes and */
      intarray_copy(out->j + o, in1->j + i1, rest_length);

      /* the corresponding values. */
      darray_copy(out->a + o, in1->a + i1, rest_length);

      i1 += rest_length;
      o += rest_length;
    } else if (i2 < i2_next) {
      rest_length = i2_next - i2;

      CSRd_reserve_block(out, o + rest_length);

      /* Copy the rest of the indexes and */
      intarray_copy(out->j + o, in2->j + i2, rest_length);

      /* negate the corresponding values. */
      darray_neg(out->a, in2->a, rest_length);

      i2 += rest_length;
      o += rest_length;
    }

  }

  /* Write start index to the (y + 1):th out row. */
  out->i[h] = o;
}

void
CSRd_elementwise_mul(CSRd out, const CSRd in1, const CSRd in2)
{
  int y, h;
  int o, i1, i2, i1_next, i2_next;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (out == in1 || out == in2) {
    leprintf("out cannot equal in1 nor in2.\n");
    return;
  }
  if (!check_pointwise_binary_dims(out, in1, in2)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  /* Shorthands. */
  h = in1->h;

  /* First row always starts at index 0. */
  o = 0;
  i1 = 0;
  i2 = 0;

  /* For all rows. */
  for (y = 0; y < h; y++) {

    /* Write start index to the current out row. */
    out->i[y] = o;

    /* Shorthands for the indexes to the next in rows. */
    i1_next = in1->i[y + 1];
    i2_next = in2->i[y + 1];

    /* Worst case (upper) prediction of nnz's in current out row. */
/*          CSRd_reserve_block(out, o + MIN2((i2_next - i2), (i1_next - i1))); */

    /* The loop where the real arithmetic is done. */
    while (i1 < i1_next && i2 < i2_next) {
      if (in1->j[i1] == in2->j[i2]) {
	CSRd_reserve_block(out, o + 1);
	out->j[o] = in1->j[i1];
	out->a[o] = in1->a[i1] * in2->a[i2];
	o++;
	i1++;
	i2++;
      } else if (in1->j[i1] < in2->j[i2]) {
	i1++;
      } else if (in1->j[i1] > in2->j[i2]) {
	i2++;
      }
    }

    /* No copying (like in pwadd) is needed here. */
  }

  /* Write start index to the (y + 1):th out row. */
  out->i[h] = o;
}

void
CSRd_elementwise_div(CSRd out, const CSRd in1, const CSRd in2)
{
  int y, h;
  int o, i1, i2, i1_next, i2_next;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (out == in1 || out == in2) {
    leprintf("out cannot equal in1 nor in2.\n");
    return;
  }
  if (!check_pointwise_binary_dims(out, in1, in2)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  /* Shorthands. */
  h = in1->h;

  /* First row always starts at index 0. */
  o = 0;
  i1 = 0;
  i2 = 0;

  /* For all rows. */
  for (y = 0; y < h; y++) {

    /* Write start index to the current out row. */
    out->i[y] = o;

    /* Shorthands for the indexes to the next in rows. */
    i1_next = in1->i[y + 1];
    i2_next = in2->i[y + 1];

    /* Worst case (upper) prediction of nnz's in current out row. */
/*          CSRd_reserve_block(out, o + MIN2((i2_next - i2), (i1_next - i1))); */

    /* The loop where the real arithmetic is done. */
    while (i1 < i1_next && i2 < i2_next) {
      if (in1->j[i1] == in2->j[i2]) {
	CSRd_reserve_block(out, o + 1);
	out->j[o] = in1->j[i1];
	out->a[o] = in1->a[i1] / in2->a[i2];
	i1++;
	i2++;
	o++;
      } else if (in1->j[i1] < in2->j[i2]) {
	i1++;
      } else if (in1->j[i1] > in2->j[i2]) {
	i2++;
      }
    }

    /* No copying is needed in multiplication. */
  }

  /* Write start index to the (y + 1):th out row. */
  out->i[h] = o;
}

void
CSRd_elementwise_eq(CSRd out, const CSRd in1, const CSRd in2)
{
  int x, y, w, h;
  int o, i1, i2, i1_next, i2_next;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (out == in1 || out == in2) {
    leprintf("out cannot equal in1 nor in2.\n");
    return;
  }
  if (!check_pointwise_binary_dims(out, in1, in2)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  /* Shorthands. */
  w = in1->w;
  h = in1->h;

  /* First row always starts at index 0. */
  o = 0;
  i1 = 0;
  i2 = 0;

  /* For all rows. */
  for (y = 0; y < h; y++) {

    /* Write start index to the current out row. */
    out->i[y] = o;

    /* Shorthands for the indexes to the next in rows. */
    i1_next = in1->i[y + 1];
    i2_next = in2->i[y + 1];

    /* Worst case (upper) prediction of nnz's in current out row. */
/*          CSRd_reserve_block(out, o + w - */
/*                                ABS((i2_next - i2) - (i1_next - i1))); */

    /* The loop where the real arithmetic is done. */
    for (x = 0; x < w; x++) {
      if (x == in1->j[i1] && x == in2->j[i2]) {
	if (in1->a[i1] == in2->a[i2]) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in1->j[i1];
	  out->a[o] = 1.0;
	  o++;
	}
	i1++;
	i2++;
      } else if (x != in1->j[i1] && x != in2->j[i2]) {
	CSRd_reserve_block(out, o + 1);
	out->j[o] = in1->j[i1];
	out->a[o] = 1.0;
	o++;
      }
    }

  }

  /* Write start index to the (y + 1):th out row. */
  out->i[h] = o;
}

void
CSRd_elementwise_min(CSRd out, const CSRd in1, const CSRd in2)
{
  int y, w, h;
  int o, i1, i2, i1_next, i2_next;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (out == in1 || out == in2) {
    leprintf("out cannot equal in1 nor in2.\n");
    return;
  }
  if (!check_pointwise_binary_dims(out, in1, in2)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  /* Shorthands. */
  w = in1->w;
  h = in1->h;

  /* First row always starts at index 0. */
  o = 0;
  i1 = 0;
  i2 = 0;

  /* For all rows. */
  for (y = 0; y < h; y++) {

    /* Write start index to the current out row. */
    out->i[y] = o;

    /* Shorthands for the indexes to the next in rows. */
    i1_next = in1->i[y + 1];
    i2_next = in2->i[y + 1];

    /* The loop (in row) where the real comparison is done. */
    while (i1 < i1_next && i2 < i2_next) {
      if (in1->j[i1] == in2->j[i2]) {
	/* Minimum of two non-zero elements is always non-zero. */
	CSRd_reserve_block(out, o + 1);
	out->j[o] = in1->j[i1];
	out->a[o] = MIN2(in1->a[i1], in2->a[i2]);
	o++;
	i1++;
	i2++;
      } else if (in1->j[i1] < in2->j[i2]) {
	/* min(a, 0) is non-zero whenever a < 0. */
	if (in1->a[i1] < 0.0) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in1->j[i1];
	  out->a[o] = in1->a[i1];
	  o++;
	}
	i1++;
      } else if (in1->j[i1] > in2->j[i2]) {
	/* min(a, 0) is non-zero whenever a < 0. */
	if (in2->a[i2] < 0.0) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in2->j[i2];
	  out->a[o] = in2->a[i2];
	  o++;
	}
	i2++;
      }
    }

    /* The rest in current row. */
    if (i1 < i1_next) {
      while (i1 < i1_next) {
	if (in1->a[i1] < 0.0) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in1->j[i1];
	  out->a[o] = in1->a[i1];
	  o++;
	}
	i1++;
      }
    } else if (i2 < i2_next) {
      while (i2 < i2_next) {
	if (in2->a[i2] < 0.0) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in2->j[i2];
	  out->a[o] = in2->a[i2];
	  o++;
	}
	i2++;
      }
    }
  }

  /* Write start index to the (y + 1):th out row. */
  out->i[h] = o;
}

void
CSRd_elementwise_max(CSRd out, const CSRd in1, const CSRd in2)
{
  int y, w, h;
  int o, i1, i2, i1_next, i2_next;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (out == in1 || out == in2) {
    leprintf("out cannot equal in1 nor in2.\n");
    return;
  }
  if (!check_pointwise_binary_dims(out, in1, in2)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  /* Shorthands. */
  w = in1->w;
  h = in1->h;

  /* First row always starts at index 0. */
  o = 0;
  i1 = 0;
  i2 = 0;

  /* For all rows. */
  for (y = 0; y < h; y++) {

    /* Write start index to the current out row. */
    out->i[y] = o;

    /* Shorthands for the indexes to the next in rows. */
    i1_next = in1->i[y + 1];
    i2_next = in2->i[y + 1];

    /* The loop (in row) where the real comparison is done. */
    while (i1 < i1_next && i2 < i2_next) {
      if (in1->j[i1] == in2->j[i2]) {
	/* Maximum of two non-zero elements is always non-zero. */
	CSRd_reserve_block(out, o + 1);
	out->j[o] = in1->j[i1];
	out->a[o] = MAX2(in1->a[i1], in2->a[i2]);
	o++;
	i1++;
	i2++;
      } else if (in1->j[i1] < in2->j[i2]) {
	/* max(a1, 0) is non-zero whenever a1 > 0. */
	if (in1->a[i1] > 0.0) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in1->j[i1];
	  out->a[o] = in1->a[i1];
	  o++;
	}
	i1++;
      } else if (in1->j[i1] > in2->j[i2]) {
	/* max(a2, 0) is non-zero whenever a2 > 0. */
	if (in2->a[i2] > 0.0) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in2->j[i2];
	  out->a[o] = in2->a[i2];
	  o++;
	}
	i2++;
      }
    }

    /* The rest in current row. */
    if (i1 < i1_next) {
      while (i1 < i1_next) {
	if (in1->a[i1] > 0.0) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in1->j[i1];
	  out->a[o] = in1->a[i1];
	  o++;
	}
	i1++;
      }
    } else if (i2 < i2_next) {
      while (i2 < i2_next) {
	if (in2->a[i2] > 0.0) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in2->j[i2];
	  out->a[o] = in2->a[i2];
	  o++;
	}
	i2++;
      }
    }
  }

  /* Write start index to the (y + 1):th out row. */
  out->i[h] = o;
}

void
CSRd_elementwise_lt(CSRd out, const CSRd in1, const CSRd in2)
{
  /* Reuse, cause (in1 > in2) <=> (in2 < in1). */
  CSRd_elementwise_gt(out, in2, in1);
}

void
CSRd_elementwise_gt(CSRd out, const CSRd in1, const CSRd in2)
{
  int y, w, h;
  int o, i1, i2, i1_next, i2_next;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (out == in1 || out == in2) {
    leprintf("out cannot equal in1 nor in2.\n");
    return;
  }
  if (!check_pointwise_binary_dims(out, in1, in2)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  /* Shorthands. */
  w = in1->w;
  h = in1->h;

  /* First row always starts at index 0. */
  o = 0;
  i1 = 0;
  i2 = 0;

  /* For all rows. */
  for (y = 0; y < h; y++) {

    /* Write start index to the current out row. */
    out->i[y] = o;

    /* Shorthands for the indexes to the next in rows. */
    i1_next = in1->i[y + 1];
    i2_next = in2->i[y + 1];

    /* The loop (in row) where the real comparison is done. */
    while (i1 < i1_next && i2 < i2_next) {
      if (in1->j[i1] == in2->j[i2]) {
	/* whenever in1 > in2. */
	if (in1->a[i1] > in2->a[i2]) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in1->j[i1];
	  out->a[o] = 1.0;
	  o++;
	}
	i1++;
	i2++;
      } else if (in1->j[i1] < in2->j[i2]) {
	/* whenever in1 > 0. */
	if (in1->a[i1] > 0.0) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in1->j[i1];
	  out->a[o] = 1.0;
	  o++;
	}
	i1++;
      } else if (in1->j[i1] > in2->j[i2]) {
	/* whenever in2 < 0. */
	if (0.0 > in2->a[i2]) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in2->j[i2];
	  out->a[o] = 1.0;
	  o++;
	}
	i2++;
      }
    }

    /* The rest in current row. */
    if (i1 < i1_next) {
      while (i1 < i1_next) {
	if (in1->a[i1] > 0.0) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in1->j[i1];
	  out->a[o] = 1.0;
	  o++;
	}
	i1++;
      }
    } else if (i2 < i2_next) {
      while (i2 < i2_next) {
	if (0.0 > in2->a[i2]) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in2->j[i2];
	  out->a[o] = 1.0;
	  o++;
	}
	i2++;
      }
    }
  }

  /* Write start index to the (y + 1):th out row. */
  out->i[h] = o;
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Pointwise dcsr-double Comparison.
 */
void
CSRd_elementwise_eq_dbl(CSRd out, const CSRd in, double scalar)
{
  int x, y, w, h, o, i, i_next, n_in;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (out == in) {
    leprintf("out cannot equal in.\n");
    return;
  }
  if (!check_pointwise_unary_dims(out, in)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  /* Shorthands. */
  w = in->w;
  h = in->h;
  n_in = CSRd_nnz(in);

  /* First row always starts at index 0. */
  o = 0;
  i = 0;

  if (scalar == 0.0) {
    /* Exact prediction of nnz's in out is possible. */
    int n_out = w * h - n_in;
    CSRd_reserve_block(out, n_out);
    darray_ones(out->a, n_out);

    /* For all rows. */
    for (y = 0; y < h; y++) {
      /* Write start index to the current out row. */
      out->i[y] = o;

      /* Shorthands for the indexes to the next in rows. */
      i_next = in->i[y + 1];

      /* The loop where the real arithmetic is done. */
      for (x = 0; x < w; x++)
	if (i < i_next)		/* If non-zeros are left in current in row. */
	  if (x != in->j[i]) {
	    out->j[o] = x;
	    o++;
	  } else		/* Avoid non-zero element. */
	    i++;
	else {			/* Otherwise set a one. */
	  out->j[o] = x;
	  o++;
	}
    }

  } else {

    /* For all rows. */
    for (y = 0; y < h; y++) {
      /* Write start index to the current out row. */
      out->i[y] = o;

      /* Shorthands for the indexes to the next in rows. */
      i_next = in->i[y + 1];

      /* The loop where the real arithmetic is done. */
      while (i < i_next) {
	if (in->a[i] == scalar) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in->j[i];
	  out->a[o] = 1.0;
	  o++;
	}
	i++;
      }
    }

  }

  /* Write start index to the (y + 1):th out row. */
  out->i[h] = o;
}

void
CSRd_elementwise_lt_dbl(CSRd out, const CSRd in, double scalar)
{
  int i, i_next, o, x, y, w, h;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (!check_pointwise_unary_dims(out, in)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  /* Shorthands. */
  w = in->w;
  h = in->h;

  /* First row always starts at index 0. */
  o = 0;
  i = 0;

  if (scalar <= 0.0) {

    /* For all rows. */
    for (y = 0; y < h; y++) {

      /* Write start index to the current out row. */
      out->i[y] = o;

      /* Shorthands for the indexes to the next in rows. */
      i_next = in->i[y + 1];

      /* For all non-zero elements in current in-row. */
      while (i < i_next) {
	/* If in < scalar then set 1. */
	if (in->a[i] < scalar) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in->j[i];
	  out->a[o] = 1.0;
	  o++;
	}
	i++;
      }
    }

  } else {

    /* For all rows. */
    for (y = 0; y < h; y++) {

      /* Write start index to the current out row. */
      out->i[y] = o;

      /* Shorthands for the indexes to the next in rows. */
      i_next = in->i[y + 1];

      for (x = 0; x < w; x++) {
	if (x == in->j[i]) {
	  if (in->a[i] < scalar) {
	    CSRd_reserve_block(out, o + 1);
	    out->j[o] = in->j[i];
	    out->a[o] = 1.0;
	    o++;
	  }
	  i++;
	} else {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in->j[i];
	  out->a[o] = 1.0;
	  o++;
	}
      }
    }
  }

  /* Write start index to the (y + 1):th out row. */
  out->i[h] = o;
}

void
CSRd_elementwise_gt_dbl(CSRd out, const CSRd in, double scalar)
{
  int i, i_next, o, x, y, w, h;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (!check_pointwise_unary_dims(out, in)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  /* Shorthands. */
  w = in->w;
  h = in->h;

  /* First row always starts at index 0. */
  o = 0;
  i = 0;

  if (scalar >= 0.0) {

    /* For all rows. */
    for (y = 0; y < h; y++) {

      /* Write start index to the current out row. */
      out->i[y] = o;

      /* Shorthands for the indexes to the next in rows. */
      i_next = in->i[y + 1];

      /* For all non-zero elements in current in-row. */
      while (i < i_next) {
	/* If in1 > scalar then set 1. */
	if (in->a[i] > scalar) {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in->j[i];
	  out->a[o] = 1.0;
	  o++;
	}
	i++;
      }
    }

  } else {

    /* For all rows. */
    for (y = 0; y < h; y++) {

      /* Write start index to the current out row. */
      out->i[y] = o;

      /* Shorthands for the indexes to the next in rows. */
      i_next = in->i[y + 1];

      for (x = 0; x < w; x++) {
	if (x == in->j[i]) {
	  if (in->a[i] > scalar) {
	    CSRd_reserve_block(out, o + 1);
	    out->j[o] = in->j[i];
	    out->a[o] = 1.0;
	    o++;
	  }
	  i++;
	} else {
	  CSRd_reserve_block(out, o + 1);
	  out->j[o] = in->j[i];
	  out->a[o] = 1.0;
	  o++;
	}
      }
    }

  }

  /* Write start index to the (y + 1):th out row. */
  out->i[h] = o;
}

void
CSRd_elementwise_min_dbl(CSRd out, const CSRd in, double scalar)
{
  int i, i_next, o, x, y, w, h, n;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (!check_pointwise_unary_dims(out, in)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  n = CSRd_nnz(in);
  w = in->w;
  h = in->h;

  if (scalar > 0.0) {

    /* out will get the same sparseness as in. */
    CSRd_make_same(out, in);

    /* Reuse array version. */
    darray_min2_dbl(out->a, in->a, scalar, n);

  } else if (scalar == 0.0) {

    /* The elements of out will become at least as sparse as in. */
    CSRd_make_same(out, in);

    /* First row always starts at index 0. */
    o = 0;
    i = 0;

    /* For all rows. */
    for (y = 0; y < h; y++) {

      /* Write start index to the current out row. */
      out->i[y] = o;

      /* Shorthands for the indexes to the next in rows. */
      i_next = in->i[y + 1];

      /* For all non-zero elements in current in-row. */
      while (i < i_next) {
	/* min(a, 0) is non-zero only when a < 0. */
	if (in->a[i] < 0.0) {
	  out->j[o] = in->j[i];
	  out->a[o] = in->a[i];
	  o++;
	}
	i++;
      }
    }

    /* Write start index to the (y + 1):th out row. */
    out->i[h] = o;

  } else {			/* out's elements will all be non-zero. */

    /* The elements of out will all be non-zero. */
    CSRd_reserve_block(out, w * h);

    /* First row always starts at index 0. */
    o = 0;
    i = 0;

    /* For all rows. */
    for (y = 0; y < h; y++) {

      /* Write start index to the current out row. */
      out->i[y] = o;

      /* For all positions in the current out row. */
      for (x = 0; x < w; x++) {
	out->j[o] = x;
	if (x == in->j[i]) {	/* If the current position is non-zero */
	  if (in->a[i] < scalar)	/* and if its element is < scalar */
	    out->a[o] = in->a[i];	/* write the element, */
	  else			/* otherwise */
	    out->a[o] = scalar;	/* write the scalar. */
	  i++;
	} else
	  out->a[o] = scalar;
	o++;
      }
    }

    /* Write start index to the (y + 1):th out row. */
    out->i[h] = o;

  }
}

void
CSRd_elementwise_max_dbl(CSRd out, const CSRd in, double scalar)
{
  int i, i_next, o, x, y, w, h, n;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (!check_pointwise_unary_dims(out, in)) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  n = CSRd_nnz(in);
  w = in->w;
  h = in->h;

  if (scalar < 0.0) {

    /* out will get the same sparseness as in. */
    CSRd_make_same(out, in);

    /* Reuse array version. */
    darray_max2_dbl(out->a, in->a, scalar, n);

  } else if (scalar == 0.0) {

    /* The elements of out will become at least as sparse as in. */
    CSRd_make_same(out, in);

    /* First row always starts at index 0. */
    o = 0;
    i = 0;

    /* For all rows. */
    for (y = 0; y < h; y++) {

      /* Write start index to the current out row. */
      out->i[y] = o;

      /* Shorthands for the indexes to the next in rows. */
      i_next = in->i[y + 1];

      /* For all non-zero elements in current in-row. */
      while (i < i_next) {
	/* max(a, 0) is non-zero only when a > 0. */
	if (in->a[i] > 0.0) {
	  out->j[o] = in->j[i];
	  out->a[o] = in->a[i];
	  o++;
	}
	i++;
      }
    }

    /* Write start index to the (y + 1):th out row. */
    out->i[h] = o;

  } else {			/* out's elements will all be non-zero. */

    /* The elements of out will all be non-zero. */
    CSRd_reserve_block(out, w * h);

    /* First row always starts at index 0. */
    o = 0;
    i = 0;

    /* For all rows. */
    for (y = 0; y < h; y++) {

      /* Write start index to the current out row. */
      out->i[y] = o;

      /* For all positions in the current out row. */
      for (x = 0; x < w; x++) {
	out->j[o] = x;
	if (x == in->j[i]) {	/* If the current position is non-zero */
	  if (in->a[i] > scalar)	/* and if its element is < scalar */
	    out->a[o] = in->a[i];	/* write the element, */
	  else			/* otherwise */
	    out->a[o] = scalar;	/* write the scalar. */
	  i++;
	} else
	  out->a[o] = scalar;
	o++;
      }
    }

    /* Write start index to the (y + 1):th out row. */
    out->i[h] = o;

  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
CSRd_mmmul_tr(CSRd out, const CSRd in1, const CSRd in2)
{
  int y1, y2, w, h1, h2;
  int o, i1, i1a, i2, i1_next, i2_next;
  double sum;

#if NDEBUG
  /* \TODO: Move elsewhere. */
  if (out == in1 || out == in2) {
    leprintf("out cannot equal in1 nor in2.\n");
    return;
  }
  if (out->w != in2->h || out->h != in1->h || in1->w != in2->w) {
    leprintf("argument dimension mismatch.\n");
    return;
  }
#endif

  /* Shorthands. */
  w = in1->w;
  h1 = in1->h;
  h2 = in2->h;

  /* First out and in1 row always starts at index 0. */
  o = 0;
  i1 = 0;

  /* Start estimation of out's nnz. */
  {
/*          int n_in1, n_in2, n_out; */
/*          double p_in1, p_in2, p_out; */

/*          n_in1 = CSRd_nnz(in1); */
/*          n_in2 = CSRd_nnz(in2); */

    /* Probabilities for an element in in1 and in2 being zero. */
/*          p_in1 = (double)n_in1 / w / h1; */
/*          p_in2 = (double)n_in2 / w / h2; */

/*          p_out = 1.0 - pow((1 - p_in1) * (1 - p_in2), w); */
/*          n_out = p_out * h1 * h2; */

/*          printf("Est  n_out: %d\n", n_out); */

/*          CSRd_reserve_block_p1(out, 1); */
  }

  /* For all in1's rows. */
  for (y1 = 0; y1 < h1; y1++) {

    /* Start index to the current out row is row in left-hand-side
     * matrix. */
    out->i[y1] = o;

    /* Shorthands for the indexes to the next in1 rows. */
    i1_next = in1->i[y1 + 1];

    /* First in2 row always starts at index 0. */
    i2 = 0;
    /* For all in2's rows. */
    for (y2 = 0; y2 < h2; y2++) {

      /* Go back to current in1 row. */
      i1a = i1;

      /* Shorthands for the indexes to the next in2's rows. */
      i2_next = in2->i[y2 + 1];

      /* The loop where the real arithmetic is done. */
      sum = 0;
      while (i1a < i1_next && i2 < i2_next) {
	if (in1->j[i1a] == in2->j[i2]) {
	  sum += in1->a[i1a] * in2->a[i2];
	  i1a++;
	  i2++;
	} else if (in1->j[i1a] < in2->j[i2]) {
	  i1a++;
	} else if (in1->j[i1a] > in2->j[i2]) {
	  i2++;
	}
      }

      /* Column index is row in right-hand-side matrix. */
      if (sum != 0) {
	CSRd_reserve_block(out, o + 1);
	out->j[o] = y2;
	out->a[o] = sum;
	o++;
      }

      /* Next in2 row. */
      i2 = i2_next;
    }

    /* Next in1 row. */
    i1 = i1_next;

  }

  /* Write start index to the (y + 1):th out row. */
  out->i[h1] = o;
}

void
CSRd_mmmul(CSRd out, const CSRd in1, const CSRd in2)
{
  CSRd in2_tr;
  CSRd_init_zeros(in2_tr, in2->h, in2->w);

  /* Reuse! */
  CSRd_transpose(in2_tr, in2);
  CSRd_mmmul_tr(out, in1, in2_tr);

  CSRd_clear(in2_tr);
}

void
CSRd_mvmul_f64a(double *out, const CSRd in1, const double *in2)
{
  int y, h, i1, i1_next;
  double dot;

#if NDEBUG
  /* \TODO: Move elsewhere. */
#endif

  /* Shorthands. */
  h = in1->h;

  /* First row always starts at index 0. */
  i1 = 0;

  /* For all rows. */
  for (y = 0; y < h; y++) {

    /* Shorthands for the indexes to the next in rows. */
    i1_next = in1->i[y + 1];

    dot = 0.0;

    /* The loop where the real arithmetic is done. */
    while (i1 < i1_next) {
      dot += in1->a[i1] * in2[in1->j[i1]];
      i1++;
    }
    out[y] = dot;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

double
CSRd_xy_sum(const CSRd in)
{
  return darray_sum(in->a, CSRd_nnz(in));
}

double
CSRd_xy_product(const CSRd in)
{
  return darray_prod(in->a, CSRd_nnz(in));
}

double
CSRd_xy_dot(const CSRd in1, const CSRd in2)
{
  int y, h, i1, i2, i1_next, i2_next;
  double sum;

#if NDEBUG
  /* \TODO: Move elsewhere. */
#endif

  /* Shorthands. */
  h = in1->h;

  /* First row always starts at index 0. */
  i1 = 0;
  i2 = 0;

  sum = 0.0;

  /* For all rows. */
  for (y = 0; y < h; y++) {

    /* Shorthands for the indexes to the next in rows. */
    i1_next = in1->i[y + 1];
    i2_next = in2->i[y + 1];

    /* The loop where the real arithmetic is done. */
    while (i1 < i1_next && i2 < i2_next) {
      if (in1->j[i1] == in2->j[i2]) {
	sum += in1->a[i1] * in2->a[i2];
	i1++;
	i2++;
      } else if (in1->j[i1] < in2->j[i2]) {
	i1++;
      } else if (in1->j[i1] > in2->j[i2]) {
	i2++;
      }
    }
  }

  return sum;
}

/* ---------------------------- Group Separator ---------------------------- */

void
CSRd_x_sum(CSRd out, const CSRd in)
{
  int o, i, y, h, i_next;
  double sum;

  /* Shorthands. */
  h = in->h;

  /* Make room for one dense column vector. */
  CSRd_reserve_block(out, h);

  /* First row always starts at index 0. */
  o = 0;
  i = 0;

  /* For all rows. */
  for (y = 0; y < h; y++) {

    /* Write start index to the current out row. */
    out->i[y] = o;

    /* Shorthand for the index to the next in row. */
    i_next = in->i[y + 1];

    /* Reuse! */
    sum = darray_sum(in->a + i, i_next - i);
    i = i_next;

    if (sum != 0.0) {		/* If non-zero element. */
      out->j[o] = 0;		/* At the first column in the current row. */
      out->a[o] = sum;		/* Write sum. */
      o++;			/* Next out row, please. */
    }

  }

  /* Write start index to the (y + 1):th out row. */
  out->i[h] = o;
}
