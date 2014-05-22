/*! \file show_ngram.hpp
 * \brief Show n-gram.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-07-02 12:55
 */

#pragma once
#include "show.hpp"
#include "show_histogram.hpp"
#include "ngram.hpp"

template<class V, class C = size_t, class L = uint8_t>
std::ostream& show_each(std::ostream& os,
                        const pnw::histogram::ngram::tree<V,C,L>& a) {
    bool flag2d = false;
    os << "{@" << static_cast<int>(a.level()) << ":";
    if (flag2d) { os << std::endl; } else { os << " "; }
    for (auto& e : a.map().get()) {
        auto& val = e.first;        // value
        auto& cnt = e.second.first; // count
        const auto& sub = e.second.second.get();
        if (flag2d) { os_spaces(os, a.level()); }
        show(os, val); os << SEPCH; show(os, cnt);
        if (sub and not sub->empty()) {
            os << "→";
            show_each(os, *sub);
        }
        if (flag2d) { os << std::endl; } else { os << " "; }
    }
    os << "}";
    if (flag2d) { os << std::endl; }
    return os;
}

template<class V, class C = size_t, class L = uint8_t>
std::ostream& show(std::ostream& os,
                   const pnw::histogram::ngram::tree<V,C,L>& a) {
    show_each(os, a);
    os << " : pnw::histogram::ngram::pnw::histogram::ngram::tree<"
       << "V=" << typenameof<V>() << ","
       << "C=" << typenameof<C>() << ","
       << "L=" << typenameof<L>() << ">";
    return os;
}

template<class V, class C = size_t, class L = uint8_t>
std::ostream& operator << (std::ostream& os, const pnw::histogram::ngram::tree<V,C,L>& a) {
    return show(os, a);
}
