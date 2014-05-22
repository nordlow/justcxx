#pragma once

#include "utils.h"
#include "cycle.h"

class Ticks
{
public:
    Ticks() { update(); }
    void update() { m_ticks = getticks(); }
    ticks get() const { return m_ticks; }
protected:
    ticks m_ticks;
};
