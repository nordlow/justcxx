/*!
 * \file calculate.hpp
 * \brief
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-03-28 16:37
 *
 * \see http://stackoverflow.com/questions/4764590/can-i-use-a-lambda-function-or-stdfunction-object-in-place-of-a-function-point
 * \see http://stackoverflow.com/a/7828997/683710
 */

#pragma once

template<class FuncType>
auto calculate(FuncType&& func) -> decltype(func())
{
    // precalc();
    auto ret = func();
    // postcalc();
    return ret;
}
