/*! \file algorithm_x.hpp
 * \brief Extensions to STL <algorithm>.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * \todo http://herbsutter.com/2011/10/07/why-no-container-based-algorithms/
 * \todo Maybe rename all functions pnw:$(X) to std::$(X)_all.
 * \todo Merge with http://cbear.berlios.de/range/index.html
 *
 * \todo Specialize all use std::numeric_limits::min() and
 * std::numeric_limits::max() and using explicit \c UINT8_MIN, \c UINT8_MAX etc
 * and check whether compiler generates faster code.
 *
 * \todo Write ip_append(std::vector1, std::vector2) and ip_append(std::array, std::array) as
 *       vector1.insert( vector1.end(), vector2.begin(), vector2.end() );
 */

#pragma once
#include <cctype>
#include <functional>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <cstring>
#include <limits>
#include <ext/algorithm>
// #include <ext/functional>
// #include <ext/iterator>

// #include <boost/dynamic_bitset.hpp>
#include <tr2/dynamic_bitset>

#include "sortn.hpp"
#include "type_traits_x.hpp"
#include "cc_features.h"
#include "enforce.hpp"

namespace pnw
{

/*! @name Evaluate Code with Arguments.
 * \see http://functionalcpp.wordpress.com/2013/08/03/generalized-function-evaluation/
 */
/// \{
/*! Evaluate Functions (\c F), Functors, Lambdas, etc. */
template<class F, class... Args,
         class = typename std::enable_if<!std::is_member_function_pointer<F>::value>::type,
         class = typename std::enable_if<!std::is_member_object_pointer<F>::value>::type>
auto eval(F&& f, Args&&... args) -> decltype(f(std::forward<Args>(args)...)) { return f(std::forward<Args>(args)...); }

/*! Evaluate Const Member Function */
template<class R, class C, class P, class... Args>
auto eval(R(C::*f)() const, P&& p, Args&&... args) -> R { return (*p.*f)(std::forward<Args>(args)...); }

/*! Evaluate Non-Const Member Function */
template<class R, class C, class P, class... Args>
auto eval(R(C::*f)(), P&& p, Args&&... args) -> R { return (*p.*f)(std::forward<Args>(args)...); }

/*! Evaluate Member Object. */
template<class R, class C, class P>
auto eval(R(C::*m), P&& p) -> R { return *p.*m; }
/// \}

/*! @name Extremize.
 * These two functions are somewhat more easy to use in cases where you want
 * to assign a std::numeric_limits<T>::min() or max() to a value.
 */
/// \{
/*! Get \p T's \em minimum value. */
template<class T> inline pure T minof() { return std::numeric_limits<T>::min(); }
/*! Get \p T's \em maximum value. */
template<class T> inline pure T maxof() { return std::numeric_limits<T>::max(); }
/*! Set argument to its minimum value. */
template<class T> inline void minimize(T& a) { a = minof<T>(); }
/*! Set argument to its maximum value. */
template<class T> inline void maximize(T& a) { a = maxof<T>(); }
/// \}

/*! @name Check whether \p a is Minimum/Maximum of \p a. */
/// \{
template<class T> inline pure bool is_min(const T& a) { return a == pnw::minof<T>(); }
template<class T> inline pure bool is_max(const T& a) { return a == pnw::maxof<T>(); }
/// \}

/// @name Return Uninitialized Value of Type \c T.
/// @{
template<class T> inline T           uninitialized             () { return std::numeric_limits<T>::max(); }
template<>        inline float       uninitialized<float>      () { return nanf("NAN(string)"); }
template<>        inline double      uninitialized<double>     () { return nan ("NAN(string)"); }
template<>        inline long double uninitialized<long double>() { return nanl("NAN(string)"); }
// template<class T> inline typename std::enable_if<std::is_integral<T>::value, T>::type uninitialized() { return std::numeric_limits<T>::max(); }
/// @}

/// @name Defined?
/// @{
// template<class T> inline pure bool is_defined(T a) { return a; }
template<class T> inline pure typename std::enable_if<std::is_integral<T>::value, bool>::type is_defined(T a) { return a != 0; }
template<class T> inline pure typename std::enable_if<std::is_pointer<T>::value, bool>::type is_defined(T a) { return a != nullptr; }
template<class T> inline pure typename std::enable_if<std::is_floating_point<T>::value, bool>::type is_defined(T a) { return a != 0; }
// inline pure bool is_defined(const std::string&  a) { return !a.empty(); }
// inline pure bool is_defined(const std::string&& a) { return !a.empty(); }
template<class C> inline pure typename std::enable_if<is_container<C>::value, bool>::type is_defined(const C& a) { return !a.empty(); }
/// @}

}

