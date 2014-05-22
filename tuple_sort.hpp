/*! \file: tuple_sort.hpp
 * \brief: Tuple Sorting.
 * \author: Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date: 2012-06-23 18:28
 * \see https://gist.github.com/2975962
 * \see http://www.reddit.com/r/cpp/comments/vgn49/i_was_bored_variadic_template_based_tuple_sorting/
 */

#pragma once
#include <utility>
#include <tuple>

// Forward Declarations
template<int N1, int N2, int LAST, typename T>
inline typename std::enable_if<N1 + 1 == LAST and N2 == LAST, void>::type order_helper(T&& t);
template<int N1, int N2, int LAST, typename T>
inline typename std::enable_if<N1 + 1 == LAST and N2 < LAST, void>::type order_helper(T&& t);
template<int N1, int N2, int LAST, typename T>
inline typename std::enable_if<N1 + 1 < LAST and N2 == LAST, void>::type order_helper(T&& t);
template<int N1, int N2, int LAST, typename T>
inline typename std::enable_if<N1 + 1 < LAST and N2 < LAST, void>::type order_helper(T&& t);

/// N + 1 = last, N2 = last
template<int N1, int N2, int LAST, typename T>
inline typename std::enable_if<N1 + 1 == LAST and N2 == LAST, void>::type
order_helper(T&& t) {
    if (std::get<N1>(t) > std::get<N2>(t))
        std::swap(std::get<N1>(t), std::get<N2>(t));
}

/// N + 1 = last, N2 < last
template<int N1, int N2, int LAST, typename T>
inline typename std::enable_if<N1 + 1 == LAST and N2 < LAST, void>::type
order_helper(T&& t) {
    if (std::get<N1>(t) > std::get<N2>(t))
        std::swap(std::get<N1>(t), std::get<N2>(t));
    order_helper<N1, N2 + 1, LAST>(t);
}

/// N1 + 1 < last, N2 = last
template<int N1, int N2, int LAST, typename T>
inline typename std::enable_if<N1 + 1 < LAST and N2 == LAST, void>::type
order_helper(T&& t) {
    if (std::get<N1>(t) > std::get<N2>(t))
        std::swap(std::get<N1>(t), std::get<N2>(t));
    order_helper<N1 + 1, N1 + 2, LAST>(t);
}

/// N1 + 1 < last, N1 < last
template<int N1, int N2, int LAST, typename T>
inline typename std::enable_if<N1 + 1 < LAST and N2 < LAST, void>::type
order_helper(T&& t) {
    if (std::get<N1>(t) > std::get<N2>(t))
        std::swap(std::get<N1>(t), std::get<N2>(t));
    order_helper<N1, N2 + 1, LAST>(t);
}

    template<typename A, typename B> std::tuple<A, B> order(A&& a, B&& b) {
    return (a > b) ? std::make_tuple(b, a) : std::make_tuple(a, b);
}

/*! Order (Sort) \p a, \p b and \p rest... . */
template<typename A, typename B, typename... Rest>
std::tuple<A, B, Rest...> order(A&& a, B&& b, Rest&&... rest) {
    auto t = std::make_tuple(a, b, rest...);
    constexpr auto LAST = std::tuple_size<decltype(t)>::value - 1;
    order_helper<0, 1, LAST>(t);
    return t;
}
