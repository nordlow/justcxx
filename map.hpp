/*!
 * \file map.hpp
 * \brief Mappings from World to Screen Coordinates.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * Transformations that map world to view screen positions and dimensions.
 *
 * \p wrp: World Window Upper (Reference) Position.
 * \p srp: Screen Window Upper (Reference) Position.
 */

#pragma once
#include <vector>
#include "box.hpp"

namespace pnw {

/*!
 * Map one position.
 */
template<typename W, typename S>
inline pure
vec2<S> map_pos(const vec2<W> & wrp, const vec2<W> & mapf,
                const vec2<S> & srp,
                const vec2<W> & a)
{
    const vec2<W> & ra = wrp;
    const vec2<S> sa(static_cast<S>((a(0) - ra(0)) * mapf(0)),
                     static_cast<S>((a(1) - ra(1)) * mapf(1)));
    return sa + srp;
}

#define USE_MAP_OPT		///< Use mapping optimizations.

/*!
 * Map one position.
 *
 * Specialized for float to short mappings.
 */
#ifdef USE_MAP_OPT
inline pure
vec2<short> map_pos_dirty(const vec2<float> & wrp,
                          const vec2<float> & mapf,
                          const box2<short> & srp,
                          const vec2<float> & a)
{
//     int yy[2];
//     const float xx[2] = { a(0), a(1) };
//     const float rp[2] = { rect.l(0), rect.l(1) };
//     const float mf[2] = { mapf(0), mapf(1) };
//     pwss2d(yy, xx, rp, mf);
    vec2<short> sa;
    fa2_ss_sc(reinterpret_cast<short*>(&sa),
              reinterpret_cast<const float*>(&a),
              reinterpret_cast<const float*>(&wrp),
              reinterpret_cast<const float*>(&mapf),
              reinterpret_cast<const short*>(&srp));
    return sa;
}
#endif

/*! Map Two Positions. */
template<typename W, typename S>
inline
void map_two_pos(const vec2<W> & wrp, const vec2<W> & mapf,
                 const vec2<S> & srp,
                 vec2<S> & sa, const vec2<W> & a,
                 vec2<S> & sb, const vec2<W> & b)
{
    map_pos_dirty(wrp, mapf, srp, sa, a);
    map_pos_dirty(wrp, mapf, srp, sb, b);
//     MMX_EMMS();
}

/*! Map Three Positions. */
template<typename W, typename S>
inline
void map_three_pos(const vec2<W> & wrp, const vec2<W> & mapf,
                   const vec2<S> & srp,
                   vec2<S> & sa, const vec2<W> & a,
                   vec2<S> & sb, const vec2<W> & b,
                   vec2<S> & sc, const vec2<W> & c)
{
    map_pos_dirty(wrp, mapf, srp, sa, a);
    map_pos_dirty(wrp, mapf, srp, sb, b);
    map_pos_dirty(wrp, mapf, srp, sc, c);
//     MMX_EMMS();
}

/*!
 * Map a vector positions.
 */
template<typename W, typename S>
inline
void map_poss(const vec2<W> & wrp, const vec2<W> & mapf,
              const vec2<S> & srp,
              std::vector<vec2<S> > & sa,
              const std::vector<vec2<W> > & a)
{
    const size_t q = a.size() / 2;
    const size_t r = a.size() % 2;
    size_t i;
    // loop unrolling....
    for (i = 0; i < q * 2; i += 2) {
        map_pos_dirty(wrp, mapf, srp, sa[i + 0], a[i + 0]);
        map_pos_dirty(wrp, mapf, srp, sa[i + 1], a[i + 1]);
    }
    if (r) {
        map_pos_dirty(wrp, mapf, srp, sa[i], a[i]);
    }
//     MMX_EMMS();
}

/*! Map a C-array of n world positions sa to screen position a. */
template<typename W, typename S>
inline
void map_posarray(const vec2<W> & wrp, const vec2<W> & mapf,
                  const vec2<S> & srp,
                  vec2<S> * sa, const vec2<W> * a,
                  size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        map_pos_dirty(wrp, mapf, srp, sa[i], a[i]);
    }
//     MMX_EMMS();
}

/*! Map Rectangle. */
template<typename W, typename S>
inline pure
box2<S> map_rect(const vec2<W> & wrp, const vec2<W> & mapf,
                 const vec2<S> & srp, const box2<W> & rect)
{
    box2<S> srect;
    map_two_pos(wrp, mapf, srp,
                srect.l(), rect.l(),
                srect.l(), rect.u());
    return srect;
}

}

#endif
