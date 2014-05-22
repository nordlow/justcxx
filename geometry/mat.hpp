/*! \file mat.hpp
 * \brief Statically-Sized M-by-N Matrix.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <boost/serialization/access.hpp>
#include "vec.hpp"
#include "../cc_features.h"

/*! MxN Matrix with Column-Major Storage. */
template<class T, std::size_t M, std::size_t N>
class mat : public vec<vec<T, M>, N> // vector of column vectors
{
public:
    mat(T a) : vec<vec<T, M>, N>(a) {}
#if NDEBUG
    /*! Default Construct Uninitialized. */
    mat() {}
#else
    /*! Default Construct to Zero-Initialized. */
    mat() : vec<vec<T, M>, N>(0) {}
#endif

#if HAVE_CXX11_VARIADIC_TEMPLATES
#else
    /// Construct from 9 scalars.
    mat(T a, T b, T c,
        T d, T e, T f,
        T g, T h, T i) {
        mat& m = *this;
        m(0,0) = a; m(0,1) = b; m(0,2) = c;
        m(1,0) = d; m(1,1) = e; m(1,2) = f;
        m(2,0) = g; m(2,1) = h; m(2,2) = i;
    }
    /// Construct from 16 scalars.
    mat(T a, T b, T c, T d,
        T e, T f, T g, T h,
        T i, T j, T k, T l,
        T m, T n, T o, T p) {
        mat& x = *this;
        x(0,0) = a; x(0,1) = b; x(0,2) = c; x(1,3) = d;
        x(1,0) = e; x(1,1) = f; x(2,2) = g; x(2,3) = h;
        x(2,0) = i; x(2,1) = j; x(2,2) = k; x(2,3) = l;
        x(3,0) = m; x(3,1) = n; x(3,2) = k; x(3,3) = p;
    }
#endif

    /// Construct Rotation Matrix rotating the angle \p a around the axis rotaxis.
    mat(const vec<T,3>& rotaxis, T a) { origo_rotator(rotaxis, a); }

    /// Generate a rotation matrix around the normalized axis r and angle.
    void origo_rotator(const vec<T,3>& rotaxis, T angle);

    /// Reorthogonalize matrix.
    void reorthogonalize(size_t limit = 10);

    /// \name Element Access.
    /// \{
    /// Get \p i:th Row.
    vec<T,N> row(size_t i) const {
        vec<T,N> r;
        size_t c = 0;
        for (auto e : *this) { r(c++) = e(i); }
        return r;
    }
    vec<T,N>& row(size_t i) {
        vec<T,N> r;
        size_t c = 0;
        for (auto e : *this) { r(c++) = e(i); }
        return r;
    }
    /// Get \p j:th Column.
    const vec<T,M>  col(size_t j) const { return (*this)[j]; } // column-major storage is easy
    const vec<T,M>& col(size_t j)       { return (*this)[j]; } // column-major storage is easy
    /// Get Element (\p i, \p j).
    T  operator () (size_t i, size_t j) const { return (*this)[j][i]; }
    /// Get Element (\p i, \p j).
    T& operator () (size_t i, size_t j)       { return (*this)[j][i]; }
    /// Get Element (\p i) in Column-Major Order.
    T  operator () (size_t i) const { return (*this)[0][i]; }
    /// Get Element (\p i) in Column-Major Order.
    T& operator () (size_t i)       { return (*this)[0][i]; }
    /// \}

    // Binary Arithmetic and Assignment.
    void premul(const mat& a) { *this = a * *this; }
    void postmul(const mat& a) { *this = *this * a; }
    void operator *= (const mat& a) { postmul(a); }

    /// Transpose \c this.
    void transpose() { ip_transpose(*this); } // delegate
    /// Transpose \c this.
    void t() { transpose(); } // alias

    /// Invert \c this.
    void inv() { *this = inv(*this); }

    /*! Show. */
    friend std::ostream& show(std::ostream& os, const mat& m) {
        os << "[ "
           << m(0,0) << ' ' << m(0,1) << ' ' << m(0,2)
           << " ; "
           << m(1,0) << ' ' << m(1,1) << ' ' << m(1,2)
           << " ; "
           << m(2,0) << ' ' << m(2,1) << ' ' << m(2,2)
           << " ]";
        return os;
    }
    friend std::ostream& operator << (std::ostream& os, const mat& m) { return show(os, m); }

