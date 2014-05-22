/*! \file backwards.hpp
 * \brief Backward Iterator.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-05-16 21:51
 *
 * \see \c BOOST_REVERSE_FOREACH
 */

#pragma once

template<class T>
class iterate_backwards {
public:
    explicit iterate_backwards(const T &t) : t(t) {}
    typename T::const_reverse_iterator begin() const { return t.rbegin(); }
    typename T::const_reverse_iterator end()   const { return t.rend(); }
private:
    const T &t;
};

template<class T>
iterate_backwards backwards(const T &t) {
    return iterate_backwards(t);
}
