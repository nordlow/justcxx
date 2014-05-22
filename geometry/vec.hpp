/*! \file: vec.hpp
 * \brief N-Dimensional Vector with MATLAB style namings of functions and algorithms.
 * \author: Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date: 2012-05-13 22:55
 *
 * \see http://stackoverflow.com/questions/10592193/transform-and-accumulate
 * \see http://stackoverflow.com/questions/5040805/boost-units-using-a-dimensionless-type-of-arbitrary-system
 */

#pragma once
#include <algorithm>
#include <numeric>
#include <array>
#include <cmath>
#include <cfloat>
#include <boost/serialization/access.hpp>
#include "../cc_features.h"
#include "../algorithm_x.hpp"
#include "../math_x.hpp"
#include "../enforce.hpp"
#include "../mean.hpp"
#include "../bitwise.hpp"
#include "../angles.hpp"

#define MEMCH ("≡")              ///< Member Character

/*! \p N-Dimensional Vector. */
template<class T, std::size_t N>
class vec : public std::array<T, N> {
public:
    /// \name Construct.
    /// \{
#if NDEBUG
    vec() {}                    ///< Default Construct Uninitialized. */
#else
    /*! Default Construct to Undefined. */
    vec() { this->fill(pnw::uninitialized<T>()); }
#endif

#if defined (OGLPLUS_VECTOR_1107121519_HPP)
    /*! Constructors for integration with oglplus. */
    vec(const oglplus::Vector<T,N>& a) { std::copy(Data(a), Data(a)+Size(a), begin(*this)); }
#endif

   /*! Constructors for integration with Armadillo Linear Algebra Library. */
#if defined (ARMA_INCLUDES)
    // template<class AT> vec(const typename arma::Col<AT>::fixed& a) { std::copy(begin(a), end(a), begin(*this)); }
    // template<class AT> vec(const typename arma::Row<AT>::fixed& a) { std::copy(begin(a), end(a), begin(*this)); }

    // vec(const arma::uvec::fixed<N>& a) { std::copy(begin(a), end(a), begin(*this)); }
    // vec(const arma::ivec::fixed<N>& a) { std::copy(begin(a), end(a), begin(*this)); }
    // vec(const arma::fvec::fixed<N>& a) { std::copy(begin(a), end(a), begin(*this)); }
    // vec(const arma:: vec::fixed<N>& a) { std::copy(begin(a), end(a), begin(*this)); }

    // vec(const arma::ucolvec::fixed<N>& a) { std::copy(begin(a), end(a), begin(*this)); }
    // vec(const arma::icolvec::fixed<N>& a) { std::copy(begin(a), end(a), begin(*this)); }
    // vec(const arma::fcolvec::fixed<N>& a) { std::copy(begin(a), end(a), begin(*this)); }
    // vec(const arma:: colvec::fixed<N>& a) { std::copy(begin(a), end(a), begin(*this)); }

    // vec(const arma::urowvec::fixed<N>& a) { std::copy(begin(a), end(a), begin(*this)); }
    // vec(const arma::irowvec::fixed<N>& a) { std::copy(begin(a), end(a), begin(*this)); }
    // vec(const arma::frowvec::fixed<N>& a) { std::copy(begin(a), end(a), begin(*this)); }
    // vec(const arma:: rowvec::fixed<N>& a) { std::copy(begin(a), end(a), begin(*this)); }

    template<class AT> vec(const arma::Col<AT>& a) { enforce_eq(a.size(), N); std::copy(a.memptr(), a.memptr()+a.size(), begin(*this)); }
    template<class AT> vec(const arma::Row<AT>& a) { enforce_eq(a.size(), N); std::copy(a.memptr(), a.memptr()+a.size(), begin(*this)); }
    template<class AT> vec(const arma::Mat<AT>& a) {
        const bool dim_match = ((a.n_rows() == N and a.n_cols() == 1) or
                                (a.n_cols() == N and a.n_rows() == 1));
        enforce(dim_match);
        std::copy(a.memptr(), end(a), begin(*this));
    }
#endif

    vec(T a) { this->fill(a); } ///< Construct from scalar \p a, defaulting to \em origo. */
#if HAVE_CXX11_VARIADIC_TEMPLATES
    /*! Construct from \p N arguments.
     * Gives compile error using when not N arguments are given (using \c BOOST_STATIC_ASSERT).
     */
    template<class... R> vec(T a, R... r) {
        static_assert(1+(sizeof...(r)) == N, "vec needs exactly N arguments in construction."); // assure N arguments to construct
        (*this)[0] = a; assign(1, r...); // recurse
    }
    /// \}

