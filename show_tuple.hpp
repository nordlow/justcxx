/*! \file show_tuple.hpp
 * \brief Show std::tuple.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "show.hpp"
#include <tuple>

// \see http://stackoverflow.com/questions/6245735/pretty-print-stdtuple
// \see http://ideone.com/a9UOR

template<class T, std::size_t N>
struct TuplePrinter {
    static void print(std::ostream& os, const T& t) { TuplePrinter<T,N-1>::print(os, t); os << ","; show(os, std::get<N-1>(t)); }
};
template<class T>
struct TuplePrinter<T, 1> {
    static void print(std::ostream& os, const T& t) { show(os, std::get<0>(t)); }
};

template<class T, std::size_t N>
struct TupleTypesPrinter {
    static void print(std::ostream& os, const T& t) { TupleTypesPrinter<T,N-1>::print(os, t); os << "," << typenameof<T>(); }
};
template<class T>
struct TupleTypesPrinter<T, 1> {
    static void print(std::ostream& os, const T& t) { os << typenameof<T>(); }
};

#if HAVE_CXX11_VARIADIC_TEMPLATES

template<class... Ts>
std::ostream& show_each(const std::tuple<Ts...>& t,
                        std::ostream& os = std::cout,
                        const char* opening = "(",
                        const char* closing = ")",
                        const char* separator = ",") {
    typedef const std::tuple<Ts...>& tuple_t;
    os << opening;
    TuplePrinter<tuple_t,sizeof...(Ts)>::print(os,t);
    return os << closing;
}

template<class T, class... R>
inline std::ostream& show(std::ostream& os, const std::tuple<T,R...>& v) {
    show_each(v, os);
    os << " : std::tuple<";
    typedef const std::tuple<T,R...>& tuple_t;
    TupleTypesPrinter<tuple_t,1+sizeof...(R)>::print(os,v);
    os << ">";
    return os;
}

template<class T, class... R>
inline std::ostream& operator << (std::ostream& os, const std::tuple<T,R...>& v) {
    return show(os, v);
}

#endif
