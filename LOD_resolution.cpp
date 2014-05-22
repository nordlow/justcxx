#include "LOD_resolution.hpp"

/*! Define this to enable Lookup-Table (LUT) Optimizations for binlog. */
#define LOD_BINLOG_LUT_OPT (1)

static float g_srad_max_scale = 0.5f;

pure uint calcCirLODres(const vec2f& mapfW2S,
                                         const float curve,
                                         bool LOD_flag)
{
#ifndef NDEBUG
    if (curve < 0) { PWARN("negative curve\n"); }
#endif
    uint bits = TESS_Q_BITS; /* default value */
    if (LOD_flag) {
        float srad_max = std::max(curve * mapfW2S(0),
                                  curve * mapfW2S(1));
        uint8_t srad_max_ui8 = std::min(srad_max * g_srad_max_scale, 255);
#ifdef LOD_BINLOG_LUT_OPT
        bits = std::min<uint>(int8_binlog_LUT(srad_max_ui8), TESS_Q_BITS);
#else
        bits = std::min<uint>(int_binlog(srad_max_ui8), TESS_Q_BITS);
#endif
    }
    return bits;
}

pure uint calcEllLODres(const vec2f& mapfW2S,
                                         const vec2f& curve,
                                         bool LOD_flag)
{
#ifndef NDEBUG
    if (curve(0) < 0 || curve(1) < 0) { PWARN("negative curve\n"); }
#endif
    uint bits = TESS_Q_BITS; /* default value */
    if (LOD_flag) {
        float srad_max = std::max(curve(0) * mapfW2S(0),
                                  curve(1) * mapfW2S(1));
        uint8_t srad_max_ui8 = std::min(srad_max * g_srad_max_scale, 255);
#ifdef LOD_BINLOG_LUT_OPT
        bits = std::min<uint>(int8_binlog_LUT(srad_max_ui8), TESS_Q_BITS);
#else
        bits = std::min<uint>(int_binlog(srad_max_ui8), TESS_Q_BITS);
#endif
        /*     if (LOD_flag >= 10) { */
        /*       printf("srad_max:%f\n", srad_max); */
        /*       printf("srad_max_ui8:%d\n", srad_max_ui8); */
        /*       printf("bits:%d\n", bits); */
        /*     } */
    }
    return bits;
}

pure uint vec2f_getMaxLODres(void)
{
    uint bits = TESS_Q_BITS; /* default value */
    return bits;
}

pure uint calcEllLODres2(const vec2f& mapfW2S,
                                          const vec2f& curveI,
                                          const vec2f& curveO,
                                          bool LOD_flag)
{
#ifndef NDEBUG
    if (curveI(0) < 0 || curveI(1) < 0) { PWARN("negative curveI\n"); }
    if (curveO(0) < 0 || curveO(1) < 0) { PWARN("negative curveO\n"); }
#endif
    uint bits = TESS_Q_BITS; /* default value */
    if (LOD_flag) {
        float srad_max = std::max(std::max(curveI(0), curveO(0)) * mapfW2S(0),
                                  std::max(curveI(1), curveO(1)) * mapfW2S(1));
        uint8_t srad_max_ui8 = std::min(srad_max * g_srad_max_scale, 255);
#ifdef LOD_BINLOG_LUT_OPT
        bits = std::min<uint>(int8_binlog_LUT(srad_max_ui8), TESS_Q_BITS);
#else
        bits = std::min<uint>(int_binlog(srad_max_ui8), TESS_Q_BITS);
#endif
    }
    return bits;
}
