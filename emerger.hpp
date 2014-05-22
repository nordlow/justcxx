/*!
 * \file emerger.hpp
 * \brief Emergering class.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * Base clustering class for constructing emergent behaviour algorithms.
 */

#pragma once

#include <boost/array.hpp>
#include "rand.hpp"
#include "algorithm_x.hpp"
#include "clamp.hpp"

#include <algorithm>
#include <vector>

/*!
 * Emerger of N-dimensional points with element type T.
 */
template<class T, std::size_t N>
class emerger
{
public:
    typedef boost::array<T, N> Point;
    typedef std::vector<Point> Points;
public:
    /***
     * Create an emerger with nd data and nm models.
     *
     * \param[in] nm is truncated to nd if necessary because every model needs at
     *        least one of data point to exist.
     */
    emerger(size_t nd, size_t nm)
	: data(nd, 0),
	  model(std::min(nd, nm), 0)
    {}

    /***
     * Randomize stuff.
     */
    void rrand_data(const T & l,
			   const T & h)
    {
	for (auto i = begin(data); i != end(data); i++)
	{
	    for (size_t j = 0; j < N; j++)
	    {
		(*i)[j] = pnw::rrand<T>(l, h);
	    }
	}
    }

    /***
     * Perform one emergering iteration.
     */
    void iterate()
    {
    }

    void print(std::ostream & os) const
    {
	os << "data: " << data << " "
	   << "model: " << model << std::endl;
    }

    /***
     * Read data into the vector \p a along dimension dim in data.
     */
    void read_data_along(std::vector<T> & a, size_t dim) const
    {
	dim = pnw::upper_clamp(dim, N);
	a.resize(data.size());

	for (size_t i = 0; i < a.size(); i++)
	{
	    a[i] = data[i][dim];
	}
    }

    size_t get_nd() const { return data.size(); }
    size_t get_nm() const { return model.size(); }

private:
    Points data;
    Points model;
};

template<class T, std::size_t N>
inline std::ostream & operator << (std::ostream & os,
				   const emerger<T, N> & a)
{
    a.print(os); return os;
}