    template<class... R> void assign(size_t i, T a, R... r) { (*this)[i] = a; assign(i+1, r...); } // recursion case
    void assign(size_t i, T a) { (*this)[i] = a; } // termination case
    // template<class... R> vec(T a, R... r) {
    // // assure N arguments to construct
    // static_assert(1 + (sizeof...(r)) == N); //enforce_eq(1 + (sizeof...(r)), N);
    // (*this)[0] = a;
    // const T ra[sizeof...(r)] = { r... }; // convert all arguments in \p r... bto \c T
    // for (size_t i = 1; i != N; i++) {
    // (*this)[i] = ra[i-1];
    // }
    // }
#else
    /*! Construct from [\p a, \p b]. */
    vec(T a, T b)           { if (N >= 1) (*this)[0] = a; if (N >= 2) (*this)[1] = b; }
    /*! Construct from [\p a, \p b, \p c]. */
    vec(T a, T b, T c)      { if (N >= 1) (*this)[0] = a; if (N >= 2) (*this)[1] = b; if (N >= 3) (*this)[2] = c; }
    /*! Construct from [\p a, \p b, \p c, \p d]. */
    vec(T a, T b, T c, T d) { if (N >= 1) (*this)[0] = a; if (N >= 2) (*this)[1] = b; if (N >= 3) (*this)[2] = c; if (N >= 4) (*this)[3] = d; }
#endif

#if HAVE_CXX11_INITIALIZER_LISTS
    // \see http://stackoverflow.com/questions/5438671/static-assert-on-initializer-listsize
    vec             (const std::initializer_list<T>& list) { enforce_eq(N, list.size()); std::copy(begin(list), end(list), begin(*this)); }
    vec& operator = (const std::initializer_list<T>& list) { enforce_eq(N, list.size()); std::copy(begin(list), end(list), begin(*this)); }
#endif
    size_t dimnum() const { return N; }

    /// \name Element Access.
    /// \{
    T  operator () (size_t i) const { return (*this)[i]; }
    T& operator () (size_t i)       { return (*this)[i]; }
    T  x() const { return (*this)[0]; }
    T& x()       { return (*this)[0]; }
    T  y() const { return (*this)[1]; }
    T& y()       { return (*this)[1]; }
    T  z() const { return (*this)[2]; }
    T& z()       { return (*this)[2]; }
    T  w() const { return (*this)[3]; }
    T& w()       { return (*this)[3]; }
    /// \}

    /// \name Pointwise Binary Comparison.
    /// \{
    friend bool operator == (const vec& a, const vec& b) { return std::all_of( eq(a, b)); }
    friend bool operator != (const vec& a, const vec& b) { return std::any_of(neq(a, b)); }
    friend bool operator == (const vec& a, T   b) { return std::all_of( eq(a, b)); }
    friend bool operator != (const vec& a, T   b) { return std::all_of(neq(a, b)); }
    /// \}

public:
    /// \name Pointwise Binary Arithmetic and Assignment
    /// \{
#if HAVE_CXX11_RANGE_BASED_FOR_LOOP
    vec& operator  = (T a) { for (auto& e : *this) { e  = a; }; return *this; }
    vec& operator += (T a) { for (auto& e : *this) { e += a; }; return *this; }
    vec& operator -= (T a) { for (auto& e : *this) { e -= a; }; return *this; }
    vec& operator *= (T a) { for (auto& e : *this) { e *= a; }; return *this; }
    vec& operator /= (T a) { for (auto& e : *this) { e /= a; }; return *this; }
    vec& operator %= (T a) { for (auto& e : *this) { e %= a; }; return *this; }
#else
    vec& operator  = (T a) { std::for_each(begin(*this), end(*this), [a](T & e) { e  = a; }); return *this; }
    vec& operator += (T a) { std::for_each(begin(*this), end(*this), [a](T & e) { e += a; }); return *this; }
    vec& operator -= (T a) { std::for_each(begin(*this), end(*this), [a](T & e) { e -= a; }); return *this; }
    vec& operator *= (T a) { std::for_each(begin(*this), end(*this), [a](T & e) { e *= a; }); return *this; }
    vec& operator /= (T a) { std::for_each(begin(*this), end(*this), [a](T & e) { e /= a; }); return *this; }
    vec& operator %= (T a) { std::for_each(begin(*this), end(*this), [a](T & e) { e %= a; }); return *this; }
#endif
    vec& operator += (const vec& a) { aop(a, [](T x, T y) { return x + y; }); return *this; }
    vec& operator -= (const vec& a) { aop(a, [](T x, T y) { return x - y; }); return *this; }
    vec& operator *= (const vec& a) { aop(a, [](T x, T y) { return x * y; }); return *this; }
    vec& operator /= (const vec& a) { aop(a, [](T x, T y) { return x / y; }); return *this; }
    vec& operator %= (const vec& a) { aop(a, [](T x, T y) { return x % y; }); return *this; }
    /// \}

