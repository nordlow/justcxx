/*!
 * \file image.hpp
 * \brief Two-Dimensional Image.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "algorithm_x.hpp"
#include "encode.hpp"
#include "decode.hpp"

// ============================================================================

/*!
 * Two-dimensional Image
 *
 */
template<class T>
class image2
{
public:
    image2(size_t m, size_t n, const T& bg = 0)
        : m(m), n(n), bg(bg)
    {
	img = new T* [m];
	img[0] = new T [m * n];
	for (size_t i = 0; i < m; i++)
	{
	    img[i] = img[0] + i * n;
	}
    }

    ~image2()
    {
	delete img[0];
	delete img;
    }

    // element access

    T  operator() (size_t i, size_t j) const { return img[i][j]; }
    T& operator() (size_t i, size_t j)       { return img[i][j]; }

    // whole access

    void operator = (const T& color)
    {
	for (size_t i = 0; i < m; i++)
	{
	    for (size_t j = 0; j < n; j++)
	    {
		(*this)(i,j) = color;
	    }
	}
    }

    // whole roi access

    void fill(const Roi& roi, const T& color)
    {
	const size_t i0 = roi.i, j0 = roi.j;
	const size_t m = roi.m, n = roi.n;

	for (size_t i = 0; i < m; i++)
	{
	    for (size_t j = 0; j < n; j++)
	    {
		(*this)(i0 + i,
			j0 + j) = color; // \todo Optimize...
	    }
	}
    }

    void clear()
    {
	for (size_t i = 0; i < m; i++)
	{
	    for (size_t j = 0; j < n; j++)
	    {
		(*this)(i, j) = bg; // \todo Optimize...
	    }
	}
    }

    void draw_box(const Roi& roi, const T& fg, const T& bg)
    {
	const size_t i0 = roi.i, j0 = roi.j;
	const size_t m = roi.m, n = roi.n;

	for (size_t i = 0; i < m; i++)
	{
	    for (size_t j = 0; j < n; j++)
	    {
		if (i == 0 or i == m-1 or
		    j == 0 or j == n-1)
		{
		    (*this)(i0 + i,
			    j0 + j) = fg;
		}
		else
		{
		    (*this)(i0 + i,
			    j0 + j) = bg;
		}
	    }
	}
    }

    size_t n_rows() const { return m; }
    size_t n_cols() const { return n; }

private:
    // \todo REPlace with size_t with ushort.
    size_t m, n; ///< The number of rows and columns of the image.
    T **img;                   ///< The image data.
    T bg;                      ///< The image background.
};
