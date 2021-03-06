#include "iter.hpp"
#include "patt.hpp"
#include "regfile.hpp"

#if 0
Obs Iter::perform(bool overlap)
{
    Obs hits;

    auto rangeL = bir::full();
    bix minL = bix::inf();         // minimum look-ahead
    bix maxL = bix::min();      // maximum look-ahead

    // prepare pattern statistics
    Ob * obC = net_first(REL_CHILD, OB_PATT_);
    if (auto ppatt = dynamic_cast<Base*>(obC)) {
        ppatt->auto_set_detected_pmatch();
        ppatt->sample_range(minL, maxL);
    }

    // \todo
    // apply this => REL_CHILD => ppatts member-functions on them
    //
    // We need a \c File \c Base Content Block Iterator that respects
    // - Rep::overlapF: Determines if multiple hits should overlap or not
    // - std::iostream
    //
    // - begin by reading in \c minL bytes from stream and perform scan
    // - if that fails read File-stream blocks until we get a hit

    // for (auto& ob : m_obR) {
    //     if (ob and
    //         ob->match_type(scan->get_target_type())) {
    //         pHit phit;
    //         int mret = csc_find_LMATCH(ob->get_pathL(), key, pMode(), hit);
    //         if (mret >= 1) {
    //             // std::cout << " mret:" << mret
    //             //           << " off:" << hit.get_offset()
    //             //           << " len:" << hit.get_length();
    //             hits.push_back(ob);
    //         }
    //     }
    // }

    return hits;
}
#endif
