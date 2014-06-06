/*!
 * \file comb.hpp
 * \brief Pattern Combination (Logical).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

namespace semnet {
namespace patterns {

class Base;

/*! Pattern \em (And) Combination. */
class Comb
{
public:
    /*! \em Logical Combination Operation. */
    typedef enum {
        OR,                     ///< Logical Or.
        AND,                    ///< Logical And.
    } __attribute__ ((packed)) Op;

    typedef std::vector<Base*> Args; ///< Combination Argument Patterns.

    Comb(Base * arg) { add(arg); }
    Comb(Base ** args, size_t argsN) { add(args, argsN); }
    template<class... Tp>
    Comb(Tp*... args) { add(args...); }

    virtual ~Comb() { clear(); }

    Comb * set_name(const char * name) { m_nameP.assign(name); return this; }

    virtual void add(Base * arg) {
        m_args.push_back(arg);  // downlink
    }
    virtual void add(Base ** args, size_t n) {
        m_args.insert(m_args.end(), args, args+n); // downlinks
    }

    /*! Add Arg-Patterns \p pA. */
    template<class... Tp>
    void add(Tp*... args) { Base* parr[sizeof...(args)] = { args... }; add(parr, sizeof...(args)); } // C++11

    virtual void clear() { m_args.clear(); }
    virtual size_t size() const { return m_args.size(); }
    virtual bool empty() const { return m_args.empty(); }

private:
    csc m_nameP;                ///< \em Name if any.
    mutable Args m_args;        ///< Combination \em Arg-Patterns.
    Op m_op;
};

}
}
