/*!
 * \file value_packed_vector.hpp
 * \brief A Vector whose elements are packed based on the
 *        strides and offsets relative to each other.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com).
 */

#pragma once

#include <vector>
#include <iostream>

#include "pnw_types.h"
#include "gridfit.hpp"

// ============================================================================

/*!
 * A Vector Container whose elements are packed based on the offsets
 * and strides relative to each other.
 *
 * Note: Useful for storing small \em arrays of \em pointers
 * especially on \em 64-bit machines.
 */
template<class T>
class value_packed_vector
{
public:
    // Handy aliases.
    typedef std::vector<uint8_t> v8;
    typedef std::vector<uint16_t> v16;
    typedef std::vector<uint32_t> v32;
    typedef std::vector<uint64_t> v64;

    typedef enum {
        PTYPE_NONE,
        PTYPE_UINT8,
        PTYPE_UINT16,
        PTYPE_UINT32,
        PTYPE_UINT64,
    } PackType;

    typedef struct {
        PackType type;
        union {
            v8  *u8; ///< Packed into 8-bit unsigned ints.
            v16 *u16; ///< Packed into 16-bit unsigned ints.
            v32 *u32; ///< Packed into 32-bit unsigned ints.
            v64 *u64; ///< Packed into 64-bit unsigned ints.
        } d;
    } PackDat;

    /// Create empty.
    value_packed_vector(void) {
        m_dP.d.u8 = NULL;
        reset();
    }

    /// Append element \p a.
    void push_back(const T& a) {
        std::cout << "\todo " << std::endl;
        if (m_dU == NULL) { m_dU = new std::vector<T>; }
        m_dU->push_back(a);
        pack();
    }

    /// Append all elements in \p a.
    void push_back(const std::vector<T>& a) {
        std::cout << "\todo " << std::endl;
        if (m_dU == NULL) { m_dU = new std::vector<T>; }
        m_dU->resize(m_dU->size() + a.size());
        copy(m_dU->begin(), m_dU->end(), m_dU->end() - a.size());
        pack();
    }

    /// Clear.
    void clear() {
        if (m_dU) { delete m_dU; m_dU = NULL; }
        clear(m_dP);
        reset();
    }

private:
    void reset(void) {
        m_dU = 0;

        m_umin = pnw::maxof<T>();
        m_umax = pnw::minof<T>();
        m_uors = 0;

        m_dP.type = PTYPE_NONE;
        m_dP.d.u8 = NULL;
    }

    /// Clear.
    void clear(PackDat& dP) {
        if (dP.d.u8) {
            switch (dP.type) {
            case PTYPE_UINT8:  delete dP.d.u8; dP.d.u8   = NULL; break;
            case PTYPE_UINT16: delete dP.d.u16; dP.d.u16 = NULL; break;
            case PTYPE_UINT32: delete dP.d.u32; dP.d.u32 = NULL; break;
            case PTYPE_UINT64: delete dP.d.u64; dP.d.u64 = NULL; break;
            default: break;
            }
        }
        dP.type = PTYPE_NONE;
    }

    /// Clear.
    size_t get_size(PackDat& dP) {
        size_t size = 0;
        if (dP.d.u8) {
            switch (dP.type) {
            case PTYPE_UINT8: size = dP.d.u8->size(); break;
            case PTYPE_UINT16: size = dP.d.u16->size(); break;
            case PTYPE_UINT32: size = dP.d.u32->size(); break;
            case PTYPE_UINT64: size = dP.d.u64->size(); break;
            default: break;
            }
        } else {
            size = 0;
        }
        return size;
    }

    void pack(void) {
        if (m_dU) { pack(*m_dU, m_dP); }
    }

    /// Calculate position of LSB-set in \p m_uors.
    uint get_ushift() const {
        uint ushift = 0;
        for (uint bi = 0; bi < 64; bi++) {
            if (m_uors& (1 << bi)) { ushift = bi; break; }
        }
        return ushift;
    }

    uint get_uspan(uint ushift) const {
        return (m_umax - m_umin) >> ushift;
    }

