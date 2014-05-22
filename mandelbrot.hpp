/*!
 * \file mandelbrot.hpp
 * \brief Mandelbrot Fractal.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include <complex>
#include <armadillo>

/*!
 * Generate a mandelbrot fractal.
 *
 * \param[out] out is the buffer to write to.
 * \param[in] m is the number of rows.
 * \param[in] n is the number of columns.
 * \param[in] iter_max is the maximum number of iterations.
 */
template<typename T>
void mandelbrot(uchar **out, size_t m, size_t n,
                uchar iter_max,
                const T & re_min = -2.25, const T & re_max = 0.75,
                const T & im_min = -1.25, const T & im_max = 1.25)
{
    T z0_re, z0_im;

    const T step_im = (im_max - im_min) / m;
    const T step_re = (re_max - re_min) / n;

    for (size_t i = 0, z0_im = im_min; i < m; i++, z0_im += step_im)
    {
        for (size_t j = 0, z0_re = re_min; j < n; j++, z0_re += step_re)
        {
            T zi_re = z0_re;
            T zi_im = z0_im;

            T zi_re2;
            T zi_im2;

            int count = 0;
            while (((zi_re2 = zi_re * zi_re) +
                    (zi_im2 = zi_im * zi_im) <= 4.0) and
                   (count++ < iter_max))
            {
                zi_re = zi_re2 - zi_im2 + z0_re;
                zi_im = 2 * zi_re * zi_im + z0_im;
            }

            if (count >= iter_max)
            {
                out[i][j] = 0;
            }
            else
            {
                out[i][j] = (uchar) count;
            }
        }
    }
}

/*!
 * Generate a mandelbrot fractal.
 *
 * \param[out] out is the buffer to write to.
 * \param[in] m is the number of rows.
 * \param[in] n is the number of columns.
 * \param[in] iter_max is the maximum number of iterations.
 */
template<typename T>
void mandelbrot(arma::Mat<uchar> out,
                uchar iter_max = 128,
                const T & value_limit = 4.0,
                const std::complex<T> & min = std::complex<T> (-2.25, 0.75),
                const std::complex<T> & max = std::complex<T> (-1.25, 1.25))
{
    typedef std::complex<T> CTp;

    const size_t m = out.n_rows(), n = out.n_cols();

    const CTp step = ((max.real() - min.real()) / m,
                      (max.imag() - min.imag()) / n);

    CTp z0 = min;

    for (size_t i = 0; i < m; i++, z0 += CTp (0, step.imag()))
    {
        z0 = CTp (min.real(), z0.imag());
        for (size_t j = 0; j < n; j++, z0 += CTp (step.real(), 0))
        {
            CTp zi = z0;
            size_t iter = 0;
            while (iter++ < iter_max and
                   zi * conj(zi) <= value_limit)
            {
                zi = pnw::square (zi) + z0;
            }
            if (iter >= iter_max)
            {
                out(i,j) = 0;
            }
            else
            {
                out(i,j) = (uchar) iter;
            }
        }
    }
}
