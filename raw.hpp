/*! \file bits.hpp
 * \brief Raw Bits.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-02-03 16:10
 */

#pragma once
//#include <boost/endian/integers.hpp>
#include <bitset>
#include <tuple>

namespace boost {
namespace raw {

/*! Sequence of N Bits.
 * \todo Merge with \c semnet::patterns::lit
 */
template<size_t N>
inline std::bitset<N> bits()
{
    std::bitset<N> x;
    return x;
}

/*!
 * Data Sequence.
 *
 * \todo Merge with \c semnet::patterns::seq
 *
 * \todo Turn into a class \c bitseq.
 * \todo Require its arguments to have bytesize() and bitsize() member functions
 * via a type_trait has_member or SFINAE as
 *
 * \todo Use std::bitset
 *
 * \see http://stackoverflow.com/questions/2122319/c-type-traits-to-check-if-class-has-operator-member
 * \see http://stackoverflow.com/questions/257288/is-it-possible-to-write-a-c-template-to-check-for-a-functions-existence.
 */
template<class ... T>
std::tuple<T...> seq(T&... t)
{
    return std::make_tuple(t...);
}


}
}