namespace std
{

/*! @name Any.
 * \see http://stackoverflow.com/questions/3703658/specifying-one-type-for-all-arguments-passed-to-variadic-function-or-variadic-te
 * \note Uses value-semantic on function arguments to enable move-semantics.
 */
/// @{
template<class T>             inline pure T any(T a) { return a; } // \note Or use pnw::is_defined(a) ? a : T()
template<class T, class... U> inline pure typename std::common_type<T,U...>::type any(T a, U... b) { return pnw::is_defined(a) ? a : any(b...); }
/// @}

/*! @name Any.
 * \see http://stackoverflow.com/questions/3703658/specifying-one-type-for-all-arguments-passed-to-variadic-function-or-variadic-te
 * \note Uses value-semantic on function arguments to enable move-semantics.
 */
/// @{
template<class T>             inline pure T all(T a) { return a; } // \note Or use pnw::is_defined(a) ? a : T()
template<class T, class... U> inline pure typename std::common_type<T,U...>::type all(T a, U... b) { return pnw::is_defined(a) ? all(b...) : a; } // \note Or T() instead a.
/// @}

/// @name Overload \c min, \c max and \c abs to use C99 builtin variants.
/// \todo Extend with more functions.
/// @{
inline pure double      min(double f1, double f2)           { return fmin (f1, f2); }
inline pure float       min(float f1, float f2)             { return fminf(f1, f2); }
inline pure long double min(long double f1, long double f2) { return fminl(f1, f2); }
inline pure double      max(double f1, double f2)           { return fmax (f1, f2); }
inline pure float       max(float f1, float f2)             { return fmaxf(f1, f2); }
inline pure long double max(long double f1, long double f2) { return fmaxl(f1, f2); }
//inline pure long long int abs(long long int f) { return llabs(f); }
/// @}

/*! \name Simultaneous Minimum and Maximum Element of STL Containers. */
/// \{
/*! Get \em Iterator to Smallest Element in STL container (C) \p c. */
template<class C> inline pure typename std::enable_if<is_container<C>::value, C>::type::const_iterator min_element(const C& c) { return std::min_element(begin(c), end(c)); }
/*! Get \em Iterator to Largest Element in STL container (C) \p c. */
template<class C> inline pure typename std::enable_if<is_container<C>::value, C>::type::const_iterator max_element(const C& c) { return std::max_element(begin(c), end(c)); }
/*! Read \em Minimum \p min and \em Maximum \p max Element of STL Container \p a. */
template<class C> inline pure typename std::pair<typename C::const_iterator,
                                                 typename C::const_iterator> minmax_element(const C& c) {
    return std::minmax_element(begin(c), end(c));
}
/*! Read \em Minimum \p min and \em Maximum \p max Element Values of STL Container \p a. */
template<class C> inline pure std::pair<typename C::value_type,
                                        typename C::value_type> minmax_element_value(const C& c) {
    auto mm = std::minmax_element(begin(c), end(c));
    return std::make_pair(*mm.first, *mm.second);
}
/// @}

/// \em Map UnaryFunction @p func on all elements in the sequence \p a.
template<FAC C, class UnaryFunction> inline typename std::enable_if<is_container<C>::value, UnaryFunction>::type for_each(C& a, UnaryFunction func) { return std::for_each(begin(a), end(a), func); }

/// \em Fill \p a with \p value.
template<FAC C, class T>              inline C&   fill(C& a, const T& value) { std::fill(begin(a), end(a), value); return a; }
/// \em Fill \p a with \p count number of \p value.
template<FAC C, class Size, class T > inline void fill(C a, Size count, const T& value) { std::fill(begin(a), count, value); return a; }

/// \em Sort \p a.
template<RAC C> inline typename std::enable_if<is_container<C>::value, C>::type& sort(C& a) { std::sort(begin(a), end(a)); return a; }
/// \em Copying (Non-Mutable) Sort \p a.
template<RAC C> inline typename std::enable_if<is_container<C>::value, C>::type  sort_copy(C a) { std::sort(begin(a), end(a)); return a; }
/// \em Sort \p a.
template<RAC C, class Compare> inline typename std::enable_if<is_container<C>::value, C>::type sort(C& a, Compare b) { std::sort(begin(a), end(a), b); return a; }

/// \em Stable-Sort \p a.
template<RAC C>                           inline typename std::enable_if<is_container<C>::value, C>::type stable_sort(C a) { std::stable_sort(begin(a), end(a)); return a; }
/// \em Stable-Sort \p a.
template<RAC C, class StrictWeakOrdering> inline typename std::enable_if<is_container<C>::value, C>::type stable_sort(C a, StrictWeakOrdering b) { std::stable_sort(begin(a), end(a), b); return a; }

/// \em Merge \p a and \p b into \p c.
template<class IC, class OC>                           inline void merge(const IC& a, const IC& b, const OC& c) {
    std::merge(begin(a), end(a),
               begin(b), end(b),
               begin(c));
}
/// \em Merge \p a and \p b into \p c.
template<class IC, class OC, class StrictWeakOrdering> inline OC& merge(const IC& a, const IC& b, const OC& c, StrictWeakOrdering d) {
    std::merge(begin(a), end(a),
               begin(b), end(b),
               begin(c),
               d);
    return c;
}

/*! Copy Elements from \© IC \p a to \c OC \p b. */
template<class IC, class OC> inline typename std::enable_if<is_container<OC>::value, OC>::type& copy(const IC& a, OC& b) { return b = a; }

#if defined (_GLIBCXX_VECTOR) || defined (_STL_VECTOR_H)            ///< If <vector> was included
inline void copy_cstr(const char* a, std::vector<uint8_t>& b) {
    b.resize(std::strlen(a));
    for (size_t i = 0; i < b.size(); i++) { b[i] = a[i]; }
}
#endif

/// Check if \p a is \em sorted.
template<FAC C>                inline typename std::enable_if<is_container<C>::value, bool>::type is_sorted(const C& a) { return is_sorted(begin(a), end(a)); }
/// Check if \p a is \em sorted.
template<FAC C, class Compare> inline typename std::enable_if<is_container<C>::value, bool>::type is_sorted(const C& a, Compare comp) { return is_sorted(begin(a), end(a), comp); }

#ifdef HAVE_CXX11_LAMBDA_EXPRESSIONS
template<RAC C> inline C& abs_sort(C& a) {
    typedef typename C::value_type T;
    std::sort(begin(a), end(a), [](const T& x,
                                   const T& y) { return (std::abs(x) <
                                                         std::abs(y)); } );
    return a;
}
/// Sort \p a on the absolute values of the elements.
template<RAC C> inline C& abs_stable_sort(C& a) {
    typedef typename C::value_type T;
    std::stable_sort(begin(a), end(a), [](const T& x,
                                          const T& y) { return (std::abs(x) <
                                                                std::abs(y)) ; } );
    return a;
}
#endif

// Random Pick Element between InputIterators \p first and \p last.
template<class II> inline typename II::value_type random_pick(II first, II last) { typename II::value_type pick; __gnu_cxx::random_sample(first, last, (&pick)+0, (&pick)+1); return pick; }
// Random Pick Element in \p a.
template<FAC C>    inline typename std::enable_if<is_container<C>::value, C>::type::value_type  random_pick(C a)               { return random_pick(begin(a), end(a)); }

/*! Random Shuffle.
 * \see http://en.cppreference.com/w/cpp/algorithm/random_shuffle
 */
template<RAC C> typename std::enable_if<is_container<C>::value, C>::type& random_shuffle(C& a) { std::random_shuffle(begin(a), end(a)); }

// All Of
template<class IC, class UnaryPredicate> inline typename std::enable_if<is_container<IC>::value, bool>::type all_of(const IC& a, UnaryPredicate p) { return all_of(begin(a), end(a), p); }
template<class IC>                       inline typename std::enable_if<is_container<IC>::value, bool>::type all_of(const IC& a) { return all_of(begin(a), end(a), [](bool x) { return x; }); }
// Any Of
template<class IC, class UnaryPredicate> inline typename std::enable_if<is_container<IC>::value, bool>::type any_of(const IC& a, UnaryPredicate p) { return any_of(begin(a), end(a), p); }
template<class IC>                       inline typename std::enable_if<is_container<IC>::value, bool>::type any_of(const IC& a) { return any_of(begin(a), end(a), [](bool x) { return x; }); }
// None Of
template<class IC, class UnaryPredicate> inline typename std::enable_if<is_container<IC>::value, bool>::type none_of(const IC& a, UnaryPredicate p) { return none_of(begin(a), end(a), p); }
template<class IC>                       inline typename std::enable_if<is_container<IC>::value, bool>::type none_of(const IC& a) { return none_of(begin(a), end(a), [](bool x) { return x; }); }

/*! Reverse \p a.
 * \alias flip, mirror */
template<class BIDirContainer> inline BIDirContainer reverse(BIDirContainer a) { reverse(begin(a), end(a)); return std::move(a); }

/*! Uniquify \p a
 * \alias uniquify */
template<class C> inline C unique(C a)                                           { unique(begin(a), end(a)); return std::move(a); }
template<class C, class BinaryPredicate> inline C unique(C a, BinaryPredicate p) { unique(begin(a), end(a), p); return std::move(a); }

/*! Container Variants of std::find.
 * \see http://en.cppreference.com/w/cpp/algorithm/find */
template<class IC, class T>              typename IC::iterator find       (IC c, const T& value)   { return find(begin(c), end(c), value); }
template<class IC, class UnaryPredicate> typename IC::iterator find_if    (IC c, UnaryPredicate p) { return find_if(begin(c), end(c), p); }
template<class IC, class UnaryPredicate> typename IC::iterator find_if_not(IC c, UnaryPredicate q) { return find_if_not(begin(c), end(c), q); }

/*! Transformed Accumulate.
 * \see stackoverflow.com/questions/10592193/transform-and-accumulate
 * \alias zip_with
 * \tparam II InputIterator
 */
template<class II, class UnaryOp, class T>
T transformed_accumulate(II first, II last, UnaryOp transform_op, T init) {
    return std::accumulate(first, last, init,
                           [transform_op](const T& a, const T& b) {
                               return a + transform_op(b); });
}
/*! Transformed Accumulate.
 * \see stackoverflow.com/questions/10592193/transform-and-accumulate
 * \alias zip_with
 * \tparam II InputIterator
 */
template<class II, class UnaryOp, class BinaryOp, class T>
T transformed_accumulate(II first, II last, UnaryOp transform_op, T init, BinaryOp accumulate_op) {
    return std::accumulate(first, last, init,
                           [transform_op, accumulate_op](const T& a, const T& b) {
                               return accumulate_op(a, transform_op(b)); });
}

template<class T> void add(const T& a, const T& b) {  return a+b; }
template<class T> void sub(const T& a, const T& b) {  return a-b; }
template<class T> void mul(const T& a, const T& b) {  return a*b; }
template<class T> void div(const T& a, const T& b) {  return a/b; }

template<class C, class Gen> inline typename std::enable_if<is_container<C>::value, C>::type generate(C& c, Gen gen ) {
    generate(begin(c), end(c), gen); return c;
}
template<class C, class Gen> inline typename std::enable_if<is_container<C>::value, C>::type generate_n(C& c, size_t n, Gen gen ) {
    generate_n(begin(c), n, gen); return c;
}

}

