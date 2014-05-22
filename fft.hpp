/*!
 * \file fft.hpp
 * \brief Fast Fourier Transforms.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

// dft

template<typename T>
void dft_and_idft(T *realO, T *imagO, T *realI, T *imagI, int n, int wsign);

template<typename T>
void dft(T *realO, T *imagO, T *realI, T *imagI, int n);

template<typename T>
void idft(T *realO, T *imagO, T *realI, T *imagI, int n);

// 1-point

template<typename T>
void fft1(T *realO, T *imagO, T *realI, T *imagI);

template<typename T>
void ifft1(T *realO, T *imagO, T *realI, T *imagI);

// 2-point

template<typename T>
void fft2(T *realO, T *imagO, T *realI, T *imagI);

template<typename T>
void ifft2(T *realO, T *imagO, T *realI, T *imagI);

// 3-point

template<typename T>
void fft3(T *realO, T *imagO, T *realI, T *imagI);

template<typename T>
void ifft3(T *realO, T *imagO, T *realI, T *imagI);

// 4-point

template<typename T>
void fft4(T *realO, T *imagO, T *realI, T *imagI);

template<typename T>
void ifft4(T *realO, T *imagO, T *realI, T *imagI);

// 5-point

template<typename T>
void fft5(T *realO, T *imagO, T *realI, T *imagI);

template<typename T>
void ifft5(T *realO, T *imagO, T *realI, T *imagI);

// 8-point

template<typename T>
void fft8(T *realO, T *imagO, T *realI, T *imagI);

template<typename T>
void ifft8(T *realO, T *imagO, T *realI, T *imagI);

// n-point

template<typename T>
void fft_and_ifft(T *realO, T *imagO, T *realI, T *imagI, int n, int wsign);

template<typename T>
void fft(T *realO, T *imagO, T *realI, T *imagI, int n);

template<typename T>
void ifft(T *realO, T *imagO, T *realI, T *imagI, int n);

template<typename T>
void realpairfft(T *out0_r, T *out0_i,
                 T *out1_r, T *out1_i,
                 T *in0_r, T *in1_r,
                 int n);

template<typename T>
void hermitianpairifft(T *out0_r, T *out1_r,
                       T *in0_r, T *in0_i,
                       T *in1_r, T *in1_i,
                       int n);

template<typename T>
void hermitianpairidft(T *out0_r, T *out1_r,
                       T *in0_r, T *in0_i,
                       T *in1_r, T *in1_i,
                       int n);

template<typename T>
void realfft(T *out0_r, T *out0_i, T *in0_r, int n);

template<typename T>
void realifft(T *out0_r, T *out0_i, T *in0_r, int n);

template<typename T>
void fft2d(T **realO, T **imagO, T **realI, T **imagI,
           int dim0, int dim1, int is_inverse);

template<typename T>
void realfft2d(T **realO, T **imagO, T **realI,
               int dim0, int dim1, int is_inverse);

void fft_test(int n, int show);

// ============================================================================

template<typename T>
void dft_and_idft(T *realO, T *imagO, T *realI, T *imagI, size_t n,
                  int wsign)
{
    const T e = wsign * M_2PI / n;
    T w_r [n], w_i [n];

    // initialize table of frequency factors
    for (size_t i = 0; i < n; i++)
    {
        w_r[i] = std::cos(e * i); w_i[i] = std::sin(e * i);
    }

    // transform
    for (size_t i = 0; i < n; i++)
    {
        // execute n times
        T sum_r = realI[0], sum_i = imagI[0]; // init accumulation
        for (size_t j = 1; j < n; j++)
        {
            // execute n * n times
            T re = realI[j], im = imagI[j];
            T cs = w_r[i * j % n], sn = w_i[i * j % n];
            sum_r += +re*cs -im*sn; sum_i += +re*sn +im*cs;
        }
        realO[i] = sum_r;
        imagO[i] = sum_i;
    }

    if (wsign == 1)             // if invers
    {
        // normalize inverse transform
        for (size_t i = 0; i < n; i++)
        {
            realO[i] /= n;
            imagO[i] /= n;
        }
    }
}

// DFT
template<typename T>
void dft(T *realO, T *imagO, T *realI, T *imagI, size_t n)
{
    dft_and_idft(realO, imagO, realI, imagI, n, -1);
}

template<typename T>
void idft(T *realO, T *imagO, T *realI, T *imagI, size_t n)
{
    dft_and_idft(realO, imagO, realI, imagI, n, +1);
}

/// 1-point FFT and IFFT
template<typename T>
void fft1(T *realO, T *imagO, T *realI, T *imagI)
{
    realO[0] = realI[0]; imagO[0] = imagI[0];
}

template<typename T>
void ifft1(T *realO, T *imagO, T *realI, T *imagI)
{
    realO[0] = realI[0]; imagO[0] = imagI[0];
}

/// 2-point FFT and IFFT
template<typename T>
void fft2(T *realO, T *imagO, T *realI, T *imagI)
{
    T r0 = realI[0], r1 = realI[1];
    T i0 = imagI[0], i1 = imagI[1];

    realO[0] = (r0 + r1) / 1; imagO[0] = (i0 + i1) / 1;
    realO[1] = (r0 - r1) / 1; imagO[1] = (i0 - i1) / 1;
}

template<typename T>
void ifft2(T *realO, T *imagO, T *realI, T *imagI)
{
    T r0 = realI[0], r1 = realI[1];
    T i0 = imagI[0], i1 = imagI[1];

    realO[0] = (r0 + r1) / 2; imagO[0] = (i0 + i1) / 2;
    realO[1] = (r0 - r1) / 2; imagO[1] = (i0 - i1) / 2;
}

/// 3-point FFT and IFFT
template<typename T>
void fft3(T *realO, T *imagO, T *realI, T *imagI)
{
    T r0 = realI[0], r1 = realI[1], r2 = realI[2];
    T i0 = imagI[0], i1 = imagI[1], i2 = imagI[2];

    T r_mean = (r1 + r2) / 2, r_diff = (r1 - r2) * M_SQRT3_2;
    T i_mean = (i1 + i2) / 2, i_diff = (i1 - i2) * M_SQRT3_2;

    realO[0] = r0 + r1 + r2;             imagO[0] = i0 + i1 + i2;
    realO[1] = r0 - (r_mean) + (i_diff); imagO[1] = i0 - (r_diff) - (i_mean);
    realO[2] = r0 - (r_mean) - (i_diff); imagO[2] = i0 + (r_diff) - (i_mean);
}

template<typename T>
void ifft3(T *realO, T *imagO, T *realI, T *imagI)
{
    T r0 = realI[0], r1 = realI[1], r2 = realI[2];
    T i0 = imagI[0], i1 = imagI[1], i2 = imagI[2];

    T r_mean = (r1 + r2) / 2, r_diff = (r1 - r2) * M_SQRT3_2;
    T i_mean = (i1 + i2) / 2, i_diff = (i1 - i2) * M_SQRT3_2;

    realO[0] = (r0 + r1 + r2) / 3;
    imagO[0] = (i0 + i1 + i2) / 3;

    realO[1] = (r0 - (r_mean) - (i_diff)) / 3;
    imagO[1] = (i0 + (r_diff) - (i_mean)) / 3;

    realO[2] = (r0 - (r_mean) + (i_diff)) / 3;
    imagO[2] = (i0 - (r_diff) - (i_mean)) / 3;
}

/// 4-point FFT and IFFT
template<typename T>
void fft4(T *realO, T *imagO, T *realI, T *imagI)
{
    T r0 = realI[0], r1 = realI[1], r2 = realI[2], r3 = realI[3];
    T i0 = imagI[0], i1 = imagI[1], i2 = imagI[2], i3 = imagI[3];

    T r0_p_r2 = r0 + r2, i0_p_i2 = i0 + i2;
    T r1_p_r3 = r1 + r3, i1_p_i3 = i1 + i3;
    T r0_m_r2 = r0 - r2, i0_m_i2 = i0 - i2;
    T r1_m_r3 = r1 - r3, i1_m_i3 = i1 - i3;

    realO[0] = r0_p_r2 + r1_p_r3; imagO[0] = i0_p_i2 + i1_p_i3;
    realO[1] = r0_m_r2 + i1_m_i3; imagO[1] = i0_m_i2 - r1_m_r3;
    realO[2] = r0_p_r2 - r1_p_r3; imagO[2] = i0_p_i2 - i1_p_i3;
    realO[3] = r0_m_r2 - i1_m_i3; imagO[3] = i0_m_i2 + r1_m_r3;
}

template<typename T>
void ifft4(T *realO, T *imagO, T *realI, T *imagI)
{
    T r0 = realI[0], r1 = realI[1], r2 = realI[2], r3 = realI[3];
    T i0 = imagI[0], i1 = imagI[1], i2 = imagI[2], i3 = imagI[3];

    T r0_p_r2 = r0 + r2, i0_p_i2 = i0 + i2;
    T r1_p_r3 = r1 + r3, i1_p_i3 = i1 + i3;
    T r0_m_r2 = r0 - r2, i0_m_i2 = i0 - i2;
    T r1_m_r3 = r1 - r3, i1_m_i3 = i1 - i3;

    realO[0] = (r0_p_r2 + r1_p_r3) / 4; imagO[0] = (i0_p_i2 + i1_p_i3) / 4;
    realO[1] = (r0_m_r2 - i1_m_i3) / 4; imagO[1] = (i0_m_i2 + r1_m_r3) / 4;
    realO[2] = (r0_p_r2 - r1_p_r3) / 4; imagO[2] = (i0_p_i2 - i1_p_i3) / 4;
    realO[3] = (r0_m_r2 + i1_m_i3) / 4; imagO[3] = (i0_m_i2 - r1_m_r3) / 4;
}

/// 5-point FFT and IFFT
template<typename T>
void fft5(T *realO, T *imagO, T *realI, T *imagI)
{
    T r0=realI[0], r1=realI[1], r2=realI[2], r3=realI[3], r4=realI[4];
    T i0=imagI[0], i1=imagI[1], i2=imagI[2], i3=imagI[3], i4=imagI[4];

    const T w = - M_2PI_5;

    T a = std::cos(1 * w), b = std::sin(1 * w);
    T c = std::cos(2 * w), d = std::sin(2 * w);

    T ar1 = a * r1, br1 = b * r1, cr1 = c * r1, dr1 = d * r1;
    T ai1 = a * i1, bi1 = b * i1, ci1 = c * i1, di1 = d * i1;

    T ar2 = a * r2, br2 = b * r2, cr2 = c * r2, dr2 = d * r2;
    T ai2 = a * i2, bi2 = b * i2, ci2 = c * i2, di2 = d * i2;

    T ar3 = a * r3, br3 = b * r3, cr3 = c * r3, dr3 = d * r3;
    T ai3 = a * i3, bi3 = b * i3, ci3 = c * i3, di3 = d * i3;

    T ar4 = a * r4, br4 = b * r4, cr4 = c * r4, dr4 = d * r4;
    T ai4 = a * i4, bi4 = b * i4, ci4 = c * i4, di4 = d * i4;

    realO[0] = r0 +    +  r1 +     +  r2 +     +  r3 +     +  r4      ;
    imagO[0] =      i0 +     +  i1 +     +  i2 +     +  i3 +     +  i4;

    realO[1] = r0 +    + ar1 + bi1 + cr2 + di2 + cr3 - di3 + ar4 - bi4;
    imagO[1] =      i0 - br1 + ai1 - dr2 + ci2 + dr3 + ci3 + br4 + ai4;

    realO[2] = r0 +    + cr1 + di1 + ar2 - bi2 + ar3 + bi3 + cr4 - di4;
    imagO[2] =    + i0 - dr1 + ci1 + br2 + ai2 - br3 + ai3 + dr4 + ci4;

    realO[3] = r0 +    + cr1 - di1 + ar2 + bi2 + ar3 - bi3 + cr4 + di4;
    imagO[3] =    + i0 + dr1 + ci1 - br2 + ai2 + br3 + ai3 - dr4 + ci4;

    realO[4] = r0 +    + ar1 - bi1 + cr2 - di2 + cr3 + di3 + ar4 + bi4;
    imagO[4] =    + i0 + br1 + ai1 + dr2 + ci2 - dr3 + ci3 - br4 + ai4;
}

template<typename T>
void ifft5(T *realO, T *imagO, T *realI, T *imagI)
{
    T r0 = realI[0], r1 = realI[1], r2 = realI[2], r3 = realI[3], r4 = realI[4];
    T i0 = imagI[0], i1 = imagI[1], i2 = imagI[2], i3 = imagI[3], i4 = imagI[4];

    const T w = + M_2PI_5;

    T a = std::cos(1 * w), b = std::sin(1 * w);
    T c = std::cos(2 * w), d = std::sin(2 * w);

    T ar1 = a * r1, br1 = b * r1, cr1 = c * r1, dr1 = d * r1;
    T ai1 = a * i1, bi1 = b * i1, ci1 = c * i1, di1 = d * i1;

    T ar2 = a * r2, br2 = b * r2, cr2 = c * r2, dr2 = d * r2;
    T ai2 = a * i2, bi2 = b * i2, ci2 = c * i2, di2 = d * i2;

    T ar3 = a * r3, br3 = b * r3, cr3 = c * r3, dr3 = d * r3;
    T ai3 = a * i3, bi3 = b * i3, ci3 = c * i3, di3 = d * i3;

    T ar4 = a * r4, br4 = b * r4, cr4 = c * r4, dr4 = d * r4;
    T ai4 = a * i4, bi4 = b * i4, ci4 = c * i4, di4 = d * i4;

    realO[0] = (r0 +    +  r1 +     +  r2 +     +  r3 +     +  r4      ) / 5;
    imagO[0] = (     i0 +     +  i1 +     +  i2 +     +  i3 +     +  i4) / 5;

    realO[1] = (r0 +    + ar1 + bi1 + cr2 + di2 + cr3 - di3 + ar4 - bi4) / 5;
    imagO[1] = (     i0 - br1 + ai1 - dr2 + ci2 + dr3 + ci3 + br4 + ai4) / 5;

    realO[2] = (r0 +    + cr1 + di1 + ar2 - bi2 + ar3 + bi3 + cr4 - di4) / 5;
    imagO[2] = (   + i0 - dr1 + ci1 + br2 + ai2 - br3 + ai3 + dr4 + ci4) / 5;

    realO[3] = (r0 +    + cr1 - di1 + ar2 + bi2 + ar3 - bi3 + cr4 + di4) / 5;
    imagO[3] = (   + i0 + dr1 + ci1 - br2 + ai2 + br3 + ai3 - dr4 + ci4) / 5;

    realO[4] = (r0 +    + ar1 - bi1 + cr2 - di2 + cr3 + di3 + ar4 + bi4) / 5;
    imagO[4] = (   + i0 + br1 + ai1 + dr2 + ci2 - dr3 + ci3 - br4 + ai4) / 5;
}

/// 8-point FFT and IFFT
template<typename T>
void fft8(T *realO, T *imagO, T *realI, T *imagI)
{
    T ar0, ar1, ar2, ar3, ar4, ar5, ar6, ar7;
    T ai0, ai1, ai2, ai3, ai4, ai5, ai6, ai7;

    T br0, br1, br2, br3, br4, br5, br6, br7;
    T bi0, bi1, bi2, bi3, bi4, bi5, bi6, bi7;

    T cadd5, csub5, cadd7, csub7;

    const T c = M_SQRT2_2;

    // 1:st step - bit-reverse permutatation

    ar0 = realI[0]; ai0 = imagI[0];
    ar1 = realI[4]; ai1 = imagI[4];
    ar2 = realI[2]; ai2 = imagI[2];
    ar3 = realI[6]; ai3 = imagI[6];

    ar4 = realI[1]; ai4 = imagI[1];
    ar5 = realI[5]; ai5 = imagI[5];
    ar6 = realI[3]; ai6 = imagI[3];
    ar7 = realI[7]; ai7 = imagI[7];

    // 2:nd step

    br0 = ar0 + ar1; bi0 = ai0 + ai1;
    br1 = ar0 - ar1; bi1 = ai0 - ai1;
    br2 = ar2 + ar3; bi2 = ai2 + ai3;
    br3 = ar2 - ar3; bi3 = ai2 - ai3;

    br4 = ar4 + ar5; bi4 = ai4 + ai5;
    br5 = ar4 - ar5; bi5 = ai4 - ai5;
    br6 = ar6 + ar7; bi6 = ai6 + ai7;
    br7 = ar6 - ar7; bi7 = ai6 - ai7;

    // 3:rd step

    ar0 = br0 + br2; ai0 = bi0 + bi2;
    ar1 = br1 + bi3; ai1 = bi1 - br3;
    ar2 = br0 - br2; ai2 = bi0 - bi2;
    ar3 = br1 - bi3; ai3 = bi1 + br3;

    ar4 = br4 + br6; ai4 = bi4 + bi6;
    ar5 = br5 + bi7; ai5 = bi5 - br7;
    ar6 = br4 - br6; ai6 = bi4 - bi6;
    ar7 = br5 - br7; ai7 = bi5 + br7;

    // 4:th step

    cadd5 = c*(ar5 + ai5);
    csub5 = c*(ar5 - ai5);
    cadd7 = c*(ar7 + ai7);
    csub7 = c*(ar7 - ai7);

    realO[0] = (ar0 + ar4);   imagO[0] = (ai0 + ai4);
    realO[1] = (ar1 + cadd5); imagO[1] = (ai1 - csub5);
    realO[2] = (ar2 + ai6);   imagO[2] = (ai2 - ar6);
    realO[3] = (ar3 - csub7); imagO[3] = (ai3 - cadd7);

    realO[4] = (ar0 - ar4);   imagO[4] = (ai0 - ai4);
    realO[5] = (ar1 - cadd5); imagO[5] = (ai1 + csub5);
    realO[6] = (ar2 - ai6);   imagO[6] = (ai2 + ar6);
    realO[7] = (ar3 + csub7); imagO[7] = (ai3 + cadd7);
}

template<typename T>
void ifft8(T *realO, T *imagO, T *realI, T *imagI)
{
    T ar0, ar1, ar2, ar3, ar4, ar5, ar6, ar7;
    T ai0, ai1, ai2, ai3, ai4, ai5, ai6, ai7;

    T br0, br1, br2, br3, br4, br5, br6, br7;
    T bi0, bi1, bi2, bi3, bi4, bi5, bi6, bi7;

    T cadd5, csub5, cadd7, csub7;

    const T c = M_SQRT2_2;

    // 1:st step - bit-reverse permutatation

    ar0 = realI[0]; ai0 = imagI[0];
    ar1 = realI[4]; ai1 = imagI[4];
    ar2 = realI[2]; ai2 = imagI[2];
    ar3 = realI[6]; ai3 = imagI[6];

    ar4 = realI[1]; ai4 = imagI[1];
    ar5 = realI[5]; ai5 = imagI[5];
    ar6 = realI[3]; ai6 = imagI[3];
    ar7 = realI[7]; ai7 = imagI[7];

    // 2:nd step

    br0 = ar0 + ar1; bi0 = ai0 + ai1;
    br1 = ar0 - ar1; bi1 = ai0 - ai1;
    br2 = ar2 + ar3; bi2 = ai2 + ai3;
    br3 = ar2 - ar3; bi3 = ai2 - ai3;

    br4 = ar4 + ar5; bi4 = ai4 + ai5;
    br5 = ar4 - ar5; bi5 = ai4 - ai5;
    br6 = ar6 + ar7; bi6 = ai6 + ai7;
    br7 = ar6 - ar7; bi7 = ai6 - ai7;

    // 3:rd step

    ar0 = br0 + br2; ai0 = bi0 + bi2;
    ar1 = br1 - bi3; ai1 = bi1 + br3;
    ar2 = br0 - br2; ai2 = bi0 - bi2;
    ar3 = br1 + bi3; ai3 = bi1 - br3;

    ar4 = br4 + br6; ai4 = bi4 + bi6;
    ar5 = br5 - bi7; ai5 = bi5 + br7;
    ar6 = br4 - br6; ai6 = bi4 - bi6;
    ar7 = br5 + bi7; ai7 = bi5 - br7;

    // 4:th step

    cadd5 = c*(ar5 + ai5);
    csub5 = c*(ar5 - ai5);
    cadd7 = c*(ar7 + ai7);
    csub7 = c*(ar7 - ai7);

    realO[0] = (ar0 + ar4) / 8;   imagO[0] = (ai0 + ai4) / 8;
    realO[1] = (ar1 + csub5) / 8; imagO[1] = (ai1 + cadd5) / 8;
    realO[2] = (ar2 - ai6) / 8;   imagO[2] = (ai2 + ar6) / 8;
    realO[3] = (ar3 - cadd7) / 8; imagO[3] = (ai3 + csub7) / 8;

    realO[4] = (ar0 - ar4) / 8;   imagO[4] = (ai0 - ai4) / 8;
    realO[5] = (ar1 - csub5) / 8; imagO[5] = (ai1 - cadd5) / 8;
    realO[6] = (ar2 + ai6) / 8;   imagO[6] = (ai2 - ar6) / 8;
    realO[7] = (ar3 + cadd7) / 8; imagO[7] = (ai3 - csub7) / 8;
}

/// n-point FFT and IFFT
/// \todo Split into two functions FFT, and IFFT
/// @cite{crecipes} 12.2 p. 504
template<typename T>
void fft_and_ifft(T *realO, T *imagO, T *realI, T *imagI, size_t n,
                  int wsign)
{
    int nbits;
    T theta, wtmp;
    T w_r, w_i, wp_r, wp_i, tmp_r, tmp_i;
    T w = wsign * M_2PI; // angle frequency

    nbits = int_bits_needed(n);

    // \todo ABStract to a separate function like
    // void b64v_reorder(b64 *out, b64 *in, size_t n)

    if (realO != realI)
    {
        // reorder(out, in)
        for (size_t i = 0; i < n; i++)
        {
            size_t j = int_revnbits(i, nbits);
#if 0
            j = ((i & 1) << 2) |
                (i >> 1);
#endif
            realO[j] = realI[i];
            imagO[j] = imagI[i];
        }
    }
    else
    {
        // reorder(out, out)
        for (size_t i = 0; i < n; i++)
        {
            // i and j are equal in 2^{\ceil{length/2}} of the cases
            size_t j = int_revnbits(i, nbits);
            if (i < j)
            {
                // prevent extra swap
                SWAP2(realO[j], realO[i]);
                SWAP2(imagO[j], imagO[i]);
            }
        }
    }

    // the FFT
    // outer loop executed log_2(n) times
    size_t stride = 1;
    while (stride < n)
    {
#if 0
        if ((n / stride) % 4 == 0)
        {
            for (size_t i = 0; i < n; i += stride * 4)
            {
//                  rowPrint(stdout, realO + i, 4, "realO");
//                  rowPrint(stdout, imagO + i, 4, "imagO");
                darray4_FFT(realO + i, imagO + i, realO + i, imagO + i);
//                  rowPrint(stdout, realO + i, 4, "realO");
//                  rowPrint(stdout, imagO + i, 4, "imagO");
            }
            stride <<= 2;
            continue;
        }
#endif
        // trigonometric constants
        theta = w / (2 * stride);
        wtmp = std::sin(0.5 * theta);
        wp_r = -2.0 * std::sqr(wtmp); wp_i = std::sin(theta);
//          printf("stride:%d wp_r:%e wp_i:%e\n", stride, wp_r, wp_i);

        // initialize trigonometric recurrence
        w_r = 1.0; w_i = 0.0;   // exp(i*0)

//          printf("=============================\n",);
        for (size_t m = 0; m < stride; m++)
        {
//              printf("------------\n",);
            for (size_t i = m; i < n; i += stride << 1)
            {
                size_t j = i + stride;
//                  printf("i:%d j:%d\n", i, j);

                // transform out data with a butterfly operation.
                tmp_r = w_r * realO[j] - w_i * imagO[j];
                tmp_i = w_r * imagO[j] + w_i * realO[j];
                realO[j] = realO[i] - tmp_r;
                imagO[j] = imagO[i] - tmp_i;
                realO[i] += tmp_r;
                imagO[i] += tmp_i;
            }

            // trigonometric recurrence
            wtmp = w_r;
            w_r = wtmp * wp_r - w_i * wp_i + w_r;
            w_i = w_i * wp_r + wtmp * wp_i + w_i;
        }

        stride <<= 1;
    }

    if (wsign == 1)
    {
        // normalize inverse transform
        for (size_t i = 0; i < n; i++)
        {
            realO[i] /= n;
            imagO[i] /= n;
        }
    }
}

/// n-point FFT and IFFT

template<typename T>
void fft(T *realO, T *imagO, T *realI, T *imagI, size_t n)
{
    fft_and_ifft(realO, imagO, realI, imagI, n, -1);
}

template<typename T>
void ifft(T *realO, T *imagO, T *realI, T *imagI, size_t n)
{
    fft_and_ifft(realO, imagO, realI, imagI, n, +1);
}

/// n-point FFT and IFFT of pair of two
template<typename T>
void realpairfft(T *out0_r, T *out0_i,
                 T *out1_r, T *out1_i,
                 T *in0_r,
                 T *in1_r,
                 size_t n)
{
    fft_and_ifft(out0_r, out0_i, in0_r, in1_r, n, -1);

    for (size_t i = 0; i <= n / 2; i++)
    {
        size_t j = (n - i) % n;

        T r0 = (+ out0_r[i] + out0_r[j]) / 2;
        T i0 = (+ out0_i[i] - out0_i[j]) / 2;
        T r1 = (+ out0_i[i] + out0_i[j]) / 2;
        T i1 = (- out0_r[i] + out0_r[j]) / 2;

        out0_r[i] = r0; out0_r[j] = r0;
        out0_i[i] = i0; out0_i[j] = -i0;
        out1_r[i] = r1; out1_r[j] = r1;
        out1_i[i] = i1; out1_i[j] = -i1;
    }
}

/// hermitian pair ifft
template<typename T>
void hpifft(T *out0_r,
            T *out1_r,
            T *in0_r, T *in0_i,
            T *in1_r, T *in1_i,
            size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        T r0 = in0_r[i] - in1_i[i];
        T r1 = in0_i[i] + in1_r[i];
        out0_r[i] = r0;
        out1_r[i] = r1;
    }

    fft_and_ifft(out0_r, out1_r, out0_r, out1_r, n, +1);
}

// hermitian pair idft
template<typename T>
void hpidft(T *out0_r,
            T *out1_r,
            T *in0_r, T *in0_i,
            T *in1_r, T *in1_i,
            size_t n)
{
    T *realI = new T[n];
    T *imagI = new T[n];

    for (size_t i = 0; i < n; i++)
    {
        T r0 = in0_r[i] - in1_i[i];
        T r1 = in0_i[i] + in1_r[i];
        realI[i] = r0;
        imagI[i] = r1;
    }

    dft_and_idft(out0_r, out1_r, realI, imagI, n, +1);

    delete [] realI;
    delete [] imagI;
}

template<typename T>
void realfftifft(T *out0_r, T *out0_i,
                 T *in0_r, size_t n,
                 int wsign)
{
    // \todo ...
}

template<typename T>
void realfft(T *out0_r, T *out0_i, T *in0_r, size_t n)
{
    realfftifft(out0_r, out0_i, in0_r, n, -1);
}

template<typename T>
void realifft(T *out0_r, T *out0_i, T *in0_r, size_t n)
{
    realfftifft(out0_r, out0_i, in0_r, n, +1);
}

template<typename T>
void fft2d(T **realO, T **imagO, T **realI, T **imagI,
           int dim0, int dim1, int is_inverse)
{
    // \todo ...
    // reuse FFT
}

template<typename T>
void realfft2d(T **realO, T **imagO, T **realI,
               int dim0, int dim1, int is_inverse)
{
    // \todo ...
    // reuse realPairFFT(when dim0 > 1 or dim1 > 1)
    // reuse FFT(when dim0 or dim1 is odd)
}

// ============================================================================

template<typename T>
void fft_test(size_t n, int show)
{
/* Octave:
   n=8; X=([1:n]+i*[1:n]); tic, Y=IFFT(FFT(X)); toc
   sum(sum(abs(Y-X))) */
    pTimer t; ptimer_init(&t, CLOCK_PROCESS_CPUTIME_ID);
    int i;
    T error;

    printf("%s: n:%d\n", __FUNCTION__, n);

    T *realI = new T[n];
    T *imagI = new T[n];
    T *realO = new T[n];
    T *imagO = new T[n];

    ramp(realI, n, 0.0, 1.0);
    ramp(imagI, n, 0.0, 1.0);
    if (show)
    {
        printComplex(realI, imagI, n);
    }
    puts("");

    if (0)
    {
        ptimer_tic(&t);
        DFT(realO, imagO, realI, imagI, n);
        IDFT(realO, imagO, realO, imagO, n);
        ptimer_toc(&t);
        printf("DFT, IDFT: ");
        siunit_show(ptimer_sec(&t));
        for (error = 0, i = 0; i < n; i++)
            error += fabs(realO[i] - realI[i]) + fabs(imagO[i] - imagI[i]);
        printf("  error: %.5e\n", error);
        if (show)
        {
            printComplex(realO, imagO, n);
        }
        puts("");
    }

    if (n == 3)
    {
        printf("darray3_FFT, IFFT: ");
        ptimer_tic(&t);
        darray3_FFT(realO, imagO, realI, imagI);
        darray3_IFFT(realO, imagO, realO, imagO);
        ptimer_toc(&t);
        siunit_show(ptimer_sec(&t));
        puts("");
        if (show)
        {
            printComplex(realO, imagO, n);
        }
        puts("");
    }

    if (n == 4)
    {
        printf("darray4_FFT, IFFT: ");
        ptimer_tic(&t);
        darray4_FFT(realO, imagO, realI, imagI);
        darray4_IFFT(realO, imagO, realO, imagO);
        ptimer_toc(&t);
        siunit_show(ptimer_sec(&t));
        puts("");
        if (show)
        {
            printComplex(realO, imagO, n);
        }
        puts("");
    }

    if (n == 5)
    {
        printf("darray5_FFT, IFFT: ");
        ptimer_tic(&t);
        darray5_FFT(realO, imagO, realI, imagI);
        darray5_IFFT(realO, imagO, realO, imagO);
        ptimer_toc(&t);
        siunit_show(ptimer_sec(&t));
        puts("");
        if (show)
        {
            printComplex(realO, imagO, n);
        }
        puts("");
    }

    if (n == 8)
    {
        printf("darray8_FFT, IFFT: ");
        ptimer_tic(&t);
        darray8_FFT(realO, imagO, realI, imagI);
        darray8_IFFT(realO, imagO, realO, imagO);
        ptimer_toc(&t);
        siunit_show(ptimer_sec(&t));
        puts("");
        if (show)
        {
            printComplex(realO, imagO, n);
        }
        puts("");
    }


    {
        ptimer_tic(&t);
        FFT(realO, imagO, realI, imagI, n);
        IFFT(realO, imagO, realO, imagO, n);
        ptimer_toc(&t);
        printf("FFT, IFFT: ");
        siunit_show(ptimer_sec(&t));
        for (error = 0, i = 0; i < n; i++)
            error += fabs(realO[i] - realI[i]) + fabs(imagO[i] - imagI[i]);
        printf("  error: %.5e\n", error);
        if (show)
        {
            printComplex(realO, imagO, n);
        }
    }

    puts("");

    {
        ptimer_tic(&t);
        realPairFFT(realO, imagO, realI, imagI, realI, imagI, n);
        hpIFFT(imagI, realI, realO, imagO, realO, imagO, n);
        ptimer_toc(&t);
        printf("realPairFFT, hpIFFT: ");
        siunit_show(ptimer_sec(&t));
        for (error = 0, i = 0; i < n; i++)
            error += fabs(realO[i] - realI[i]) + fabs(imagO[i] - imagI[i]);
        printf("  error: %.5e\n", error);
        if (show)
        {
            printComplex(realI, imagI, n);
            printComplex(realO, imagO, n);
        }
    }

    delete [] realI;
    delete [] imagI;
    delete [] realO;
    delete [] imagO;
}
