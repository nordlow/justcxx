/*! \file armadillo_x.hpp
 * \brief Extensions to Armadillo.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-01-12 03:12
 */

#pragma once
#include "cc_features.h"
#include <armadillo>

namespace arma {

#if HAVE_CXX11_VARIADIC_TEMPLATES_disabled
/*! N-ary Version of \c join_rows. */
// template<class T...>
// join_rows()
// {
// }
template<class T, class ... U>
inline const Glue<T, U..., glue_join> join_rows(const Base<typename T::elem_type,T>& A,
                                                const Base<typename T::elem_type,U...>&... B)
{
    return join_rows(A,
                     join_rows(B...));
}
#else

#if HAVE_CXX1Y_RETURN_TYPE_DEDUCTION

template<class T1, class T2, class T3>
inline const auto
join_cols(const Base<typename T1::elem_type,T1>& A,
          const Base<typename T1::elem_type,T2>& B,
          const Base<typename T1::elem_type,T3>& C)
{
    return join_cols(join_cols(A, B), C);
}

template<class T1, class T2, class T3>
inline const auto
join_rows(const Base<typename T1::elem_type,T1>& A,
          const Base<typename T1::elem_type,T2>& B,
          const Base<typename T1::elem_type,T3>& C)
{
    return join_rows(join_rows(A, B), C);
}

template<class T1, class T2, class T3>
inline const auto
join_slices(const BaseCube<typename T1::elem_type,T1>& A,
            const BaseCube<typename T1::elem_type,T2>& B,
            const BaseCube<typename T1::elem_type,T3>& C)
{
    return join_slices(join_slices(A, B), C);
}

#else

template<class T1, class T2, class T3>
inline const auto
join_cols(const Base<typename T1::elem_type,T1>& A,
          const Base<typename T1::elem_type,T2>& B,
          const Base<typename T1::elem_type,T3>& C) -> decltype(join_cols(join_cols(A, B), C))
{
    return join_cols(join_cols(A, B), C);
}

template<class T1, class T2, class T3>
inline const auto
join_rows(const Base<typename T1::elem_type,T1>& A,
          const Base<typename T1::elem_type,T2>& B,
          const Base<typename T1::elem_type,T3>& C) -> decltype(join_rows(join_rows(A, B), C))
{
    return join_rows(join_rows(A, B), C);
}

template<class T1, class T2, class T3>
inline const auto
join_slices(const BaseCube<typename T1::elem_type,T1>& A,
            const BaseCube<typename T1::elem_type,T2>& B,
            const BaseCube<typename T1::elem_type,T3>& C) -> decltype(join_slices(join_slices(A, B), C))
{
    return join_slices(join_slices(A, B), C);
}

#endif

#endif

}