    template<class S> friend pure S concept_name(const mat& m) { S s("0x0-Dimensional Matrix"); s[0] += M; s[2] += N; return s; }
    template<class S> friend pure S typenameof(const mat& a) { S s("vec<T,M,N>"); return s; }

    friend class boost::serialization::access;
    template<class Ar> void serialize(Ar& ar, const uint version) {
#if HAVE_CXX11_RANGE_BASED_FOR_LOOP
        for (auto& e : *this) { ar & e; }
#else
        std::for_each(begin(*this), end(*this), [&ar](T& e) { ar & e; });
#endif
    }
};

#if HAVE_CXX11_TEMPLATE_ALIASES
template<class T> using mat2 = mat<T,2,2>;
template<class T> using mat3 = mat<T,3,3>;
template<class T> using mat4 = mat<T,4,4>;
template<class T> using mat23 = mat<T,2,3>;
template<class T> using mat32 = mat<T,3,2>;
template<class T> using mat34 = mat<T,3,4>;
template<class T> using mat43 = mat<T,4,3>;
#endif
typedef mat<float,2,2> mat2f;
typedef mat<float,3,3> mat3f;
typedef mat<float,4,4> mat4f;
typedef mat<double,2,2> mat2d;
typedef mat<double,3,3> mat3d;
typedef mat<double,4,4> mat4d;
typedef mat<long double,2,2> mat2l;
typedef mat<long double,3,3> mat3l;
typedef mat<long double,4,4> mat4l;

template<class T, std::size_t N> inline void read_rows(const mat<T,3,N>& m,
                                                       vec<T,N>& a,
                                                       vec<T,N>& b,
                                                       vec<T,N>& c) {
    a = m.row(0);
    b = m.row(1);
    c = m.row(2);
}

/// Right-Handed Orthogonal Normalized (Orthonormal) Base.
template<class T> inline pure mat<T,3,3> right_onbase(const vec<T,3>& a,
                                                      const vec<T,3>& b) {
    return mat<T,3,3>(a, b, cross(a, b));
}

template<class T> inline void get_all(const mat<T,4,4>& mat,
                                      T& a, T& b, T& c, T& d,
                                      T& e, T& f, T& g, T& h,
                                      T& i, T& j, T& k, T& l,
                                      T& m, T& n, T& o, T& p)
{
    a = mat(0,0); b = mat(0,1); c = mat(0,2); d = mat(0,3);
    e = mat(1,0); f = mat(1,1); g = mat(1,2); h = mat(1,3);
    i = mat(2,0); j = mat(2,1); k = mat(2,2); l = mat(2,3);
    m = mat(3,0); n = mat(3,1); o = mat(3,2); p = mat(3,3);
}

/// \name Diagonal Matrix.
/// Diagonal Matrix from vectors \p v.
template<class T, std::size_t P> inline pure mat<T,P,P> diag(const vec<T,P> v) {
    mat<T,P,P> m;
    for (size_t i = 0; i != P; i++) {
        for (size_t j = 0; j != P; j++) {
            m(i,j) = (i == j ? v(i) : 0);
        }
    }
    return m;
}
/// Diagonal Matrix from scalars \p v.
template<class T> inline pure mat<T,2,2> diag(T v0, T v1) { return diag(vec<T,2>({v0,v1})); }
/// Diagonal Matrix from scalars \p v.
template<class T> inline pure mat<T,3,3> diag(T v0, T v1, T v2) { return diag(vec<T,2>({v0,v1,v2})); }
/// \}