    /// Unpack data from \p dP into \p dU.
    void unpack(PackDat& dP,
                std::vector<T>& dU) const {
        dU.resize(dP.size());
        auto ui = begin(dU);

        const uint ushift = get_ushift();

        switch (m_dP.type) {
        case PTYPE_UINT8: {
            auto pi = dP.d.u8->begin();
            for (ui = begin(dU); ui != end(dU); ui++, pi++) {
                *ui = (*pi << ushift) + m_umin; // unpack element \c ui to \c pi
            }
            break;
        }
        case PTYPE_UINT16: {
            auto pi = dP.d.u16->begin();
            for (ui = begin(dU); ui != end(dU); ui++, pi++) {
                *ui = (*pi << ushift) + m_umin; // unpack element \c ui to \c pi
            }
            break;
        }
        case PTYPE_UINT32: {
            auto pi = dP.d.u32->begin();
            for (ui = begin(dU); ui != end(dU); ui++, pi++) {
                *ui = (*pi << ushift) + m_umin; // unpack element \c ui to \c pi
            }
            break;
        }
        case PTYPE_UINT64: {
            auto pi = dP.d.u64->begin();
            for (ui = begin(dU); ui != end(dU); ui++, pi++) {
                *ui = (*pi << ushift) + m_umin; // unpack element \c ui to \c pi
            }
            break;
        }
        default:
            break;
        }
    }

    PackType uspan_to_PTYPE(uint uspan) const {
        PackType ptype = PTYPE_NONE;
        if      (uspan <= std::numeric_limits<uint8_t>::max())  { ptype = PTYPE_UINT8; }
        else if (uspan <= std::numeric_limits<uint16_t>::max()) { ptype = PTYPE_UINT16; }
        else if (uspan <= std::numeric_limits<uint32_t>::max()) { ptype = PTYPE_UINT32; }
        else if (uspan <= std::numeric_limits<uint64_t>::max()) { ptype = PTYPE_UINT64; }
        return ptype;
    }