    /*! \name Pointwise Unary Arithmetic.
     * \alt friend vec sqr (const vec& a) { vec b; std::transform(begin(a), end(a), begin(b), std::sqr<T>); return b; }
     */
    /// \{
    friend vec operator - (const vec& a) { vec b; std::transform(begin(a), end(a), begin(b), [](T x) { return -x; }); return b; }
    friend vec sqrt (const vec& a) { vec b; std::transform(begin(a), end(a), begin(b), (T(*)(T))std::sqrt); return b; }
    friend vec cbrt (const vec& a) { vec b; std::transform(begin(a), end(a), begin(b), (T(*)(T))std::cbrt); return b; }
    friend vec abs  (const vec& a) { vec b; std::transform(begin(a), end(a), begin(b), (T(*)(T))std::abs); return b; }
    friend vec floor(const vec& a) { vec b; std::transform(begin(a), end(a), begin(b), (T(*)(T))std::floor); return b; }
    friend vec ceil (const vec& a) { vec b; std::transform(begin(a), end(a), begin(b), (T(*)(T))std::ceil); return b; }
    friend vec rint (const vec& a) { vec b; std::transform(begin(a), end(a), begin(b), (T(*)(T))rint); return b; }
    friend vec pow  (const vec& a, T y) { vec b; std::transform(begin(a), end(a), begin(b), [y](T x) { return pow(x, y); }); return b; }
    /// \}

    /*! \name Pointwise Binary Arithmetic.
     * \alt friend vec operator + (const vec& a, const vec& b) { vec c; std::transform(begin(a), end(a), begin(b), begin(c), std::add<T>()); return c; }
     * \alt friend vec operator + (const vec& a, const vec& b) { vec c; std::transform(begin(a), end(a), begin(b), begin(c), [](T x, T y) { return x + y; }); return c; }
     * \alt friend vec operator + (const vec& a, T b) { vec c; std::transform(begin(a), end(a), begin(c), [b](T x) { return x + b; }); return c; }
     */
    /// \{
#if HAVE_CXX1Y_RETURN_TYPE_DEDUCTION
    template<class U> friend auto operator + (const vec<T,N>& a, const vec<U,N>& b) { return bao(a, b, [](T x, U y) { return x + y; }); } // std::plus
    template<class U> friend auto operator - (const vec<T,N>& a, const vec<U,N>& b) { return bao(a, b, [](T x, U y) { return x - y; }); } // std::minus
    template<class U> friend auto operator * (const vec<T,N>& a, const vec<U,N>& b) { return bao(a, b, [](T x, U y) { return x * y; }); } // std::times
    template<class U> friend auto operator / (const vec<T,N>& a, const vec<U,N>& b) { return bao(a, b, [](T x, U y) { return x / y; }); } // std::divide
    template<class U> friend auto operator % (const vec<T,N>& a, const vec<U,N>& b) { return bao(a, b, [](T x, U y) { return x % y; }); } // std::percent
    template<class U> friend auto operator + (const vec& a, U b) { return uao(a, [b](U x) { return x + b; }); }
    template<class U> friend auto operator - (const vec& a, U b) { return uao(a, [b](U x) { return x - b; }); }
    template<class U> friend auto operator * (const vec& a, U b) { return uao(a, [b](U x) { return x * b; }); }
    template<class U> friend auto operator / (const vec& a, U b) { return uao(a, [b](U x) { return x / b; }); }
    template<class U> friend auto operator % (const vec& a, U b) { return uao(a, [b](U x) { return x % b; }); }
#else
    template<class U> friend auto operator + (const vec<T,N>& a, const vec<U,N>& b) -> vec<decltype(a[0] + b[0]),N> { return bao(a, b, [](T x, U y) { return x + y; }); } // std::plus
    template<class U> friend auto operator - (const vec<T,N>& a, const vec<U,N>& b) -> vec<decltype(a[0] - b[0]),N> { return bao(a, b, [](T x, U y) { return x - y; }); } // std::minus
    template<class U> friend auto operator * (const vec<T,N>& a, const vec<U,N>& b) -> vec<decltype(a[0] * b[0]),N> { return bao(a, b, [](T x, U y) { return x * y; }); } // std::times
    template<class U> friend auto operator / (const vec<T,N>& a, const vec<U,N>& b) -> vec<decltype(a[0] / b[0]),N> { return bao(a, b, [](T x, U y) { return x / y; }); } // std::divide
    template<class U> friend auto operator % (const vec<T,N>& a, const vec<U,N>& b) -> vec<decltype(a[0] / b[0]),N> { return bao(a, b, [](T x, U y) { return x % y; }); } // std::percent
    template<class U> friend auto operator + (const vec& a, U b) -> vec<decltype(a[0] + b[0]),N> { return uao(a, [b](U x) { return x + b; }); }
    template<class U> friend auto operator - (const vec& a, U b) -> vec<decltype(a[0] - b[0]),N> { return uao(a, [b](U x) { return x - b; }); }
    template<class U> friend auto operator * (const vec& a, U b) -> vec<decltype(a[0] * b[0]),N> { return uao(a, [b](U x) { return x * b; }); }
    template<class U> friend auto operator / (const vec& a, U b) -> vec<decltype(a[0] / b[0]),N> { return uao(a, [b](U x) { return x / b; }); }
    template<class U> friend auto operator % (const vec& a, U b) -> vec<decltype(a[0] % b[0]),N> { return uao(a, [b](U x) { return x % b; }); }
#endif
    friend vec         min        (const vec& a, const vec& b) { return bao(a, b, [](T x, T y) { return std::min(x, y); }); }
    friend vec         max        (const vec& a, const vec& b) { return bao(a, b, [](T x, T y) { return std::max(x, y); }); }
    friend vec         mean       (const vec& a, const vec& b) { return bao(a, b, [](T x, T y) { return pnw::mean(x, y); }); }
    friend vec<bool,N> eq         (const vec& a, const vec& b) { return bpo(a, b, [](T x, T y) { return x == y; }); }
    friend vec<bool,N> neq        (const vec& a, const vec& b) { return bpo(a, b, [](T x, T y) { return x != y; }); }
    friend vec<bool,N> operator < (const vec& a, const vec& b) { return bpo(a, b, [](T x, T y) { return x < y; }); }
    friend vec<bool,N> operator > (const vec& a, const vec& b) { return bpo(a, b, [](T x, T y) { return x > y; }); }
    friend vec<bool,N> operator <=(const vec& a, const vec& b) { return bpo(a, b, [](T x, T y) { return x <= y; }); }
    friend vec<bool,N> operator >=(const vec& a, const vec& b) { return bpo(a, b, [](T x, T y) { return x >= y; }); }

