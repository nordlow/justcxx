#pragma once

#include <cmath>
#include "interpolate.hpp"

/*! Steered Resampling of Signal \p sO, Accumulated onto \p ss.
 *
 * Cubic Interpolation assumes \em constant signal (zero derivate) extrapolation
 * (in \c y0 and \c y3) at end point samples \c sO[0] and \c sO[lS-1].
 *
 * \tparam Ti Input Original Signal Type.
 * \tparam Ti Output Resampled Signal Type.
 *
 * \param[in] sO Original Input \em Signal to be resampled
 * \param[in] lS \em Length of \p sO
 * \param[in] fN Energy Normalization \em Factor of \p sO, as 1/sqrt(sum(square sO))
 * \param[in] iv Propagation \em Velocity (Inverted) of Medium where \p sO was sampled.
 * \param[in] fs Sample Frequency (Rate) of \p sO
 * \param[in] tA Analysis \em Times
 * \param[in] rE \em Radial Distances from Emitter to Sensor at \p tA, or 0 if Unsteered.
 * \param[in] nA \em Number of \p tA times to analyze.
 * \param[in] ipm Interpolation Flag.
 * \param[out] ss Steered Signal to Add to.
 * \return Index to first undefined (NAN) result.
 */
template<typename Ti, typename To>
inline
size_t steered_resampled_accumulate(const Ti * sO, const size_t lS, const To fN,
                                    const To iv, const To fs,
                                    const To* tA, const To* rE,
                                    const size_t nA,
                                    To* ss,
                                    INTERPOL_t ipm = INTERPOL_DEFAULT)
{
    size_t nD = std::numeric_limits<size_t>::max();
    for (size_t li = 0; li < nA; li++) { // for each block local index
        const To tD = rE ? rE[li] * iv : 0; // Optional Time Delays from Emitter to Sensor
        const To iS = (tA[li] + tD) * fs; // Steered Time Index to Signal \c dO.
        To sI;                            // Interpolated Sample.
        switch (ipm) {
        case INTERPOL_NEAREST:
        {
            const size_t iN = (iS + 0.5); // Nearest neighbouring (NN) integer.
            sI = sO[iN];
            if (iN < lS) {                // If defined
                ss[li] += sI * fN;        // add normalized to superposition
            } else {                      // if undefined
                nD = std::min(nD, li);
                //ss[li] = NAN;
            }
            break;
        }
        case INTERPOL_LINEAR: {
            const size_t iF = iS;      // Floored Index
            if (iF+1 < lS) {
                const To mu = iS - iF;     // Interpolation Intra Sample Offset
                const To y1 = sO[iF];
                const To y2 = sO[iF+1];
                sI = linear_interpolate(y1,y2, mu);
                ss[li] += sI * fN; // add normalized to superposition
            } else {            // in undefined
                nD = std::min(nD, li);
                //ss[li] = NAN;
            }
            break;
        }
        case INTERPOL_CUBIC: {
            const size_t iF = iS;      // Floored Index
            if (iF+1 < lS) {
                const To mu = iS - iF; // Interpolation Intra Sample Offset
                const To y1 = sO[iF];
                const To y2 = sO[iF+1];
                const To y0 = iF-1 >= 0 ? sO[iF-1] : y1;
                const To y3 = iF+2 < lS ? sO[iF+2] : y2;
                sI = cubic_interpolate(y0,y1,y2,y3, mu);
                ss[li] += sI * fN; // add normalized to superposition
            } else {            // in undefined
                nD = std::min(nD, li);
                //ss[li] = NAN;
            }
            break;
        }
        case INTERPOL_CUBIC_CATMULL_ROM: {
            const size_t iF = iS;      // Floored Index
            if (iF+1 < lS) {
                const To mu = iS - iF; // Interpolation Intra Sample Offset
                const To y1 = sO[iF];
                const To y2 = sO[iF+1];
                const To y0 = iF-1 >= 0 ? sO[iF-1] : y1;
                const To y3 = iF+2 < lS ? sO[iF+2] : y2;
                sI = cubic_catmull_rom_interpolate(y0,y1,y2,y3, mu);
                ss[li] += sI * fN; // add normalized to superposition
            } else {            // in undefined
                nD = std::min(nD, li);
                //ss[li] = NAN;
            }
            break;
        }
        case INTERPOL_CUBIC_HERMITE: {
            const size_t iF = iS;      // Floored Index
            if (iF+1 < lS) {
                const To mu = iS - iF; // Interpolation Intra Sample Offset
                const To y1 = sO[iF];
                const To y2 = sO[iF+1];
                const To y0 = iF-1 >= 0 ? sO[iF-1] : y1;
                const To y3 = iF+2 < lS ? sO[iF+2] : y2;
                sI = cubic_hermite_interpolate(y0,y1,y2,y3, mu);
                ss[li] += sI * fN; // add normalized to superposition
            } else {            // in undefined
                nD = std::min(nD, li);
                //ss[li] = NAN;
            }
            break;
        }
        }
    }
    return nD;
}

/*! Resample \p b to the \em nearest level in a uniform \p n-level grid defined
 * by [\p min \p max].
 */
template<typename T>
inline pure
uint uniform_resample_idx(const T & a,
                          const T & min, const T & max,
                          const uint n)
{
    if (n < 2) {
        return 0;
    }
    auto span = max - min;
    if (span < 0) {
        return 0;
    }
    auto step = span / (n-1);
    uint i;
    if (a <= min) {
        i = 0;                  // saturate to min (index 0)
    } else if (a >= max) {
        i = n-1;                // saturate to max (index n-1)
    } else {
        i = static_cast<uint>(rint((a - min) / step)); // index
    }
    return i;
}
