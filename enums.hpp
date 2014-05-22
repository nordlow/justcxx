/*! \file enums.hpp
 * \brief Enumeration types.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \todo Replace/Merge with boost::smart_enum.
 */

#pragma once
#include "utils.hpp"
#include "encode.hpp"
#include "decode.hpp"

/*! An enumeration of arbitrary type.
 * Used mainly as a serialization wrapper for integers.
 */
template<class T>
class Enum
{
public:
    Enum(T a = 0) : val(a) {}
    /// Construct an enumeration from an istream.
    Enum(std::istream & is) { ::decode(is, val); }
    // Return the enumeration value.
    T get_value() const { return val; }
    // Set the enumeration value.
    void set_value(T a) { val = a; }
private:
    T val;			///< The enumeration value.
};

/*! Encode. */
template<class T>
inline void encode(std::ostream & os, Enum<T> a) { encode(os, a.get_value()); }

/*! Decode. */
template<class T> inline void decode(std::istream & is, Enum<T> & a) { T b; decode(is, b); a.set_value(b); }

typedef Enum<uint8_t> Enum256;
typedef Enum<uint16_t> Enum65536;