    friend vec         min        (const vec& a, T b) { return uao(a, [b](T x) { return std::min(x, b); }); }
    friend vec         max        (const vec& a, T b) { return uao(a, [b](T x) { return std::max(x, b); }); }
    friend vec         mean       (const vec& a, T b) { return uao(a, [b](T x) { return pnw::mean(x, b); }); }
    friend vec<bool,N> eq         (const vec& a, T b) { return upo(a, [b](T x) { return x == b; }); }
    friend vec<bool,N> neq        (const vec& a, T b) { return upo(a, [b](T x) { return x != b; }); }
    friend vec<bool,N> operator < (const vec& a, T b) { return bpo(a, [b](T x) { return x < b; }); }
    friend vec<bool,N> operator > (const vec& a, T b) { return bpo(a, [b](T x) { return x > b; }); }
    friend vec<bool,N> operator <=(const vec& a, T b) { return bpo(a, [b](T x) { return x <= b; }); }
    friend vec<bool,N> operator >=(const vec& a, T b) { return bpo(a, [b](T x) { return x >= b; }); }

    friend vec operator +         (T a, const vec& b) { return uao(b, [a](T x) { return a + x; }); }
    friend vec operator -         (T a, const vec& b) { return uao(b, [a](T x) { return a - x; }); }
    friend vec operator *         (T a, const vec& b) { return uao(b, [a](T x) { return a * x; }); }
    friend vec operator /         (T a, const vec& b) { return uao(b, [a](T x) { return a / x; }); }
    friend vec operator %         (T a, const vec& b) { return uao(b, [a](T x) { return a % x; }); }

    friend vec operator +         (const vec& a, T b) { return uao(a, [b](T x) { return x + b; }); }
    friend vec operator -         (const vec& a, T b) { return uao(a, [b](T x) { return x - b; }); }
    friend vec operator *         (const vec& a, T b) { return uao(a, [b](T x) { return x * b; }); }
    friend vec operator /         (const vec& a, T b) { return uao(a, [b](T x) { return x / b; }); }
    friend vec operator %         (const vec& a, T b) { return uao(a, [b](T x) { return x % b; }); }

    friend vec         min        (T a, const vec& b) { return uao(b, [a](T x) { return std::min(a, x); }); }
    friend vec         max        (T a, const vec& b) { return uao(b, [a](T x) { return std::max(a, x); }); }
    friend vec         mean       (T a, const vec& b) { return uao(b, [a](T x) { return pnw::mean(a, x); }); }
    friend vec<bool,N> eq         (T a, const vec& b) { return upo(b, [a](T x) { return a == x; }); }
    friend vec<bool,N> neq        (T a, const vec& b) { return upo(b, [a](T x) { return a != x; }); }
    friend vec<bool,N> operator < (T a, const vec& b) { return bpo(b, [a](T x) { return a < x; }); }
    friend vec<bool,N> operator > (T a, const vec& b) { return bpo(b, [a](T x) { return a > x; }); }
    friend vec<bool,N> operator <=(T a, const vec& b) { return bpo(b, [a](T x) { return a <= x; }); }
    friend vec<bool,N> operator >=(T a, const vec& b) { return bpo(b, [a](T x) { return a >= x; }); }
    /// \}

    /*! Return true if \em all elements in \p a are non-zero or true. */
    friend bool all(const vec& a) {
#if HAVE_CXX11_RANGE_BASED_FOR_LOOP
        for (const auto& e : a) { if (not e) return false; } return true;
#endif
    }
    /*! Return true if \em any element in \p a are non-zero or true. */
    friend bool any(const vec& a) {
#if HAVE_CXX11_RANGE_BASED_FOR_LOOP
        for (const auto& e : a) { if (e) return true; } return false;
#endif
    }
    explicit operator bool() const { return all(*this); }

