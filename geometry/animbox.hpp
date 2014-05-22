/*! \file animbox.hpp
 * \brief Animated Rectangular Box.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include "box.hpp"

typedef enum {
    TRANSITION_LINEAR,
    TRANSITION_HARMONIC
} Transition;

template<class T>
inline T animate(Transition t, const T & x)
{
    T y;
    switch (t) {
    case TRANSITION_LINEAR:
        y = x;
        break;
    case TRANSITION_HARMONIC:
        y = (1 - std::cos(x * M_PI)) / 2;
        break;
    default:
        y = x;			// default to linear
        break;
    }
    return y;
}

/*! Animated Box.
 * Animation when changing the state of a box.
 */
template<class T, size_t N>
class animbox
{
public:
    /// Normally we start with an unanimated box s.
    animbox(const box<T,N> & s, unsigned n)
        : source(s),
          dest(s),
          nsteps(n),
          step(n),		// Default to stopped.
          trans(TRANSITION_HARMONIC)
    {}

    box<T,N> get_state() const {
        T s = animate(TRANSITION_HARMONIC, static_cast<T>(step) / (nsteps ? nsteps : 1));
        return source + (dest - source) * s; // reuse linear_interpolate?
    }

    void restart(const box<T,N>& s,
                 const box<T,N>& d) {
        source = s;
        dest = d;
        step = 0;
    }

    box<T,N> get_dest() const { return dest; }
    uint     get_step() const { return step; }

    void turn_to(const box<T,N>& d) {
        source = get_state();	// start from here
        dest = d;
        step = 0;
    }

    bool is_active() const { return step < nsteps; }
    void next_frame() { if (is_active()) { step++; } }
private:
    box<T,N>    source, dest;    ///< Source and destination box of animation.
    uint       nsteps;          ///< Number of steps in animation.
    uint       step;		///< The current step.
    Transition trans:8;		///< Type of transition.
};
