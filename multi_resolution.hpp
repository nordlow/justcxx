/*! \file multi_resolution.hpp
 * \brief Partially STL-Compliant Multi-Resolution Containers.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \todo Reuse boost::accumulation
 * \see OpenCV \c PyrDown, \c PyrUp and \c PyrSegmentation
 */

#pragma once

#include <vector>
#include <algorithm>
#include "bitwise.hpp"
#include "mean.hpp"
#include "allocators.hpp"
#include "show.hpp"

namespace pnw { namespace multi_resolution {

/*!
 * Partially STL-Compliant Multi-Resolution Vector with Allocator Alignment for each Level.
 * \todo Use __builtin_assume_aligned (GCC-4.7)
 */
//template<class _Tp, class _Alloc = std::allocator<_Tp> >
template<class _Tp, std::size_t _Alignment = 16>
class vector
{
    // Concept requirements.
    typedef AlignmentAllocator<_Tp, _Alignment> _Alloc;
    typedef typename _Alloc::value_type                _Alloc_value_type;
    __glibcxx_class_requires(_Tp, _SGIAssignableConcept)
    __glibcxx_class_requires2(_Tp, _Alloc_value_type, _SameTypeConcept)

    typedef std::_Vector_base<_Tp, _Alloc>			 _Base;
    typedef typename _Base::_Tp_alloc_type		 _Tp_alloc_type;
public:
    typedef _Tp                                      value_type;
    typedef typename _Tp_alloc_type::pointer         pointer;
    typedef typename _Tp_alloc_type::const_pointer   const_pointer;
    typedef typename _Tp_alloc_type::reference       reference;
    typedef typename _Tp_alloc_type::const_reference const_reference;
    typedef size_t                                   size_type;
    typedef ptrdiff_t                                difference_type;
    typedef _Alloc                                   allocator_type;

protected:
    // using _Base::_M_allocate;
    // using _Base::_M_deallocate;
    // using _Base::_M_impl;
    // using _Base::_M_get_Tp_allocator;

public:
    vector(size_t n)
        : m_bot(n), m_datas(nullptr), m_sizes(nullptr) { construct_pyramid(); }
    vector(size_t n, value_type value)
        : m_bot(n, value), m_datas(nullptr), m_sizes(nullptr) { construct_pyramid(); }
    vector(const vector& in)
        : m_bot(in.m_bot), m_datas(nullptr), m_sizes(nullptr) { construct_pyramid(); }

    vector operator = (vector& in) {
        if (this != &in) {
            delete_pyramid();
            m_bot = in.m_bot;
            construct_pyramid();
        }
    }

    ~vector() { delete_pyramid(); }

    // Get Standard Scale Size.
    size_type size() const { return m_bot.size(); }
    // Get Normal Scale Data.
    value_type*       data() { return m_bot.data(); }
    const value_type* data() const { return m_bot.data(); }

    // Get Size at scale \p scale.
    size_type size(size_t scale) const { return m_sizes[scale]; }

    // Get Data at scale \p scale.
    value_type*       data(size_t scale) { return m_datas[scale]; }
    const value_type* data(size_t scale) const { return m_datas[scale]; }

    // Get Standard Element at index \p i.
    value_type& operator[](size_t i) { return m_bot[i]; }
    // Get Constant Standard Element at index \p i.
    const value_type& operator[](size_t i) const { return m_bot[i]; }

    // Get Constant Standard Element at scale \p scale at index \p i.
    value_type*       operator()(size_t scale, size_t i) { return m_datas[scale][i]; }
    const value_type* operator()(size_t scale, size_t i) const { return m_datas[scale][i]; }

    void resize(size_t n) {
        bool ch = (n != size());
        if (ch) { delete_pyramid(); }
        m_bot.resize(n);
        if (ch) { construct_pyramid(); }
    }

    void push_back(const _Tp& a) {
        delete_pyramid();
        m_bot.push_back(a);
        construct_pyramid();
    }
    void pop_back() {
        if (size()) { delete_pyramid(); }
        m_bot.pop_back();
        if (size()) { construct_pyramid(); }
    }
    void clear() {
        if (size()) { delete_pyramid(); }
        m_bot.clear();
    }

