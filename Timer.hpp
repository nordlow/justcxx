#pragma once

#include "timing.h"
#include "Ticks.hpp"
#include "show.hpp"
#include "show_units.hpp"

class Timer
{
public:
    Timer() { restart(); }        // start immediately

    /// Restart Timer.
    void restart() { tic.update(); }

    /// Stop Timer.
    void stop() const { toc.update(); }

    /// Get Elapsed Time from start to current time.
    double get_elapsed() const { return elapsed(toc.get(), tic.get()); }

    friend std::ostream & operator << (std::ostream & os, const Timer & in) { return pnw::siunit_show(in.get_elapsed(), os); }
protected:
    Ticks tic;                  ///< Start time.
    mutable Ticks toc;          ///< Stopp time.
};