/// \name Matrix-Vector Multiply.
/// \{
template<class T> inline pure vec<T,2> operator *(const mat<T,2,2>& m,
                                                  const vec<T,2>& v) {
    vec<T,2> y;
    // 4 muls, 2 adds
    y(0) = m(0,0) * v(0) + m(0,1) * v(1);
    y(1) = m(1,0) * v(0) + m(1,1) * v(1);
    return y;
}
template<class T> inline pure vec<T,3> operator *(const mat<T,3,3>& m,
                                                  const vec<T,3>& v) {
    // 9 muls, 6 adds
    vec<T,3> y;
    y(0) = m(0,0) * v(0) + m(0,1) * v(1) + m(0,2) * v(2);
    y(1) = m(1,0) * v(0) + m(1,1) * v(1) + m(1,2) * v(2);
    y(2) = m(2,0) * v(0) + m(2,1) * v(1) + m(2,2) * v(2);
    return y;
}
template<class T> inline pure vec<T,4> operator *(const mat<T,4,4>& m,
                                                  const vec<T,4>& v) {
    vec<T,4> y;
    // 9 muls, 6 adds
    y(0) = m(0,0) * v(0) + m(0,1) * v(1) + m(0,2) * v(2) + m(0,3) * v(3);
    y(1) = m(1,0) * v(0) + m(1,1) * v(1) + m(1,2) * v(2) + m(1,3) * v(3);
    y(2) = m(2,0) * v(0) + m(2,1) * v(1) + m(2,2) * v(2) + m(2,3) * v(3);
    y(3) = m(3,0) * v(0) + m(3,1) * v(1) + m(3,2) * v(2) + m(3,3) * v(3);
    return y;
}
/// \}

/// \name Matrix-Matrix Multiply.
/// \{
template<class T> inline pure mat<T,2,2> operator *(const mat<T,2,2>& a,
                                                    const mat<T,2,2>& b) {
    mat<T,2,2> y;
    // 8 muls, 4 adds
    y(0,0) = a(0,0) * b(0,0) + a(0,1) * b(1,0);
    y(0,1) = a(0,0) * b(0,1) + a(0,1) * b(1,1);
    y(1,0) = a(1,0) * b(0,0) + a(1,1) * b(1,0);
    y(1,1) = a(1,0) * b(0,1) + a(1,1) * b(1,1);
    return y;
}
template<class T> inline pure mat<T,3,3> operator *(const mat<T,3,3>& a,
                                                    const mat<T,3,3>& b) {
    mat<T,3,3> y;
    // 27 muls, 18 adds
    y(0,0) = a(0,0) * b(0,0) + a(0,1) * b(1,0) + a(0,2) * b(2,0);
    y(0,1) = a(0,0) * b(0,1) + a(0,1) * b(1,1) + a(0,2) * b(2,1);
    y(0,2) = a(0,0) * b(0,2) + a(0,1) * b(1,2) + a(0,2) * b(2,2);
    y(1,0) = a(1,0) * b(0,0) + a(1,1) * b(1,0) + a(1,2) * b(2,0);
    y(1,1) = a(1,0) * b(0,1) + a(1,1) * b(1,1) + a(1,2) * b(2,1);
    y(1,2) = a(1,0) * b(0,2) + a(1,1) * b(1,2) + a(1,2) * b(2,2);
    y(2,0) = a(2,0) * b(0,0) + a(2,1) * b(1,0) + a(2,2) * b(2,0);
    y(2,1) = a(2,0) * b(0,1) + a(2,1) * b(1,1) + a(2,2) * b(2,1);
    y(2,2) = a(2,0) * b(0,2) + a(2,1) * b(1,2) + a(2,2) * b(2,2);
    return y;
}
template<class T> inline pure mat<T,4,4> operator *(const mat<T,4,4>& a,
                                                    const mat<T,4,4>& b) {
    mat<T,4,4> y;
    // 64 muls, 48 adds
    y(0,0) = a(0,0)*b(0,0) + a(0,1)*b(1,0) + a(0,2)*b(2,0) + a(0,3)*b(3,0);
    y(0,1) = a(0,0)*b(0,1) + a(0,1)*b(1,1) + a(0,2)*b(2,1) + a(0,3)*b(3,1);
    y(0,2) = a(0,0)*b(0,2) + a(0,1)*b(1,2) + a(0,2)*b(2,2) + a(0,3)*b(3,2);
    y(0,3) = a(0,0)*b(0,3) + a(0,1)*b(1,3) + a(0,2)*b(2,3) + a(0,3)*b(3,3);
    y(1,0) = a(1,0)*b(0,0) + a(1,1)*b(1,0) + a(1,2)*b(2,0) + a(1,3)*b(3,0);
    y(1,1) = a(1,0)*b(0,1) + a(1,1)*b(1,1) + a(1,2)*b(2,1) + a(1,3)*b(3,1);
    y(1,2) = a(1,0)*b(0,2) + a(1,1)*b(1,2) + a(1,2)*b(2,2) + a(1,3)*b(3,2);
    y(1,3) = a(1,0)*b(0,3) + a(1,1)*b(1,3) + a(1,2)*b(2,3) + a(1,3)*b(3,3);
    y(2,0) = a(2,0)*b(0,0) + a(2,1)*b(1,0) + a(2,2)*b(2,0) + a(2,3)*b(3,0);
    y(2,1) = a(2,0)*b(0,1) + a(2,1)*b(1,1) + a(2,2)*b(2,1) + a(2,3)*b(3,1);
    y(2,2) = a(2,0)*b(0,2) + a(2,1)*b(1,2) + a(2,2)*b(2,2) + a(2,3)*b(3,2);
    y(2,3) = a(2,0)*b(0,3) + a(2,1)*b(1,3) + a(2,2)*b(2,3) + a(2,3)*b(3,3);
    y(3,0) = a(3,0)*b(0,0) + a(3,1)*b(1,0) + a(3,2)*b(2,0) + a(3,3)*b(3,0);
    y(3,1) = a(3,0)*b(0,1) + a(3,1)*b(1,1) + a(3,2)*b(2,1) + a(3,3)*b(3,1);
    y(3,2) = a(3,0)*b(0,2) + a(3,1)*b(1,2) + a(3,2)*b(2,2) + a(3,3)*b(3,2);
    y(3,3) = a(3,0)*b(0,3) + a(3,1)*b(1,3) + a(3,2)*b(2,3) + a(3,3)*b(3,3);
    return y;
}
/// \}

