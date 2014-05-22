/*!
 * \file emd.hpp
 * \brief Empirical Mode Decomposition (EMD).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * Required paramters include:
 * - nF: the number of IMFs to return
 * - nI: the number of iterations per IMF
 * - locality: in samples, the nearest two extrema may be
 *
 * If it is not specified, there is no limit (locality = 0).
 *
 * Typical use consists of calling \c emd(), followed by
 * \c decompose(), and then using the struct's "imfs" field
 * to retrieve the data. Call \c emdclear() to deallocate memory
 * inside the struct.
 *
 * \see https://code.google.com/p/realtime-emd/e (Real Shit C Code!)
 * \todo MDL Stopping Criteria
 */

#pragma once
#include "interpolate.hpp"

#define cnew(type, size) ((type*) malloc((size) * sizeof(type)))
#define cdelete(ptr) free(ptr)

/*!
 * Empirical Mode Decomposition (EMD).
 */
template<class T>
class emd {
public:
    void emd(size_t size, size_t nF, int nI, size_t locality) {
        setup(nF, nI, locality);
        resize(size);
    }

    void setup(size_t nF, int nI, size_t locality) {
        m_nI = nI;
        m_nF = nF;
        m_locality = locality;
        m_size = 0;
        m_imfs = nullptr;
        m_residue = nullptr;
        m_minP = nullptr;
        m_maxP = nullptr;
        m_min = nullptr;
        m_max = nullptr;
    }

    void resize(size_t size) {
        size_t i;

        clear(emd);

        m_size = size;
        m_imfs = cnew(T*, m_nF);
        for(i = 0; i < m_nF; i++)
            m_imfs[i] = cnew(T, size);
        m_residue = cnew(T, size);
        m_minP = cnew(size_t, size / 2);
        m_maxP = cnew(size_t, size / 2);
        m_min = cnew(T, size);
        m_max = cnew(T, size);
    }

    void clear() {
        if(m_imfs != nullptr) {
            for(size_t i = 0; i < m_nF; i++)
                cdelete(m_imfs[i]);
            cdelete(m_imfs);
            cdelete(m_minP);
            cdelete(m_maxP);
            cdelete(m_min);
            cdelete(m_max);
            cdelete(m_residue);
        }
    }

    void decompose(const T* signal) {
        memcpy(m_imfs[0], signal, m_size * sizeof(T));
        memcpy(m_residue, signal, m_size * sizeof(T));
        for(size_t i = 0; i < m_nF - 1; i++) {
            T* curImf = m_imfs[i];
            for(size_t j = 0; j < m_nI; j++) {
                make_extremas(curImf);
                if(m_minSize < 4 || m_maxSize < 4)
                    break; // can't fit splines
                interpolate(curImf, m_min, m_minP, m_minSize);
                interpolate(curImf, m_max, m_maxP, m_maxSize);
                update_imf(curImf);
            }
            make_residue(curImf);
            memcpy(m_imfs[i + 1], m_residue, m_size * sizeof(T));
        }
    }

    // Currently, extrema within (locality) of the boundaries are not allowed.
    // A better algorithm might be to collect all the extrema, and then assume
    // that extrema near the boundaries are valid, working toward the center.

    void make_extremas(const T* curImf) {
        size_t i, lastMin = 0, lastMax = 0;
        m_minSize = 0;
        m_maxSize = 0;
        for(i = 1; i < m_size - 1; i++) {
            if(curImf[i - 1] < curImf[i]) {
                if(curImf[i] > curImf[i + 1] && (i - lastMax) > m_locality) {
                    m_maxP[m_maxSize++] = i;
                    lastMax = i;
                }
            } else {
                if(curImf[i] < curImf[i + 1] && (i - lastMin) > m_locality) {
                    m_minP[m_minSize++] = i;
                    lastMin = i;
                }
            }
        }
    }

    // Replace with my interpolate.hpp.
    void interpolate(const T* in, T* out, size_t* points, size_t pointsSize) {
        size_t size = m_size;
        size_t i, j, i0, i1, i2, i3, start, end;
        T a0, a1, a2, a3;
        T y0, y1, y2, y3, muScale, mu;
        for(i = -1; i < pointsSize; i++) {
            i0 = points[mirror_index(i - 1, pointsSize)];
            i1 = points[mirror_index(i, pointsSize)];
            i2 = points[mirror_index(i + 1, pointsSize)];
            i3 = points[mirror_index(i + 2, pointsSize)];

            y0 = in[i0];
            y1 = in[i1];
            y2 = in[i2];
            y3 = in[i3];

            a0 = y3 - y2 - y0 + y1;
            a1 = y0 - y1 - a0;
            a2 = y2 - y0;
            a3 = y1;

            // left boundary
            if(i == -1) {
                start = 0;
                i1 = -i1;
            } else
                start = i1;

            // right boundary
            if(i == pointsSize - 1) {
                end = size;
                i2 = size + size - i2;
            } else
                end = i2;

            muScale = 1.f / (i2 - i1);
            for(j = start; j < end; j++) {
                mu = (j - i1) * muScale;
                out[j] = ((a0 * mu + a1) * mu + a2) * mu + a3;
            }
        }
    }

    void update_imf(T* imf) {
        size_t i;
        for(i = 0; i < m_size; i++)
            imf[i] -= (m_min[i] + m_max[i]) * .5f;
    }

    void make_residue(const T* cur) {
        size_t i;
        for(i = 0; i < m_size; i++)
            m_residue[i] -= cur[i];
    }

    size_t mirror_index(size_t i, int size) {
        if(i < size) {
            if(i < 0)
                return -i - 1;
            return i;
        }
        return (size - 1) + (size - i);
    }
private:
	size_t m_nI;                ///< Number of Iterations.
	size_t m_nF;                ///< Number of Functions.
	size_t m_locality;
	size_t *m_minP;             ///< Minimas.
	size_t *m_maxP;             ///< Maximums.
	T *m_min, *m_max, **m_imfs, *m_residue;
	size_t m_size, m_minSize, m_maxSize;
};