// TODO: Replace pnw::minmin()
namespace pnw
{

/*! \name Oddity and Evenity.
 * \em Separate integer variants could be needed for some dumb
 * compilers to prevent from generating machine code that uses integer
 * divison.
 */
/// \{
/// Return true if odd.
template<class T> inline spure bool odd(const T& a) { return a % 2 == 1; }
/// Return true if even.
template<class T> inline spure bool even(const T& a) { return a % 2 == 0; }
/// \}

/*! Set \p a to true if it is false or vice versa. */
inline void toggle(bool & a) { a = not a; }

/*! \name Median. */
/// \{
/*! Median Value of \p a, \p b and \p c. */
template<class T> inline spure T median(T a, T b, T c) {
    return (a < b ?
            (b < c ? b : a < c ? c : a) :
            (b > c ? b : a > c ? c : a));
}
/*! Median Element Value between [\p first \p last[. */
template<class It> inline pure typename It::value_type median_element(It first, It last) {
    const auto size = last - first;
    auto nth = first + size/2;
    if (odd(size)) {
        std::nth_element(first, nth, last);
        return *nth;
    } else {
        std::nth_element(first, nth-1, last); auto a = *(nth-1);
        std::nth_element(first, nth+0, last); auto b = *(nth+0);
        return (a + b)/2;
    }
}
/*! Median Element Value of \p C. */
template<RAC C> inline pure typename C::value_type median_element(C c) { return median_element(begin(c), end(c)); } // \note Move Semantics enabled!
template<class It, class Compare>
/*! Median Element Value between [\p first \p last[ using Comparison Function \p comp. */
inline pure typename It::value_type median_element(It first, It last, Compare comp) {
    const auto size = last - first;
    auto nth = first + size/2;
    if (odd(size)) {
        std::nth_element(first, nth, last, comp);
        return *nth;
    } else {
        std::nth_element(first, nth-1, last, comp); auto a = *(nth-1);
        std::nth_element(first, nth+0, last, comp); auto b = *(nth+0);
        return (a + b)/2;
    }
}
/*! Median Element Value of \p C using Comparison Function \p comp. */
template<RAC C, class Compare> inline pure typename C::value_type median_element(C c, Compare comp) { return median_element(begin(c), end(c), comp); } // \note Move Semantics enabled!
/// \}

/*! \name Minimum.
 * Return the minimum value of the arguments.
 */
/// \{
#if BOOST_NO_VARIADIC_TEMPLATES
/*! Minimum of \p a and \p b. */
template<LTC T> inline pure T min(const T& a, const T& b) { return std::min(a, b); }
/*! Minimum of \p a, \p b and \p c. */
template<LTC T> inline pure T min(const T& a, const T& b, const T& c) { return std::min(a, std::min(b, c)); }
/*! Minimum of \p a, \p b, \p c and \p d. */
template<LTC T> inline pure T min(const T& a, const T& b, const T& c, const T& d) { return std::min(a, b, std::min(c, d)); }
#else

/*! \em Multi-Type Minimum of \p a. */
template<LTC S, LTC T>
inline pure T min(const S& a, const T& b) { return std::min(a, b); }

/*! \em Multi-Type Minimum of \p a, \p b and \p c. */
template<LTC S, LTC T, LTC ... R >
inline pure T min(const S& a, const T& b, const R &... c) { return std::min(a, min(b, c...)); }

/*! \em Common-Type Minimum of \p a and \p b. */
template<LTC S, LTC T, LTC C = typename std::common_type<S,T>::type>
inline pure C common_type_min(const S& a, const T& b) { return std::min(static_cast<C>(a), static_cast<C>(b)); }

/*! \em Common-Type Minimum of \p a and \p args. */
template<LTC S, LTC T, LTC ... R, LTC C = typename std::common_type<S,T, R...>::type >
inline pure C common_type_min(const S& a, const T& b, const R &... c) { return std::min(static_cast<C>(a),
                                                                                        static_cast<C>(common_type_min(b, c...))); }
#endif
/// \}

/*! \name Maximum.
 * Return the maximum value of the arguments.
 */
/// \{
#if BOOST_NO_VARIADIC_TEMPLATES
/*! Maximum of \p a and \p b. */
template<LTC T> inline pure T max(const T& a, const T& b) { return std::max(a, b); }
/*! Maximum of \p a, \p b and \p c. */
template<LTC T> inline pure T max(const T& a, const T& b, const T& c) { return std::max(a, std::max(b, c)); }
/*! Maximum of \p a, \p b, \p c and \p d. */
template<LTC T> inline pure T max(const T& a, const T& b, const T& c, const T& d) { return std::max(a, b, std::max(c, d)); }
#else
/*! Multi-Type Maximum of \p a. */
template<LTC S, LTC T>
inline pure T max (const S& a, const T& b) { return std::max(a, b); }
/*! Multi-Type Maximum of \p a, \p b and \p c. */
template<LTC S, LTC T, LTC ... R >
inline pure T max(const S& a, const T& b, const R &... c) { return std::max(a, max(b, c...)); }
/*! Maximum of \p a and \p b. */
template<LTC S, LTC T, LTC C = typename std::common_type<S,T>::type>
inline pure C common_type_max(const S& a, const T& b) { return std::max(static_cast<C>(a), static_cast<C>(b)); }
/*! Maximum of \p a and \p args. */
template<LTC S, LTC T, LTC ... R, LTC C = typename std::common_type<S,T, R...>::type >
inline pure C common_type_max(const S& a, const T& b, const R &... c) { return std::max(static_cast<C>(a),
                                                                                                         static_cast<C>(common_type_max(b, c...))); }
#endif
/// \}

/*! @name Homogeneous Pair Minimum/Maximum. */
/// \{
template<class T> T min_element(const std::pair<T,T>& a) { return std::min<T>(a.first, a.second); }
template<class T> T max_element(const std::pair<T,T>& a) { return std::max<T>(a.first, a.second); }
/// \}

/*! @name Heterogeneous Pair Minimum/Maximum. */
/// \{
template<class T, class U> typename std::common_type<T,U>::type min_element(const std::pair<T,U>& a) { return common_type_min<T,U>(a.first, a.second); }
template<class T, class U> typename std::common_type<T,U>::type max_element(const std::pair<T,U>& a) { return common_type_max<T,U>(a.first, a.second); }
/// \}

/*! \em Min of \p a, \p b and \p c and put it back in the @em
 * first arg. The other arguments are reordered in an undefined
 * manner.
 */
template<LTC T> inline void min_sort3(T& a, T& b, T& c) {
    sort2(a, b);
    sort2(a, c);
}
/*! \em Max of \p a, \p b and \p c and put it back in the @em
 * last arg. The other arguments are reordered in an undefined
 * manner.
 */
template<LTC T> inline void max_sort3(T& a, T& b, T& c) {
    sort2(b, c);
    sort2(a, c);
}
/*! Inline-Sort \p a, \p b, \p c. */
template<LTC T> inline void sort3(T& a, T& b, T& c) {
    sort2(a,b);
    sort2(b,c);
}

/*! \name In-Place Minimum and Maximum.
 * Calculate min and max of all args and put min and max back into the
 * first and last arg respectively, while the other arguments are
 * reordered in an undefined manner.
 */
/// \{
template<LTC T> inline void minmax3(T& a, T& b, T& c) {
    max_sort3(a,b,c);
    sort2(a,b);
}
template<LTC T> inline void minmax4(T& a, T& b, T& c, T& d) {
    sort2(a, b);
    sort2(c, d);
    sort2(a, c);
    sort2(b, d);
}
template<LTC T> inline void minmax5(T& a, T& b, T& c, T& d, T& e) {
    sort2(a, b);
    sort2(c, d);
    min_sort3(a, c, e);
    max_sort3(b, d, e);
}
template<LTC T> inline void minmax6(T& a, T& b, T& c, T& d, T& e, T& f) {
    sort2(a, d);
    sort2(b, e);
    sort2(c, f);
    min_sort3(a, b, c);
    max_sort3(d, e, f);
}
/// \}

// TODO: Fix or remove variadic stuff.
// template<LTC T ,
//           BinaryPredicate <T, T> Compare ,
//           LTC ... Args >
// const T& min( const T& a , const T& b , const Args &... args , Compare comp );
// template<LTC T> T maximum(T n) { return n; }
// template<LTC T, typename... Args> int maximum(T n, Args... args) { return max(n, maximum(args...)); }

/*! Argument \p x modulo \p m. */
template<class T> inline T modulo(T x, const T& m) { x %= m; if (x < 0) { x += m; } return x; }

/*! Television-Area using the ratios \p r0 and \p r1 and the diameter \p d.
 * Units in inches and square-inches.
 * - 28-inch (3:4):  televisionArea (28, 3, 4)
 * - 32-inch (9:16): televisionArea (32, 9, 16)
 */
template<class T> inline T tv_area(T d, T r0, T r1) { return square(d) * (r0 * r1) / (square(r0) + square(r1)); }

/*! First primes from 0 to 39.
 * See: "Matematiklexikon", Page 26
 */
template<class T> inline T consecutive_primes_0_39(T a) { a += 40; return a * a - 79 * a + 1601; }

/*! Delete all elements of \p a and clear it. */
template<class T> inline void delete_all_and_clear(T& a)
{
    for (auto& e : a) { delete e; } // for (auto i = begin(a); i != end(a); i++) { delete *i; }
    a.clear();
}
/*! Delete and Zero Pointer. */
template<class T> inline void destroy(T*& a) { assert(a); delete a; a = nullptr; }

// Swap \p a with \p b inline
// \todo REMove when bug in gcc is removed.
#if 0
inline void swap(float& a, float& b)
{
    *(reinterpret_cast<uint32_t*> (&a)) ^= *(reinterpret_cast<uint32_t*> (&b));
    *(reinterpret_cast<uint32_t*> (&b)) ^= *(reinterpret_cast<uint32_t*> (&a));
    *(reinterpret_cast<uint32_t*> (&a)) ^= *(reinterpret_cast<uint32_t*> (&b));
}
inline void swap(double& a, double& b)
{
    *(reinterpret_cast<uint64_t*> (&a)) ^= *(reinterpret_cast<uint64_t*> (&b));
    *(reinterpret_cast<uint64_t*> (&b)) ^= *(reinterpret_cast<uint64_t*> (&a));
    *(reinterpret_cast<uint64_t*> (&a)) ^= *(reinterpret_cast<uint64_t*> (&b));
}
#endif


/*! Iterative multiply */
template<class Ta, class Tb> inline void iter_mul(Ta & y, Ta x, Tb a) {
    y = 0;
    while (a > 0)
    {
        if (pnw::even(a)) { x += x; a /= 2; }
        else              { y += x; a -= 1; }
    }
}
/*! Default multiply */
template<class Ta, class Tb> inline void multiply(Ta & y, Ta x, Tb a) { iter_mul(y, x, a); }

/*! Check whether \p a lacks element \p b or not. */
template<class C> inline bool lacks(const C& a, const typename C::value_type& b) {
    return std::find(begin(a), end(a), b) == end(a);
}
/*! Check whether \p a contains element \p b or not. */
template<class C> inline bool has(const C& a, const typename C::value_type& b) { return !has(a, b); }

/*! Reorder/Permutate in \p a trough permutation indexes \p i into \p b. */
template<class V, class I> void indexed_reorder(const V& a, V& b, const I& i)
{
    enforce_eq(a.size(), b.size());
    enforce_eq(b.size(), i.size());
    for (auto e : i) {
        b = a[e];
    }
}

/*! Return true if \p a is a [0, ..., n-1] permutation.
 * \complexity[time] O(n)
 */
template<class T> inline pure bool is_continuous_permutation(const T *a, size_t n) {
#if defined (_GLIBCXX_TR2_DYNAMIC_BITSET)
    std::tr2::dynamic_bitset<size_t> h(n, 0); // tags
#elif defined (BOOST_DYNAMIC_BITSET_HPP)
    boost::dynamic_bitset<size_t> h(n, 0); // tags
#endif
    for (size_t i = 0; i < n; i++) {
        if (not (a[i] >= 0 and
                 a[i] < n)) { // if not inside bounds
            return false;
        }
        if (not h[a[i]].flip()) { // if already flipped
            return false;         // its not a permutation
        }
    }
    return true;
}

template<class C> inline pure bool is_continuous_permutation(const C& a) {
    return is_continuous_permutation(a.data(), a.size());
}

/*! Reorder/Permutate in \p a trough permutation indexes \p i. */
template<class V, class P> void in_place_indexed_reorder(V& a, const P& i)
{
    enforce_eq(a.size(), i.size());

    // TODO: Fix this later on
#if 0
    typedef typename V::value_type E;    // element type
    typedef typename P::value_type I;    // permutation index type

    const I i0 = *begin(i);     // index to first element of permutate
    E       e0 = a[i0];         // value of first/current element of permutation

    size_t c = 0;
    size_t j = 0;               // permutation index
    auto n = a.size();          // element count
    while (c != n) {            // while more elements left. TODO: Change to j == n-1
        const int rN = i[j]; // next index
        std::swap(a[e], e0);
        c++;
    }
    last;
#endif
}

}