    /// Pack data at \p dU and append the result to \p dP possibly
    /// converting its type.
    void pack(const std::vector<T>& dU,
              PackDat& dP) {
        // calculate offsets
        auto ui = begin(dU);
        for (; ui != end(dU); ui++)
        {
            m_umin = std::min<uint64_t>(m_umin, *ui); // update minimum
            m_umax = std::max<uint64_t>(m_umax, *ui); // update maximum

            if (m_umin != pnw::maxof<T>()) {
                uint64_t uoff = *ui - m_umin; // calculate offset
                m_uors |= uoff;	// update bitwise offset sum
            }
        }

        std::cout << " m_umin:" << m_umin << std::endl;
        std::cout << " m_umax:" << m_umax << std::endl;
        std::cout << " m_uors:" << m_uors << std::endl;

        const uint ushift = get_ushift();
        std::cout << " ushift:" << ushift << std::endl;

        const uint64_t uspan = get_uspan(ushift);
        std::cout << " uspan:" << uspan << std::endl;

        const PackType old_type = dP.type;
        const PackType new_type = uspan_to_PTYPE(uspan);
        const size_t old_size = get_size(dP);
        const size_t new_size = get_size(dP) + dU.size();

        // if needed unpack existing packed data (dP) to new possible precision
        if (old_size) {	// if previous packed data
            if (old_type < new_type) {
                PackDat new_dP = { new_type, {0} };
                switch (old_type) { // switch old type
                case PTYPE_UINT8: {
                    auto pi = dP.d.u8->begin();
                    switch (new_type) { // switch new type
                    case PTYPE_UINT16: {
                        new_dP.d.u16 = new v16(new_size); // allocate new
                        auto ni = new_dP.d.u16->begin();
                        for (; pi != dP.d.u8->end(); pi++, ni++) { *ni = *pi; } // convert data
                        break;
                    }
                    case PTYPE_UINT32: {
                        new_dP.d.u32 = new v32(new_size); // allocate new
                        auto ni = new_dP.d.u32->begin();
                        for (; pi != dP.d.u8->end(); pi++, ni++) { *ni = *pi; } // convert data
                        break;
                    }
                    case PTYPE_UINT64: {
                        new_dP.d.u64 = new v64(new_size); // allocate new
                        auto ni = new_dP.d.u64->begin();
                        for (; pi != dP.d.u8->end(); pi++, ni++) { *ni = *pi; } // convert data
                        break;
                    }
                    default: break;
                    }
                    break;
                }
                case PTYPE_UINT16: {
                    auto pi = dP.d.u16->begin();
                    switch (new_type) { // switch new type
                    case PTYPE_UINT32: {
                        new_dP.d.u32 = new v32(new_size); // allocate new
                        auto ni = new_dP.d.u32->begin();
                        for (; pi != dP.d.u16->end(); pi++, ni++) { *ni = *pi; } // convert data
                        break;
                    }
                    case PTYPE_UINT64: {
                        new_dP.d.u64 = new v64(new_size); // allocate new
                        auto ni = new_dP.d.u64->begin();
                        for (; pi != dP.d.u16->end(); pi++, ni++) { *ni = *pi; } // convert data
                        break;
                    }
                    default: break;
                    }
                    break;
                }
                case PTYPE_UINT32: {
                    auto pi = dP.d.u32->begin();
                    switch (new_type) { // switch new type
                    case PTYPE_UINT64: {
                        new_dP.d.u64 = new v64(new_size); // allocate new
                        auto ni = new_dP.d.u64->begin();
                        for (; pi != dP.d.u32->end(); pi++, ni++) { *ni = *pi; } // convert data
                        break;
                    }
                    default: break;
                    }
                    break;
                }
                case PTYPE_UINT64: break;
                default: break;
                }
                clear(dP);	// clear packed data
                dP.type = new_dP.type;
                dP.d.u8 = new_dP.d.u8;
            } else if (old_type == new_type) {
                // reallocate dP.uXX to new_size

            } else if (old_type > new_type) {		// this should not happen
                std::cout << "This should not happen: old_type > new_type" << std::endl;
            }
        } else {		// no previous packed data
        }

        switch (dP.type) {
        case PTYPE_UINT8: {
            dP.d.u8  = new v8(dU.size());
            auto pi = dP.d.u8->begin();
            for (ui = begin(dU); ui != end(dU); ui++, pi++) {
                *pi = (*ui - m_umin) >> ushift; // pack element \c ui to \c pi
            }
            std::cout << dU.size() << " elements packed into uint8's" << std::endl;
            break;
        }
        case PTYPE_UINT16: {
            dP.d.u16 = new v16(dU.size());
            auto pi = dP.d.u16->begin();
            for (ui = begin(dU); ui != end(dU); ui++, pi++) {
                *pi = (*ui - m_umin) >> ushift; // pack element \c ui to \c pi
            }
            std::cout << dU.size() << " elements packed into uint16's" << std::endl;
            break;
        }
        case PTYPE_UINT32: {
            dP.d.u32 = new v32(dU.size());
            auto pi = dP.d.u32->begin();
            for (ui = begin(dU); ui != end(dU); ui++, pi++) {
                *pi = (*ui - m_umin) >> ushift; // pack element \c ui to \c pi
            }
            std::cout << dU.size() << " elements packed into uint32's" << std::endl;
            break;
        }
        case PTYPE_UINT64: {
            dP.d.u64 = new v64(dU.size());
            auto pi = dP.d.u64->begin();
            for (ui = begin(dU); ui != end(dU); ui++, pi++) {
                *pi = (*ui - m_umin) >> ushift; // pack element \c ui to \c pi
            }
            std::cout << dU.size() << " elements packed into uint64's" << std::endl;
            break;
        }
        default:
            break;
        }

        // NOTE: Reuse std::adjacent_difference()
    }

private:
    std::vector<T> *m_dU;	  ///< Unpacked Data.
    uint64_t m_umin;		///< Minimum Value of Unpacked Data.
    uint64_t m_umax;		///< Maximum Value of Unpacked Data.
    uint64_t m_uors;		///< Bitwise or (sum) of Unpacked Offsets.

    PackDat m_dP;		///< Packed data.
};