/// Column-wise creation of Matrix using Column Vectors \p a and \p b.
template<class T> inline pure mat<T,2,2> colwise_mat(const vec<T,2>& a,
                                                     const vec<T,2>& b) {
    return mat<T,2,2>(a(0), b(0),
                      a(1), b(1));
}
/// Row-wise creation of matrix using Row Vectors \p a and \p b.
template<class T> inline pure mat<T,2,2> rowwise_mat(const vec<T,2>& a,
                                                     const vec<T,2>& b) {
    return mat<T,2,2>(a(0), a(1),
                      b(0), b(1));
}

/// \name Determinant.
/// \{
/// Get Determinant of \p a.
template<class T> inline pure T det(const mat<T,2,2>& a) {
    return (a(0,0) * a(1,1) -
            a(0,1) * a(1,0));
}
/// Get Determinant of \p a.
template<class T> inline pure T det(const mat<T,3,3>& a) {
    return (+ a(0,0) * a(1,1) * a(2,2)
            + a(0,1) * a(1,2) * a(2,0)
            + a(0,2) * a(1,0) * a(2,1)
            - a(0,0) * a(1,2) * a(2,1)
            - a(0,1) * a(1,0) * a(2,2)
            - a(0,2) * a(1,1) * a(2,0));
}
/// \}

/// \name Transposition.
/// \{
/// Get Transpose of \p a.
template<class T> inline pure mat<T,2,2> transpose(const mat<T,2,2>& m) { return mat<T,2,2>(m(0,0), m(1,0),
                                                                                            m(0,1), m(1,1)); }
/// Get Transpose of \p a.
template<class T> inline pure mat<T,3,3> transpose(const mat<T,3,3>& a) { return mat<T,3,3>(a(0,0), a(1,0), a(2,0),
                                                                                            a(0,1), a(1,1), a(2,1),
                                                                                            a(0,2), a(1,2), a(2,2)); }
template<class T> inline pure mat<T,4,4> transpose(const mat<T,4,4>& m) { return mat<T,4,4>(m(0,0), m(1,0), m(2,0), m(3,0),
                                                                                            m(0,1), m(1,1), m(2,1), m(3,1),
                                                                                            m(0,2), m(1,2), m(2,2), m(3,2),
                                                                                            m(0,3), m(1,3), m(2,3), m(3,3)); }