    /// Dot-Product of \p a and \p b.
    // friend T dot(const vec& a, const vec& b) { return std::inner_product(begin(a), end(a), begin(b), 0); }
    /// Normalized Dot-Product of \p a and \p b.
    // friend T norm_dot(const vec& a, const vec& b) { return dot(a, b) / (norm(a) * norm(b)) ; }
    /*! Angle \theta between \p a and \p b.
     * \see https://en.wikipedia.org/wiki/Dot_product#Geometric_interpretation */
    // friend T angle(const vec& a, const vec& b) { return std::acos(norm_dot(a, b)); }

    /// \name Norms.
    /// \{
    /// l1-Norm of \p a.
    /// Square Norm of \p a, dot(a,a).
    friend pure T sqrnorm(const vec& a) { return std::transformed_accumulate(begin(a), end(a), std::sqr<T>, 0); }
    //friend pure T l1norm(const vec& a) { vec b = std::abs(a); return std::accumulate(begin(b), end(b), 0); }
    friend pure T l1norm(const vec& a) { return std::transformed_accumulate(begin(a), end(a), (T(*)(T))std::abs, 0); }
    /// l2-Norm of \p a.
    friend pure T l2norm(const vec& a) { return std::sqrt(sqrnorm(a)); }
    /// Norm of \p a.
    friend pure T norm(const vec& a) { return l2norm(a); }
    /// \p n-Norm of \p a.
    friend pure T nnorm(const vec& a, int n) {
        if      (n == 1) { return l1norm(a);  }
        else if (n == 2) { return l2norm(a); }
        else             { // PTODO("cannot handle norm %d\n", n);
            return 0; }
    }
    /// \}

    /// \name Distances.
    /// \{
    friend inline pure T l1dist(const vec& a, const vec& b) { return l1norm(a-b); }
    friend inline pure T l2dist(const vec& a, const vec& b) { return l2norm(a-b); }
    friend inline pure T dist(const vec& a, const vec& b) { return l2dist(a, b); }
    friend inline pure T ndist(const vec& a, const vec& b, int n) {
        if      (n == 1) { return l1dist(a, b);  }
        else if (n == 2) { return l2dist(a, b); }
        else             { // PTODO("cannot handle dist %d\n", n);
            return 0; }
    }
    /// \}

    /*! Dimension Distance Maximum between \p a and \p b. */
    inline pure T dimdistmax(const vec& a, const vec& b) { return max_element(abs(a-b)); }

    /// Normalize \c this.
    void normalize() { *this /= norm(*this); }

    /*! \name Minimum and Maximum Element.
     * Named in accordance with C++11 Algorithms.
     * \see http://en.cppreference.com/w/cpp/algorithm/min_element */
    /// \{
    friend pure T min_element(const vec& a) { return *std::min_element(begin(a), end(a)); }
    friend pure T max_element(const vec& a) { return *std::max_element(begin(a), end(a)); }
    friend pure T mean_element(const vec& a) { return *pnw::mean_element(begin(a), end(a)); }
    /// Index to Minimum Dimension.
    friend pure size_t min_element_index(const vec& a) { return std::min_element(begin(a), end(a)) - begin(a); }
    /// Index to Maximum Dimension.
    friend pure size_t max_element_index(const vec& a) { return std::max_element(begin(a), end(a)) - begin(a); }
    /// \}

protected:
    /*! \name Pointwise Arithmetic Helper Functions. */
    /// \{
    inline      void aop(const vec& a,               std::function<   T(T,T)> op) {        std::transform(begin(*this),end(*this),   begin(a), begin(*this), op); } ///< Unary Side-Effect Operation
    friend pure vec  uao(const vec& a,               std::function<   T(T)>   op) { vec c; std::transform(begin(a),end(a),           begin(c), op); return c; } ///< \em Unary Arithmetic Operation
    friend pure vec  bao(const vec& a, const vec& b, std::function<   T(T,T)> op) { vec c; std::transform(begin(a),end(a), begin(b), begin(c), op); return c; } ///< \em Binary Arithmetic Operation
    /// \}
    /*! \name Pointwise Predicate Helper Functions. */
    /// \{
    friend pure vec<bool,N> upo(const vec& a,        std::function<bool(T)>   op) { vec<bool,N> c; std::transform(begin(a),end(a),           begin(c), op); return c; } ///< \em Unary Predicate Operation
    friend pure vec<bool,N> bpo(const vec& a, const vec& b, std::function<bool(T,T)> op) { vec<bool,N> c; std::transform(begin(a),end(a), begin(b), begin(c), op); return c; } ///< \em Binary \em Predicate Operation
    /// \}

    template<class S> friend pure S concept_name(const vec& m) { S s("0-Dimensional Vector"); s[0] += N; return s; }
    template<class S> friend pure S typenameof(const vec& a) { S s("vec<T,N>"); return s; }

