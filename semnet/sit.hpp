/*! \file psit.hpp
 * \brief Pattern \em Situation (Context). Can be defined in terms of pattens objects.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * TODO: Add language (syntax) member.
 */

#pragma once
#include "patt.hpp"
#include "HLANG_enum.hpp"
#include <iosfwd>

namespace semnet {
namespace patterns {

/*! \em Relational - \em Position Delimiters.
 * \see \c PCTX_t
 */
typedef enum {
    PRPOS_any_,                 ///< \em Any.

    PRPOS_BOB,                  ///< Beginning Of \em Block/Name/File/String. @b Emacs: "\`"
    PRPOS_EOB,                  ///< End       Of \em Block/Name/File/String. @b Emacs: "\'"

    PRPOS_BOL,                  ///< Beginning Of \em Line. @b Emacs: "^"
    PRPOS_EOL,                  ///< End       Of \em Line. @b Emacs: "$"

    PRPOS_BOE,                  ///< Beginning Of \em Expression.
    PRPOS_EOE,                  ///< End       Of \em Expression.

    PRPOS_BOT,                  ///< Beginning Of \em Statement.
    PRPOS_EOT,                  ///< End       Of \em Statement.

    PRPOS_BOS,                  ///< Beginning Of \em Symbol. @b Emacs: "\_<"
    PRPOS_EOS,                  ///< End       Of \em Symbol. @b Emacs: "\_>"

    PRPOS_BOW,                  ///< Beginning Of \em Word. @b Emacs: "\<"
    PRPOS_EOW,                  ///< End       Of \em Word. @b Emacs: "\>"

    PRPOS_undefined_,         ///< Undefined.
} __attribute__ ((packed)) PRPOS_t;

inline const char *
PRPOS_getName(PRPOS_t rpos)
{
    const char * str = 0;
    switch (rpos) {
    case PRPOS_any_: str = "any"; break;

    case PRPOS_BOB: str = "BoF"; break;
    case PRPOS_EOB: str = "EoF"; break;

    case PRPOS_BOL: str = "BoL"; break;
    case PRPOS_EOL: str = "EoL"; break;

    case PRPOS_BOE: str = "BoE"; break;
    case PRPOS_EOE: str = "EoE"; break;

    case PRPOS_BOT: str = "BoT"; break;
    case PRPOS_EOT: str = "EoT"; break;

    case PRPOS_BOS: str = "BoS"; break;
    case PRPOS_EOS: str = "EoS"; break;

    case PRPOS_BOW: str = "BoW"; break;
    case PRPOS_EOW: str = "EoW"; break;

    case PRPOS_undefined_: str = "Undefined"; break;
    }
    return str;
}

/*! Pattern \em Situation (Context).
 * \see \c PCTX_t
 */
class Sit : public Base {
public:
    /*! Create Situation of type \p rpos. */
    Sit(PRPOS_t rpos = PRPOS_any_, const char * name = nullptr, HLANG_t lang = HLANG_any_)
        : Base(), m_rpos(rpos), m_lang(lang) {
        if (not name) {
            switch (rpos) {
            case PRPOS_BOW: name = "Beginning of Word"; break;
            case PRPOS_EOW: name = "End of Word"; break;
            case PRPOS_BOS: name = "Beginning of Symbol"; break;
            case PRPOS_EOS: name = "End of Symbol"; break;
            case PRPOS_BOL: name = "Beginning of Line"; break;
            case PRPOS_EOL: name = "End of Line"; break;
            case PRPOS_BOB: name = "Beginning of File"; break;
            case PRPOS_EOB: name = "End of File"; break;
            default: break;
            }
        }
        if (name) { set_name(name); }
    }

    virtual bir sample_range() const { return bir::none(); }
    virtual size_t complexity(bir mults = bir::full()) const { return 1; } /* \note: a situation can only be one at a time. */
    virtual csc rand(bir ssr = bir::full()) const { return csc(); }
    virtual Base::Skips8& intersect_skips(Skips8& skips) const { return skips; }

    PRPOS_t value() const { return m_rpos; }
    virtual std::ostream& show(std::ostream& os) const;

protected:
    virtual pHit match_in(const char* buf, size_t len,
                          bir roi = bir::full()) const;
    virtual pHit match_in_local(const char* buf, size_t len,
                                bir roi = bir::full()) const;

private:
    PRPOS_t m_rpos:4;         ///< Type of Relative \em Position.
    HLANG_t m_lang;           ///< \em Language.
};

namespace gen {
template<class... T> inline Sit* sit(T&&... a) { return new Sit(a...); }

inline Sit * bob() { return sit(PRPOS_BOB); }
inline Sit * eob() { return sit(PRPOS_EOB); }

inline Sit * bol() { return sit(PRPOS_BOL); }
inline Sit * eol() { return sit(PRPOS_EOL); }

inline Sit * boe(HLANG_t lang = HLANG_any_) { return sit(PRPOS_BOE, nullptr, lang); }
inline Sit * eoe(HLANG_t lang = HLANG_any_) { return sit(PRPOS_EOE, nullptr, lang); }

inline Sit * bot(HLANG_t lang = HLANG_any_) { return sit(PRPOS_BOT, nullptr, lang); }
inline Sit * eot(HLANG_t lang = HLANG_any_) { return sit(PRPOS_EOT, nullptr, lang); }

inline Sit * bos(HLANG_t lang = HLANG_any_) { return sit(PRPOS_BOS, nullptr, lang); }
inline Sit * eos(HLANG_t lang = HLANG_any_) { return sit(PRPOS_EOS, nullptr, lang); }

inline Sit * bow(HLANG_t lang = HLANG_any_) { return sit(PRPOS_BOW, nullptr, lang); }
inline Sit * eow(HLANG_t lang = HLANG_any_) { return sit(PRPOS_EOW, nullptr, lang); }
}

}}