/// In-Place-Transpose \p a.
template<class T> inline void ip_transpose(mat<T,2,2>& a) { using std::swap; swap(a(1,0), a(0,1)); }
/// In-Place-Transpose \p a.
template<class T> inline void ip_transpose(mat<T,3,3>& a) {
    T              &b = a(1,0), &c = a(2,0);
    T &d = a(0,1),              &f = a(2,1);
    T &g = a(0,2), &h = a(1,2);
    using std::swap;
    swap(b,d);
    swap(c,g);
    swap(f,h);
}
/// In-Place-Transpose \p a.
template<class T> inline void ip_transpose(mat<T,4,4>& a)
{
    T                & b = a(0,1),& c = a(0,2),& d = a(0,3);
    T& e = a(1,0),                & g = a(1,2),& h = a(1,3);
    T& i = a(2,0),& j = a(2,1),                & l = a(2,3);
    T& m = a(3,0),& n = a(3,1),& o = a(3,2)                ;
    using std::swap;
    swap(b,e);
    swap(c,i);
    swap(d,m);
    swap(g,j);
    swap(h,n);
    swap(l,o);
}
/// \}

/// \name Inversion.
/// \{
/// Get Inverse of \p a.
template<class T> inline pure mat<T,2,2> inv(const mat<T,2,2>& a) {
    auto d = det(a);
    return mat<T,2,2>(+ a(1,1) / d, - a(0,1) / d,
                      - a(1,0) / d, + a(0,0) / d);
}
/// Get Inverse of \p m.
template<class T> inline pure mat<T,3,3> inv(const mat<T,3,3>& m) {
    // elements
    const T& a = m(0,0),& b = m(1,0),& c = m(2,0);
    const T& d = m(0,1),& e = m(1,1),& f = m(2,1);
    const T& g = m(0,2),& h = m(1,2),& i = m(2,2);
    // cofactors
    const T A = +e*i-h*f, B = -d*i+g*f, C = +d*h-g*e;
    const T D = -b*i+h*c, E = +a*i-g*c, F = -a*h+g*b;
    const T G = +b*f-e*c, H = -a*f+d*c, I = +a*e-d*b;
    // determinant
    const T det = a*A + b*B + c*C;
    // \todo WHIch should we choose here?
    // inverse(adjoint matrix / det)
//          return mat(A/det, B/det, C/det,
//                       D/det, E/det, F/det,
//                       G/det, H/det, I/det);
    return mat<T,3,3>(A, B, C,
                      D, E, F,
                      G, H, I) / det;
}

#if 0
/// Get Inverse of \p a.
template<class T> inline pure mat<T,4,4> inv(const mat<T,4,4>& mat) {
    T a,b,c,d, e,f,g,h, i,j,k,l, m,n,o,p; // elements
    get_all(mat,
            a,b,c,d,
            e,f,g,h,
            i,j,k,l,
            m,n,o,p);
    // cofactors
    const T A = + f*k*p + g*l*n + h*j*o - f*l*o - g*j*p - h*k*n;
    const T B = - e*k*p - g*l*m - h*i*o + e*l*o + g*i*p + h*k*m;
    const T C = + e*j*p + f*l*m + h*i*n - e*l*n - f*i*p - h*j*m;
    const T D = - e*j*o - f*k*m - g*i*n + e*k*n + f*i*o + g*j*m;

    const T E = - b*k*p - c*l*n - d*j*o + b*l*o + c*j*p + d*k*n;
    const T F = + a*k*p + c*l*m + d*i*o - a*l*o - c*i*p - d*k*m;
    const T G = - a*j*p - b*l*m - d*i*n + a*l*n + b*i*p + d*j*m;
    const T H = + a*j*o + b*k*m + c*i*n - a*k*n - b*i*o - c*j*m;

    const T I = + b*g*p + c*h*n + d*f*o - b*h*o - c*f*p - d*g*n;
    const T J = - a*g*p - c*h*m - d*e*o + a*h*o + c*e*p + d*g*m;
    const T K = + a*f*p + b*h*m + d*e*n - a*h*n - b*e*p - d*f*m;
    const T L = - a*f*o - b*g*m - c*e*n + a*g*n + b*e*o + c*f*m;

    const T M = - b*g*l - c*h*j - d*f*k + b*h*k + c*f*l + d*g*j;
    const T N = + a*g*l + c*h*i + d*e*k - a*h*k - c*e*l - d*g*i;
    const T O = - a*f*l - b*h*i - d*e*j + a*h*j + b*e*l + d*f*i;
    const T P = + a*f*k + b*g*i + c*e*j - a*g*j - b*e*k - c*f*i;

    const T det = a*A + b*B + c*C + d*D; // determinant
    // TODO: Why does the following line fail to compile?
    return mat<T,4,4>(A, E, I, M,
                      B, F, J, N,
                      C, G, K, O,
                      D, H, L, P) / det;
}
/// \}
#endif