    friend class boost::serialization::access;
    template<class Ar> void serialize(Ar& ar, const uint version) {
        // ar & *(reinterpret_cast<std::array<T,N>*>(this));
        // NOTE: Use my own instead of the above comment out cast for now
        // because Boost's serialization of std::array is probably much larger
        // in compilation time.
#if HAVE_CXX11_RANGE_BASED_FOR_LOOP
        for (const auto& e : *this) { ar & e; }
#else
        std::for_each(begin(*this), end(*this), [&ar](T& e) { ar & e; });
#endif
    }

    friend vec& rand(vec& a) { rand(*(reinterpret_cast<std::array<T,N>*>(&a)));
// #if HAVE_CXX11_RANGE_BASED_FOR_LOOP
//     for (auto& e : a) { rand(e); }
// #else
//     std::for_each(begin(a), end(a), [](t& e) { rand(e); });
// #endif
        return a;
    }

};

#if HAVE_CXX11_TEMPLATE_ALIASES
template<class T> using vec1 = vec<T,1>;
template<class T> using vec2 = vec<T,2>;
template<class T> using vec3 = vec<T,3>;
template<class T> using vec4 = vec<T,4>;
typedef vec1<float> vec1f;
typedef vec2<float> vec2f;
typedef vec3<float> vec3f;
typedef vec4<float> vec4f;
typedef vec1<double> vec1d;
typedef vec2<double> vec2d;
typedef vec3<double> vec3d;
typedef vec4<double> vec4d;
typedef vec1<long double> vec1l;
typedef vec2<long double> vec2l;
typedef vec3<long double> vec3l;
typedef vec4<long double> vec4l;
#else
typedef vec<float,1> vec1f;
typedef vec<float,2> vec2f;
typedef vec<float,3> vec3f;
typedef vec<float,4> vec4f;
typedef vec<double,1> vec1d;
typedef vec<double,2> vec2d;
typedef vec<double,3> vec3d;
typedef vec<double,4> vec4d;
typedef vec<long double,1> vec1l;
typedef vec<long double,2> vec2l;
typedef vec<long double,3> vec3l;
typedef vec<long double,4> vec4l;
#endif

/*! Find element in \p vtx nearest to \p point.
 */
template<class T, std::size_t N> vec<T,N>* find_nearest(vec<T,N>* vtx, size_t vtxN, const vec<T,N>& point) {
    return std::min_element(vtx, vtx+vtxN, [point](vec<T,N> a, vec<T,N> b) { return (dist(point, a) <
                                                                                                   dist(point, b)); });
}
/*! Find element in \p vtx \em farthest from \p point.
 */
template<class T, std::size_t N> vec<T,N>* find_farthest(vec<T,N>* vtx, size_t vtxN, const vec<T,N>& point) {
    return std::max_element(vtx, vtx+vtxN, [point](vec<T,N> a, vec<T,N> b) { return (dist(point, a) <
                                                                                                   dist(point, b)); });
    // T max_dist = std::numeric_limits<T>::min(); /* maximum distance */
    // size_t max_i = 0;                    /* index to closest vertex */
    // std::for_each(vtx, vtx+vtxN, []() {
    //         T dist = dist(point, &vtx[i]);
    //         if (i == 0) {
    //             max_dist = dist; max_i = 0;
    //         } else {
    //             if (max_dist > dist) {
    //                 max_dist = dist; max_i = i;
    //             }
    //         }
    //     });
    // return max_i;
}

/*! Shift the \p vtxN number of \p mask-masked control points in \p vtx the distance \p mshift. */
template<class T, std::size_t N>
int shiftCtrlP(vec2f* vtx, size_t vtxN, vec2f mshift, const uint8_t * mask)
{
    int ret = 0;
    for (size_t i = 0; i < 8 * sizeof(*mask); i++) {
        if (pnw::get_bit(*mask, i)) {
            if (i < vtxN) {
                vtx[i] += mshift;
                ret = 1;
            } else {
                //PWARN("%s\n", g_shape_warn_index_ctrlp_mask_index);
            }
        }
    }
    return ret;
}

// Note: Activating this makes template algorithms such as \c outer_box fail to match instantiations of for example vec2<T>.
#if 0
/*! 2-Dimensional Vector. */
template<class T> class vec2 : public vec<T,2> {
public:
    /*! Construct from scalar \p a, defaulting to \em origo. */
    vec2(T a = 0) { this->fill(a); }
    vec2(T a, T b) { (*this)[0] = a; (*this)[1] = b; }
};
/*! 3-Dimensional Vector. */
template<class T> class vec3 : public vec<T,3> {
public:
    /*! Construct from scalar \p a, defaulting to \em origo. */
    vec3(T a = 0) { this->fill(a); }
    vec3(T a, T b, T c) { (*this)[0] = a; (*this)[1] = b; (*this)[2] = c; }
};
/*! 4-Dimensional Vector. */
template<class T> class vec4 : public vec<T,4> {
public:
    /*! Construct from scalar \p a, defaulting to \em origo. */
    vec4(T a = 0) { this->fill(a); }
    vec4(T a, T b, T c, T d) { (*this)[0] = a; (*this)[1] = b; (*this)[2] = c; (*this)[3] = d; }
};
#endif

