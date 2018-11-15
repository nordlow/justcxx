#pragma once

#include <string>

typedef double Td;

namespace splines {

  Td basis_function_b_val ( Td tdata[], Td tval );
  Td basis_function_beta_val ( Td beta1, Td beta2, Td tdata[],
                               Td tval );
  Td *basis_matrix_b_uni ( );
  Td *basis_matrix_beta_uni ( Td beta1, Td beta2 );
  Td *basis_matrix_bezier ( );
  Td *basis_matrix_hermite ( );
  Td *basis_matrix_overhauser_nonuni ( Td alpha, Td beta );
  Td *basis_matrix_overhauser_nul ( Td alpha );
  Td *basis_matrix_overhauser_nur ( Td beta );
  Td *basis_matrix_overhauser_uni ( void);
  Td *basis_matrix_overhauser_uni_l ( );
  Td *basis_matrix_overhauser_uni_r ( );
  Td basis_matrix_tmp ( int left, int n, Td mbasis[], int ndata,
                        Td tdata[], Td ydata[], Td tval );
  void bc_val ( int n, Td t, Td xcon[], Td ycon[], Td *xval,
                Td *yval );
  Td bez_val ( int n, Td x, Td a, Td b, Td y[] );
  Td bp_approx ( int n, Td a, Td b, Td ydata[], Td xval );
  Td *bp01 ( int n, Td x );
  Td *bpab ( int n, Td a, Td b, Td x );
  int chfev ( Td x1, Td x2, Td f1, Td f2, Td d1, Td d2,
              int ne, Td xe[], Td fe[], int next[] );
  int d3_fs ( Td a1[], Td a2[], Td a3[], int n, Td b[], Td x[] );
  Td *d3_mxv ( int n, Td a[], Td x[] );
  Td *d3_np_fs ( int n, Td a[], Td b[] );
  void d3_print ( int n, Td a[], std::string title );
  void d3_print_some ( int n, Td a[], int ilo, int jlo, int ihi, int jhi );
  Td *d3_uniform ( int n, int *seed );
  void data_to_dif ( int ntab, Td xtab[], Td ytab[], Td diftab[] );
  Td dif_val ( int ntab, Td xtab[], Td diftab[], Td xval );
  int i4_max ( int i1, int i2 );
  int i4_min ( int i1, int i2 );
  void least_set ( int point_num, Td x[], Td f[], Td w[],
                   int nterms, Td b[], Td c[], Td d[] );
  Td least_val ( int nterms, Td b[], Td c[], Td d[],
                 Td x );
  void least_val2 ( int nterms, Td b[], Td c[], Td d[], Td x,
                    Td *px, Td *pxp );
  void least_set_old ( int ntab, Td xtab[], Td ytab[], int ndeg,
                       Td ptab[], Td b[], Td c[], Td d[], Td *eps, int *ierror );
  Td least_val_old ( Td x, int ndeg, Td b[], Td c[], Td d[] );
  void parabola_val2 ( int ndim, int ndata, Td tdata[], Td ydata[],
                       int left, Td tval, Td yval[] );
  Td pchst ( Td arg1, Td arg2 );
  Td r8_max ( Td x, Td y );
  Td r8_min ( Td x, Td y );
  Td r8_uniform_01 ( int *seed );
  void r8vec_bracket ( int n, Td x[], Td xval, int *left, int *right );
  void r8vec_bracket3 ( int n, Td t[], Td tval, int *left );
  Td *r8vec_even ( int n, Td alo, Td ahi );
  Td *r8vec_indicator ( int n );
  void r8vec_order_type ( int n, Td x[], int *order );
  void r8vec_print ( int n, Td a[], std::string title );
  void r8vec_sort_bubble_a ( int n, Td a[] );
  Td *r8vec_uniform ( int n, Td b, Td c, int *seed );
  int r8vec_unique_count ( int n, Td a[], Td tol );
  void r8vec_zero ( int n, Td a[] );
  int s_len_trim ( std::string s );
  Td spline_b_val ( int ndata, Td tdata[], Td ydata[], Td tval );
  Td spline_beta_val ( Td beta1, Td beta2, int ndata, Td tdata[],
                       Td ydata[], Td tval );
  Td spline_constant_val ( int ndata, Td tdata[], Td ydata[], Td tval );
  Td *spline_cubic_set ( int n, Td t[], Td y[], int ibcbeg, Td ybcbeg,
                         int ibcend, Td ybcend );
  Td spline_cubic_val ( int n, Td t[], Td tval, Td y[], Td ypp[],
                        Td *ypval, Td *yppval );
  void spline_cubic_val2 ( int n, Td t[], Td tval, int *left, Td y[],
                           Td ypp[], Td *yval, Td *ypval, Td *yppval );
  Td *spline_hermite_set ( int ndata, Td tdata[], Td ydata[],
                           Td ypdata[] );
  void spline_hermite_val ( int ndata, Td tdata[], Td c[], Td tval,
                            Td *sval, Td *spval );
  Td spline_linear_int ( int ndata, Td tdata[], Td ydata[], Td a,
                         Td b );
  void spline_linear_intset ( int int_n, Td int_x[], Td int_v[],
                              Td data_x[], Td data_y[] );
  void spline_linear_val ( int ndata, Td tdata[], Td ydata[],
                           Td tval, Td *yval, Td *ypval );
  Td spline_overhauser_nonuni_val ( int ndata, Td tdata[],
                                    Td ydata[], Td tval );
  Td spline_overhauser_uni_val ( int ndata, Td tdata[], Td ydata[],
                                 Td tval );
  void spline_overhauser_val ( int ndim, int ndata, Td tdata[], Td ydata[],
                               Td tval, Td yval[] );
  void spline_pchip_set ( int n, Td x[], Td f[], Td d[] );
  void spline_pchip_val ( int n, Td x[], Td f[], Td d[], int ne,
                          Td xe[], Td fe[] );
  void spline_quadratic_val ( int ndata, Td tdata[], Td ydata[],
                              Td tval, Td *yval, Td *ypval );
  void timestamp ( );

}