/*! 2-Dimensional Rotation Matrix with the angle \p a (in radians).
 * Use generated matrix together with \c mvprod() to rotate
 * vectors.
 * \param[in] a Rotation Angle [Radians]
 * \see \c rot90() and \c rotm90() for special cases
 * \todo Wrap \a in Boost.Unit Radians?
 */
template<class T> inline spure mat<T,2,2> rotation(T a) {
    T c = std::cos(a);
    T s = std::sin(a);
    mat<T,2,2> m;
    m(0,0) =  c; m(0,1) = -s;
    m(1,0) =  s; m(1,1) =  c;
    return m;
}

/*! Rotate the vector \p a +90 degrees and put the result in \p b.
 * \use \c rotation() with \c mvprod() for general rotation
 */
template<class T> inline pure vec<T,2> rot90(const vec<T,2>& a) { return vec<T,2>(-a(1), a(0)); }
/*! Rotate the vector \p a -90 degrees and put the result in \p b.
 * \use \c rotation() with \c mvprod() for general rotation
 */
template<class T> inline pure vec<T,2> rotm90(const vec<T,2>& a) { return vec<T,2>(a(1), -a(0)); }

template<class T> inline pure mat<T,3,3> origo_rotator(const vec<T,3>& rotaxis, T angle)
{
    mat<T,3,3> d;

    // \see Computer Graphics, p.420 Eq.11-40

    T s = std::cos(angle / 2);
    T sina2 = std::sin(angle / 2);

    auto ra = normalize(rotaxis);
    T a = sina2 * ra(0);
    T b = sina2 * ra(1);
    T c = sina2 * ra(2);

#if 1
    // lower round-off error version
    T aa = a * a, bb = b * b, cc = c * c;
    T ab = a * b, bc = b * c, ac = a * c;
    T sa = s * a, sb = s * b, sc = s * c;

    d[0][0] = 1 - 2.0 * (bb + cc);
    d[0][1] =     2.0 * (ab - sc);
    d[0][2] =     2.0 * (ac + sb);

    d[1][0] =     2.0 * (ab + sc);
    d[1][1] = 1 - 2.0 * (aa + cc);
    d[1][2] =     2.0 * (bc - sa);

    d[2][0] =     2.0 * (ac - sb);
    d[2][1] =     2.0 * (bc + sa);
    d[2][2] = 1 - 2.0 * (aa + bb);
#else
    // higher performance version

    T a2 = a * 2.0;
    T b2 = b * 2.0;
    T c2 = c * 2.0;

    T aa2 = a * a2, bb2 = b * b2, cc2 = c * c2;
    T ab2 = a * b2, bc2 = b * c2, ac2 = a * c2;
    T sa2 = s * a2, sb2 = s * b2, sc2 = s * c2;

    d[0][0] = 1 - (bb2 + cc2);
    d[0][1] =     (ab2 - sc2);
    d[0][2] =     (ac2 + sb2);

    d[1][0] =     (ab2 + sc2);
    d[1][1] = 1 - (aa2 + cc2);
    d[1][2] =     (bc2 - sa2);

    d[2][0] =     (ac2 - sb2);
    d[2][1] =     (bc2 + sa2);
    d[2][2] = 1 - (aa2 + bb2);
#endif
    return d;
}

#if 0
// \todo NEEds some more mat::operators

/*! Matrix Orthogonalization.
 *
 * Eric Raible from "Graphics Gems", Andrew S. Glassner, Academic Press, 1990
 *
 * Reorthogonalize matrix R - that is find an orthogonal matrix that is
 * "close" to R by computing an approximation to the orthogonal matrix
 *
 *           T  -1/2
 *   RC = R(R R)
 *                              T      -1
 * [RC is orthogonal because (RC) = (RC) ]
 *				               	                    -1/2
 * To compute C, we evaluate the Taylor expansion of F(x) = (I + x)
 * (where x = C - I) about x=0.
 * This gives C = I - (1/2)x + (3/8)x^2 - (5/16)x^3 + ...
 */