// Show
template<class T> inline std::ostream& show(std::ostream& os, const vec1<T>& a) { os << "(x" << MEMCH << a(0) << ")"; return os; }
template<class T> inline std::ostream& show(std::ostream& os, const vec2<T>& a) { os << "(x" << MEMCH << a(0) << ",y" << MEMCH << a(1) << ")"; return os; }
template<class T> inline std::ostream& show(std::ostream& os, const vec3<T>& a) { os << "(x" << MEMCH << a(0) << ",y" << MEMCH << a(1) << ",z" << MEMCH << a(2) << ")"; return os; }
template<class T> inline std::ostream& show(std::ostream& os, const vec4<T>& a) { os << "(x" << MEMCH << a(0) << ",y" << MEMCH << a(1) << ",z" << MEMCH << a(2) << ",w" << MEMCH << a(3) << ")"; return os; }
template<class T, std::size_t N> inline std::ostream& operator << (std::ostream& os, const vec<T,N>& a) { return show(os, a); }

/*! Cross Product of \p a and \p b. */
template<class T> inline pure vec3<T> cross(const vec3<T>& a,
                                            const vec3<T>& b) {
    return vec3<T>(a(1) * b(2) - a(2) * b(1),
                   a(2) * b(0) - a(0) * b(2),
                   a(0) * b(1) - a(1) * b(0));
}

/*! Rotate \p angle Radians around X-Axis.
 * \param[in] angle is the amount of angle given in radians to rotate.
 */
template<class T> inline pure vec3<T> rotX(const vec3<T>& a, const T angle) {
    T sn, cs; std::sincos(angle, sn,cs);
    const T& y0 = a[1];
    const T& z0 = a[2];
    a[1] = + cs * y0 - sn * z0;
    a[2] = + sn * y0 + cs * z0;
    return a;
}

/*! Rotate \p angle Radians around Y-Axis.
 * \param[in] angle is the amount of angle given in radians to rotate.
 */
template<class T> inline pure vec3<T> rotY(const vec3<T>& a, const T angle) {
    T sn, cs; std::sincos(angle, sn,cs);
    const T& x0 = a[0];
    const T& z0 = a[2];
    a[0] = + cs * x0 + sn * z0;
    a[2] = - sn * x0 + cs * z0;
    return a;
}

/*! Rotate \p angle Radians around Z-Axis.
 * \param[in] angle is the amount of angle given in radians to rotate.
 */
template<class T> inline pure vec3<T> rotZ(const vec3<T>& a, const T angle) {
    T sn, cs; std::sincos(angle, sn,cs);
    const T& x0 = a[0];
    const T& y0 = a[1];
    a[0] = + cs * x0 - sn * y0;
    a[1] = + sn * x0 + cs * y0;
    return a;
}

/*! Cossinus-Sinus of \p angle. */
template<class T> inline pure vec2<T> cossin(T angle) {
    T sn, cs; std::sincos(angle, sn,cs); return vec2<T>(cs, sn);
}
/*! Generate a Cossinus-Sinus Lookup Table at \p tab of length \p n. */
template<class T> inline void cossin(vec2<T>* a, size_t n)
{
    for (size_t i = 0; i < n; i++) { a[i] = cossin(static_cast<T>(M_2PI) * i / n); }
}

#if 0

/*! Componentwise Product between \p a and \p b and put it in \p c. */
inline void
vec2f_pwprod(const vec<T,N>& a, const vec<T,N>& b, vec<T,N>& c)
{
    c(0) = a(0) * b(0);
    c(1) = a(1) * b(1);
}
/*! Componentwise Product between \p a and \p b and put it in \p c. */
inline void
vec2f_pwprodxy(const vec<T,N>& a, float bx, float by, vec<T,N>& c)
{
    c(0) = a(0) * bx;
    c(1) = a(1) * by;
}

/*! Componentwise Quotient between \p a and \p b and put it in \p c. */
inline void
vec2f_pwquot(const vec<T,N>& a, const vec<T,N>& b, vec<T,N>& c)
{
    c(0) = a(0) / b(0);
    c(1) = a(1) / b(1);
}
/*! Componentwise Quotient between \p a and \p b and put it in \p c. */
inline void
vec2f_pwquots(const vec<T,N>& a, const float b, vec<T,N>& c)
{
    c(0) = a(0) / b;
    c(1) = a(1) / b;
}
/*! Componentwise Quotient between \p a and \p b and put it in \p c. */
inline void
vec2f_pwquotxy(const vec<T,N>& a, float bx, float by, vec<T,N>& c)
{
    c(0) = a(0) / bx;
    c(1) = a(1) / by;
}

/*! Componentwise Multiply and Sum.
 * d = b * c + a;
 */
inline void
vec2f_pwprod_sum(const vec<T,N>& a, const vec<T,N>& b, const vec<T,N>& c, vec<T,N>& d)
{
    d(0) = b(0) * c(0) + a(0);
    d(1) = b(1) * c(1) + a(1);
}
/*! Componentwise Scalar Multiply (Scale) and Sum (Shift).
 * d = b * c + a;
 */