    /*! Print \p v to \p os. */
    friend std::ostream& operator << (std::ostream& os,
                                      const vector& v)
    {
        for (size_t s = 0; s < v.scale_count(); s++) { // for each cached scale
            os << "scale:" << s << ' ';
            show_each(v.m_datas[s], v.m_datas[s]+v.m_sizes[s], os);
            os << std::endl;
        }
        return os;
    }

protected:
    size_t scale_count() const { return 1 + pnw::binlog(size()); } // one extra for bottom

    typedef enum {
        EXTRAPOLATION_ZEROS,    ///< Extrapolate with Zeros.
        EXTRAPOLATION_COPY,     ///< Extrapolate By Copying last Value.
        EXTRAPOLATION_COPY_HALF, ///< Extrapolate By Copying last Half Value.
    } EXTRAPOLATION_t;

    typedef enum {
        FILTER_AVERAGE,
        FILTER_BURT_ADELSON     ///< Kernel specified on page 533 of the Burt and Adelson paper.
    } FILTER_t;

    /// Construct Scale \em Pyramid \em Bottom-Up.
    void construct_pyramid(FILTER_t filter = FILTER_AVERAGE,
                           EXTRAPOLATION_t extrapolation = EXTRAPOLATION_ZEROS) {
        if (not m_datas) {      // if no multi-scala yet
            const size_t snum = scale_count();
            if (snum >= 1) {
                m_datas = new value_type* [snum]; // allocate data pointers
                m_sizes = new size_type [snum];   // allocate lengths

                // first level is just copy
                m_datas[0] = m_bot.data();
                m_sizes[0] = m_bot.size();
            }

            // TODO: Replace with STL algorithm, std::for_each()
            for (size_t s = 1; s < snum; s++) { // for each cached scale
                auto sq = m_sizes[s-1] / 2;     // quotient
                auto sr = m_sizes[s-1] % 2;     // rest
                auto sn = m_sizes[s] = sq+sr;
                m_datas
                    [s] = m_alloc.allocate(sn * sizeof(value_type*));

                // TODO: Replace with STL algorithm, std::for_each()
                switch (filter) {
                case FILTER_AVERAGE:
                default:
                    for (size_t i = 0; i < sq; i++) { // for each dyadic reduction
                        m_datas[s][i] = pnw::mean(m_datas[s-1][2*i+0],
                                                  m_datas[s-1][2*i+1]);
                    }
                    break;
                case FILTER_BURT_ADELSON:
                    // TODO: Reuse convolution
                    for (size_t i = 0; i < sq; i++) { // for each dyadic reduction
                        m_datas[s][i] = pnw::mean(m_datas[s-1][2*i+0],
                                                  m_datas[s-1][2*i+1]);
                    }
                    break;
                }
                if (sr) {       // if rest
                    // extrapolationate
                    switch (extrapolation) {
                    case EXTRAPOLATION_ZEROS: m_datas[s][sq] = 0; break;
                    case EXTRAPOLATION_COPY: m_datas[s][sq] = m_datas[s-1][2*sq+0]; break;
                    case EXTRAPOLATION_COPY_HALF: m_datas[s][sq] = m_datas[s-1][2*sq+0] / 2; break;
                    default: m_datas[s][sq] = m_datas[s-1][2*sq+0] / 2; break;
                    }
                }
            }
        }
    }

    /// Delete Scale \em Pyramid.
    void delete_pyramid() {
        if (m_datas) {        // if no multi-scala given yet1
            const size_t snum = scale_count();
            std::for_each(m_datas + 1,
                          m_datas + snum,
                          [this](value_type * data) {
                              this->m_alloc.deallocate(data, sizeof(value_type)); // clear level
                          });
            // for (size_t s = 1; s < snum; s++) { // for each scale
            //     m_alloc.deallocate(m_datas[s], sizeof(value_type)); // clear level
            // }
            delete[] m_datas; m_datas = nullptr; // deallocate scale pointers
            delete[] m_sizes; m_sizes = nullptr; // deallocate scale pointers
        }
    }

    /// Reconstruct Scale Pyramid.
    void reconstruct_pyramid(size_t scale = 0) {
        delete_pyramid();
        construct_pyramid();
    }

private:
    std::vector<value_type, _Alloc> m_bot; ///< Bottom Resolutions.
    mutable value_type** m_datas; ///< Pyramid Resolutions Datas (Cache). Slaves (mutable) under \c m_bot.
    mutable size_type* m_sizes; ///< Pyramid Resolution Lengths. Slaves (mutable) under \c m_bot.
    _Alloc m_alloc;             ///< Allocator.
};

}
}
