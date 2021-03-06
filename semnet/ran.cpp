#include "ran.hpp"
#include "lit.hpp"
#include "any.hpp"
#include "pmatchers.hpp"

namespace semnet { namespace patterns {

#if 0
size_t
Ran::complexity(bir mults) const
{
    bix multV = 0;          /* initialize number of possibly patterns */
    for (auto it : m_subs) { /* for each whole range pair */
        const csc bufI0 = dynamic_cast<Base*>(hits[2*i+0])->rand(begin(mults), end(mults)); // sub pattern
        const csc bufI1 = dynamic_cast<Base*>(hits[2*i+1])->rand(begin(mults), end(mults)); // sub pattern
        const size_t lenI0 = bufI0.size();
        const size_t lenI1 = bufI1.size();
        if (lenI0 == lenI1) {
            switch (lenI0) {
            case 1:
                multV += bufI1[0] - bufI0[0] + 1; /* calculate inclusive span */
                ret = 1;
                break;
            default:
                std::cerr << "Cannot handle case for ret == " << lenI0 << std::endl;
                break;
            }
        } else { PERR("Cannot handle case for lenI0 != lenI1\n"); }
    }
    if (hits.size() % 2 == 1) {
        const size_t i = hits.size() - 1;
        multV = sadd(multV, sub->complexity(mults));
    }
    return multV;
}

csc
Ran::rand(bir ssr) const
{
    char bufI[maxI];
    size_t sizeI = 0;
    if (m_subs.size() == 2) {
        const csc bufI0 = dynamic_cast<Base*>(hits[0])->rand(ssr);
        const csc bufI1 = dynamic_cast<Base*>(hits[1])->rand(ssr);
        const size_t lenI0 = bufI0.size();
        const size_t lenI1 = bufI1.size();
        if (lenI0 == lenI1) {       /* if ssr alternatives are of length */
            switch (lenI0) {
            case 1:
            {
                const char val = (bufI0[0] < bufI1[0]) ?
                    char_rangerand(bufI0[0], bufI1[0]) :
                    char_rangerand(bufI1[0], bufI0[0]);
                memcpy(bufI, &val, lenI0);
                sizeI = lenI0;
                break;
            }
            default:
                std::cerr << "Cannot handle case for sizeI == " << lenI0 << std::endl; break;
            }
        } else { PERR("Cannot handle case for lenI0 != lenI1. \todo How should we interpolate ranges of different lengths???\n"); }
    } else {
        std::cerr << "Cannot handle case for elmsN=" << hits.size() << std::endl;
    }
    return csc(bufI, sizeI);
}
#endif

/*! Example: Scanning x for Alt(8-bit:0, 8-bit:127) can be optimized by ANDing x
 *  with 0x80. */
void Ran::auto_set_detected_pmatch()
{
    if (m_subs.size() == 2) {
        auto it = begin(m_subs);
        const Lit * s0 = dynamic_cast<const Lit*>(*it);
        it++;
        const Lit * s1 = dynamic_cast<const Lit*>(*it);
        if (s0 and s1) {                                   // if both literals
            if (is_byte(s0->bitsize()) and
                is_byte(s1->bitsize())) { // if both 8-bits (byte/char) long
                if (s0->get_byte(0) == 0) { // if first it has value 0
                    switch (s1->get_byte(0)) {
                    case 3: // range: [0,3]
                        m_cbuf_lmatchP = cbuf_lmatch_8bit_0to3;
                        break;
                    case 7: // range: [0,7]
                        m_cbuf_lmatchP = cbuf_lmatch_8bit_0to7;
                        break;
                    case 15: // range: [0,15]
                        m_cbuf_lmatchP = cbuf_lmatch_8bit_0to15;
                        break;
                    case 31: // range: [0,31]
                        m_cbuf_lmatchP = cbuf_lmatch_8bit_0to31;
                        break;
                    case 63: // range: [0,63]
                        m_cbuf_lmatchP = cbuf_lmatch_8bit_0to63;
                        break;
                    case 127: // range: [0,127]
                        m_cbuf_lmatchP = cbuf_lmatch_8bit_0to127;
                        break;
                    }
                }
            }
        }
    }
}

Base*
Ran::mutating_canonicalize()
{
    Base* ret = this;
    switch (m_subs.size()) {
    case 2: {
        auto it = begin(m_subs);
        Lit* subA = dynamic_cast<Lit*>(*it); it++;
        Lit* subB = dynamic_cast<Lit*>(*it);
        // TODO: Generalize to n bytes and reuse auto_set_detected_pmatch()
        if (subA and subB) {    // if both literals
            const auto nbytes = subA->bitsize().get() / 8;
            if ((subA->bitsize() == 8 and
                 subB->bitsize() == 8 and
                 subA->get_byte() == 0 and
                 (~subB->get_byte()) == 0)
                or
                (subA->bitsize() == 16 and
                 subB->bitsize() == 16 and
                 subA->get_i16ne() == 0 and
                 (~subB->get_i16ne()) == 0)
                or
                (subA->bitsize() == 32 and
                 subB->bitsize() == 32 and
                 subA->get_i32ne() == 0 and
                 (~subB->get_i32ne()) == 0)
                or
                (subA->bitsize() == 64 and
                 subB->bitsize() == 64 and
                 subA->get_i64ne() == 0 and
                 (~subB->get_i64ne()) == 0))
            {
                ret = mutate_to(anybytes(nbytes)); // mutate \p this to \c Any
            }
        }
        break;
    }
    default: {
        break;
    }
    }
    return ret;
}

Base::Skips8 &
Ran::intersect_skips(Skips8& skips) const
{
    PTODO("Like in Alt but with iteration \n");
    return load_skips8();
}

}
}