inline void
vec2f_pwprods_sum(const vec<T,N>& a, const vec<T,N>& b, float c, vec<T,N>& d)
{
    d(0) = b(0) * c + a(0);
    d(1) = b(1) * c + a(1);
}

/*! Normalize \p a with the \p n-Norm and then scale it again with \p s. */
inline void
vec2f_normalize_and_scale(vec<T,N>& a, int n, float s)
{
    float norm = vec2f_normn(a, n);
    a(0) *= s / norm;
    a(1) *= s / norm;
}

/*! \p min, \p max and \p span along X and Y dimension. */
void
vec2farray_rdSpanXY(const vec<T,N>& a, size_t n,
                    vec<T,N>& min, vec<T,N>& max, vec<T,N>& span);
/*! \p min, \p max and \p span along X dimension. */
void
vec2farray_rdSpanX(const vec<T,N>& a, size_t n,
                   float * min, float * max, float * span);
/*! \p min, \p max and \p span along Y dimension. */
void
vec2farray_rdSpanY(const vec<T,N>& a, size_t n,
                   float * min, float * max, float * span);
/*! \p min, \p max and \p span along radial dimension from * center \p cen. */
void
vec2farray_rdSpanR(const vec<T,N>& a, size_t n, const vec<T,N>& cen,
                   float * min, float * max, float * span);

int vec2f_poly_includes(const vec<T,N>& vtx, size_t vtxN,
                      const vec<T,N>& point);

size_t find_nearest(const vec<T,N>& vtx, size_t vtxN,
                    const vec<T,N>& point);

/*! Check if <em>all</em> of the vector a's components [\p x, \p y] are "Defined", that is "a Number".
 * \return 1 if true, 0 otherwise.
 */

/*! Sum \p a and \p b and put it in \p c. */
void vec2farray_sum(const vec<T,N>& a, const vec<T,N>& b, vec<T,N>& c,
                    size_t n);
/*! Sum \p a and [\p s, \p s] and put it in \p c. */
void vec2farray_sums(const vec<T,N>& a, const float b, vec<T,N>& c,
                     size_t n);

inline void vec2f_wobbleDim(vec<T,N>& fac, float fa, uint8_t prg)
{
    fac(0) = 1 + fa * prg / 256;
    fac(1) = 1 - fa * prg / 256;
}

void
vec2farray_rdSpanXY(const vec<T,N>& a, size_t n,
                    vec<T,N>& min, vec<T,N>& max, vec<T,N>& span)
{
    PTEST("Untested\n");
    min(0) = max(0) = a[0](0);
    min(1) = max(1) = a[0](1);
    for (size_t i = 1; i < n; i++) {
        vec2f_min(min, &a[i], min);
        vec2f_max(max, &a[i], max);
    }
    vec2f_diff(max, min, span);
}
void
vec2farray_rdSpanX(const vec<T,N>& a, size_t n,
                   float * min, float * max, float * span)
{
    *min = a[0](0);
    *max = a[0](0);
    for (size_t i = 1; i < n; i++) {
        *min = std::min(*min, a[i](0));
        *max = std::max(*max, a[i](0));
    }
    *span = *max - *min;
}
void
vec2farray_rdSpanY(const vec<T,N>& a, size_t n,
                   float * min, float * max, float * span)
{
    *min = a[0](1);
    *max = a[0](1);
    for (size_t i = 1; i < n; i++) {
        *min = std::min(*min, a[i](1));
        *max = std::max(*max, a[i](1));
    }
    *span = *max - *min;
}
void
vec2farray_rdSpanR(const vec<T,N>& a, size_t n, const vec<T,N>& cen,
                   float * min, float * max, float * span)
{
    // PTODO("Measure radial distance of all a from cen\n");
    *min = a[0](1);
    *max = a[0](1);
    for (size_t i = 1; i < n; i++) {
        *min = std::min(*min, a[i](1));
        *max = std::max(*max, a[i](1));
    }
    *span = *max - *min;
}

int vec2f_poly_includes(const vec<T,N>& vtx, size_t vtxN,
                        const vec<T,N>& point)
{
    int ret = 1;

    if (vtxN < 3) { PWARN("vtxN < 3\n"); return 0; }

    for (size_t i = 0; i < vtxN; i++) { /* for each vertex index i */
        size_t j = i+1;		/* next vertex index */
        if (j == vtxN) { j = 0; } /* wrap around */

        vec<T,N> side;			/* side vector */
        vec2f_diff(&vtx[j], &vtx[i], &side); /* first => next */

        vec<T,N> norm;			/* unnormalized normal */
        vec2f_rot90(&side, &norm);	/* normal vector orthogonal to side vector */

        vec<T,N> mpv;			/* mouse point vector */
        vec2f_diff(point, &vtx[i], &mpv); /* first => mouse point */

        const float dot = vec2f_dot(&norm, &mpv);
        if (dot < 0) { /* for left-handed x-y coordinate system use > 0 */
            ret = 0;
        } else {
        }
    }

    return ret;
}

#endif