template<class T> inline void mat<T,3,3>::reorthogonalize(size_t limit)
{

    // Constants taken From Mathematica
    static const T coef[10] ={
        1,
        -1 / 2.,
        3 / 8.,
        -5 / 16.,
        35 / 128.,
        -63 / 256.,
        231 / 1024.,
        -429 / 2048.,
        6435 / 32768.,
        -12155 / 65536.
    };

    limit = std::min(limit, (size_t)10);    // see to that we have enough coef's

    mat& r = *this;
    mat tmp;
    tmp = ::transpose(r);      // R'
    tmp = tmp * r;              // R' * R

    mat i = vec<T> (1,1,1);
    mat x = tmp - i;           // RtR - i
    mat xp = vec<T> (1,1,1);     // x^0
    mat sum = vec<T> (1,1,1);    // coef[0] * x^0

    for (size_t power = 1; power < limit; ++power)
    {
        xp = xp * x;
        tmp = coef[power] * xp;
        sum += tmp;
    }

    r = r * sum;
}
#endif

// FIXME: Move to C-version.
// FIXME: does not work correctly with other code.
#if 0
inline void smvmul(vec4<float>& y,
                   const mat<float,4,4>& m,
                   const vec4<float>& x)
{
    float* yy = y.d;
    const float* mm = &(m.d[0][0]);
    const float* xx = x.d;

    __asm__ volatile(	          /* Uses mm0 ... mm5 */

        "movq   (%2), %%mm0 \n\t" /* Read X[ 0 ] and X[ 1 ] */
        "movq  8(%2), %%mm1 \n\t" /* Read X[ 2 ] and X[ 3 ] */
        "movq   (%1), %%mm2 \n\t" /* Read M[ 0 ] and M[ 1 ] */
        "movq  8(%1), %%mm3 \n\t" /* Read M[ 2 ] and M[ 3 ] */

        "pfmul %%mm0, %%mm2 \n\t"
        "pfmul %%mm1, %%mm3 \n\t"
        "pfadd %%mm2, %%mm3 \n\t"
        "pfacc %%mm3, %%mm3 \n\t"
        "movq 16(%1), %%mm4 \n\t" /* Read M[ 4 ] and M[ 5 ] */
        "movq 24(%1), %%mm5 \n\t" /* Read M[ 6 ] and M[ 7 ] */
        "pfmul %%mm0, %%mm4 \n\t"
        "pfmul %%mm1, %%mm5 \n\t"
        "pfadd %%mm4, %%mm5 \n\t"
        "pfacc %%mm5, %%mm5 \n\t"
        "punpckldq %%mm5, %%mm3 \n\t" /* mm3 = mm5(63:32):mm3(31:0) */
        "movq  %%mm3,   (%0)\n\t" /* Write Y[ 0 ] and Y[ 1 ] */

        "movq 32(%1), %%mm2 \n\t" /* Read M[ 0 ] and M[ 1 ] */
        "movq 40(%1), %%mm3 \n\t" /* Read M[ 2 ] and M[ 3 ] */
        "pfmul %%mm0, %%mm2 \n\t"
        "pfmul %%mm1, %%mm3 \n\t"
        "pfadd %%mm2, %%mm3 \n\t"
        "pfacc %%mm3, %%mm3 \n\t"
        "movq 48(%1), %%mm4 \n\t" /* Read M[ 4 ] and M[ 5 ] */
        "movq 56(%1), %%mm5 \n\t" /* Read M[ 6 ] and M[ 7 ] */
        "pfmul %%mm0, %%mm4 \n\t"
        "pfmul %%mm1, %%mm5 \n\t"
        "pfadd %%mm4, %%mm5 \n\t"
        "pfacc %%mm5, %%mm5 \n\t"
        "punpckldq %%mm5, %%mm3 \n\t" /* mm3 = mm5(63:32):mm3(31:0) */
        "movq  %%mm3,   8(%0)\n\t" /* Write Y[ 2 ] and Y[ 3 ] */
        : :
          "r" (yy), "r" (mm), "r" (xx) );

    __asm__ volatile("femms\n\t");
}
#endif
