/*! \file particle.hpp
 * \brief Particle with position, velocity and mass.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-08-05 12:11
 */

#pragma once
#include "vec.hpp"

template<class T, std::size_t N>
struct particle {
public:
    typedef vec<T,N> V;         ///< Vector alias
    particle(const V& pos_,
             const T& mass_,
             const V& vel_ = 0)
        : pos(pos_), vel(vel_), mass(mass_) {}
#if NDEBUG
    particle() {}                    ///< Default Construct Uninitialized. */
#else
    particle() : particle(V(pnw::uninitialized<T>()),
                          pnw::uninitialized<T>(),
                          V(pnw::uninitialized<T>())) {} ///< Default Construct Undefined. */
#endif
    V pos;                      ///< Position.
    V vel;                      ///< Velocity.
    T mass;                     ///< Mass.

    friend particle& rand(particle& a) { rand(a.pos); rand(a.vel); rand(a.mass); return a; }

    friend V mass_center(particle& a) { return a.pos; }
    friend V mass_center(particle& a,
                         particle& b) { return (a.pos*a.mass +
                                                b.pos*b.mass) / (a.mass +
                                                                 b.mass); }
    friend V mass_center(particle& a,
                         particle& b,
                         particle& c) { return (a.pos*a.mass +
                                                b.pos*b.mass +
                                                c.pos*c.mass) / (a.mass +
                                                                 b.mass +
                                                                 c.mass); }
    friend V mass_center(particle& a,
                         particle& b,
                         particle& c,
                         particle& d) { return (a.pos*a.mass +
                                                b.pos*b.mass +
                                                c.pos*c.mass +
                                                d.pos*d.mass) / (a.mass +
                                                                 b.mass +
                                                                 c.mass +
                                                                 d.mass); }

    friend std::ostream& show (std::ostream& os, const particle& a) {
        os << "(pos" << MEMCH << a.pos << ", vel" << MEMCH << a.vel << ", mass" << MEMCH << a.mass << ")"; return os;
        return os;
    }
    friend std::ostream& operator << (std::ostream& os, const particle& a) { return show(os, a); }
};
