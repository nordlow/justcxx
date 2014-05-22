/*! \file memory_x.hpp
 * \brief C++11 unique_ptr utils.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-10-02 00:22
 *
 * http://stackoverflow.com/questions/7038357/make-unique-and-perfect-forwarding
 * \see http://stackoverflow.com/questions/10149840/c-arrays-and-make-unique
 * \see http://channel9.msdn.com/Events/GoingNative/GoingNative-2012/C-11-VC-11-and-Beyond at time 0:42:26
 *
 * \see http://stackoverflow.com/questions/9794267/double-array-using-unique-ptr
 * a[i] = std::unique_ptr<int[]>(new int[2]);
 */

#pragma once
#include <memory>
#include <type_traits>
#include <utility>

namespace std {

template<class T, class... Args>
std::unique_ptr<T> make_unique_helper(std::false_type, Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<class T, class... Args>
std::unique_ptr<T> make_unique_helper(std::true_type, Args&&... args) {
    static_assert(std::extent<T>::value == 0,
                  "make_unique<T[N]>() is forbidden, please use make_unique<T[]>().");

    typedef typename std::remove_extent<T>::type U;
    return std::unique_ptr<T>(new U[sizeof...(Args)]{std::forward<Args>(args)...});
}

template<class T, class... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return make_unique_helper<T>(std::is_array<T>(), std::forward<Args>(args)...);
}

// template<typename T>
// inline unique_ptr<T> make_unique(T * ptr) { return std::unique_ptr<T>(ptr); }
// template<typename T, typename... Args>
// std::unique_ptr<T> make_unique(Args&&... args)
// {
//     return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
// }

}
