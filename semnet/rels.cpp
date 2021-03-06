#include "rels.hpp"
#include "ob.hpp"
#include "arr.hpp"
#include "ob_cmp.hpp"
#include "../qsort.h"
#include <algorithm>

namespace semnet {

Refs::Refs(const Refs& in)
{
    init(in.m_num);
    set(in.d.b.d, in.d.b.t, in.m_num);
    // just copy states
    m_sorted = in.m_sorted;
    m_packed = in.m_packed;
}

/* ---------------------------- Group Separator ---------------------------- */

Refs& Refs::set(Ob* const rD[], const REL_t rT[], size_t num)
{
    if (num == 1) {
        d.b.d[0] = rD[0]; d.b.t[0] = rT[0];
    } else if (num == 2) {
        d.b.d[0] = rD[0]; d.b.t[0] = rT[0];
        d.b.d[1] = rD[1]; d.b.t[1] = rT[1];
    } else {
        std::copy(rD, rD + m_num, d.m.d);
        std::copy(rT, rT + m_num, d.m.t);
    }
    return *this;
}

/* ---------------------------- Group Separator ---------------------------- */

void
Refs::expand_room_from_b_to_m(size_t room)
{
    Ob*  _D[m_num];     // temporary destinations buffer
    REL_t _T[m_num];     // temporary types buffer

    // save
    std::copy(d.b.d, d.b.d + m_num, _D);
    std::copy(d.b.t, d.b.t + m_num, _T);

    // allocate new
    d.m.room = room;
    d.m.d = new Ob*[d.m.room];
    d.m.t = new REL_t[d.m.room];

    // restore
    std::copy(_D, _D + m_num, d.m.d);
    std::copy(_T, _T + m_num, d.m.t);
}

void
Refs::expand_room_in_m(size_t room)
{
    Ob* _D[m_num];  // temporary destinations buffer
    REL_t _T[m_num]; // temporary types buffer

    // save
    std::copy(d.m.d, d.m.d + m_num, _D);
    std::copy(d.m.t, d.m.t + m_num, _T);

    // delete existing arrays
    delete[] d.m.d;
    delete[] d.m.t;

    // allocate new
    d.m.room = room;
    d.m.d = new Ob*[d.m.room];
    d.m.t = new REL_t[d.m.room];

    // restore
    std::copy(_D, _D + m_num, d.m.d);
    std::copy(_T, _T + m_num, d.m.t);
}

/* ---------------------------- Group Separator ---------------------------- */

void Refs::resize(size_t num, REL_t rT, Ob * rD)
{
    if      (m_num < num) { expand(num, rT, rD); }
    else if (m_num > num) { truncate(num); }
}

void Refs::expand(size_t num, REL_t rT, Ob * rD)
{
    if (m_num <= REFS_PNUM) {
        if (num <= REFS_PNUM) { // keep d.b
             for (size_t i = m_num; i < num; i++) {
                d.b.d[i] = rD;
                d.b.t[i] = rT;
            }
        } else {                  // d.b => d.m
            Refs::expand_room_from_b_to_m(num);
            std::fill(d.m.t + m_num, d.m.t + num, rT);
            std::fill(d.m.d + m_num, d.m.d + num, rD);
        }
    } else {                  // keep d.m
        if (num > d.m.room) { // if we exceed allocated number of elements
            expand_room_in_m(num);
        }

        // set/copy arguments
        std::fill(d.m.t + m_num, d.m.t + num, rT);
        std::fill(d.m.d + m_num, d.m.d + num, rD);
    }
    m_num = num;
    if (m_sorted) {
        m_sorted = (num == 1 or (lastD() <= rD));
    }
}

void Refs::truncate(size_t num)
{
    if (m_num <= REFS_PNUM) {
        if (num <= REFS_PNUM) { // keep d.b
            // just discard elements in d.b
        } else {                 // d.b => d.m
            PERR("This should not happen if we truncate!");
        }
    } else {
        if (num <= REFS_PNUM) { // d.m => d.b
            Ob*  _D[num];  // temporary destinations buffer
            REL_t _T[num]; // temporary types buffer

            // save
            std::copy(d.m.d, d.m.d + num, _D);
            std::copy(d.m.t, d.m.t + num, _T);

            // delete existing arrays
            delete[] d.m.d;
            delete[] d.m.t;

            // pack \c REFS_PNUM elements
            std::copy(_D, _D + num, d.b.d);
            std::copy(_T, _T + num, d.b.t);
        } else {                // keep d.m
            const bool gc_flag = true; // "garbage collect"
            if (gc_flag) {
                Ob*  _D[num];  // temporary destinations buffer
                REL_t _T[num]; // temporary types buffer

                // save
                std::copy(d.m.d, d.m.d + num, _D);
                std::copy(d.m.t, d.m.t + num, _T);

                // delete existing arrays
                delete[] d.m.d;
                delete[] d.m.t;

                // allocate new
                d.m.room = num;
                d.m.d = new Ob*[d.m.room];
                d.m.t = new REL_t[d.m.room];

                // pack \c REFS_PNUM elements
                std::copy(_D, _D + num, d.m.d);
                std::copy(_T, _T + num, d.m.t);
            }
        }
    }
    m_num = num;
    m_sorted = (num <= 1);
}

/* ---------------------------- Group Separator ---------------------------- */

void Refs::clear()
{
    if (m_num > REFS_PNUM) {
        if (d.m.d) { delete[] d.m.d; d.m.d = nullptr; }
        if (d.m.t) { delete[] d.m.t; d.m.t = nullptr; }
        d.m.room = 0;
    } else {
        d.m.room = 2;
    }
    m_num = 0;
    m_sorted = true;
    m_packed = true;
}

/* ---------------------------- Group Separator ---------------------------- */

bool Refs::sort()
{
    bool ret = false;
    if (not m_sorted) {
        switch (m_num) {
        case 0: break;              // nothing needed
        case 1: break;              // nothing needed
        case 2:
            // if (voidp_ob_cmp_type_addr(&d.m.d[0], &d.m.d[1]) > 0) {
            //     std::swap(d.m.d[0], d.m.d[1]);
            //     std::swap(d.m.t[0], d.m.t[1]);
            // }
            break;
        default: {
            size_t idx[m_num];  // indexes
            for (size_t i = 0; i < m_num; i++) { idx[i] = i; } // generate indexes from 0 to \c m_num-1
            //iqsort(d.m.d, idx, m_num, sizeof(Ob*), voidp_ob_cmp_type_addr);
            //qsort(d.m.d, m_num, sizeof(Ob*), voidp_ob_cmp_type);

            REL_t tmpT[m_num]; // temporary relation array
            for (size_t i = 0; i < m_num; i++) { tmpT[i] = d.m.t[idx[i]]; } // create sorted relations
            for (size_t i = 0; i < m_num; i++) { d.m.t[i] = tmpT[i]; } // set sorted relations
            break;
        }
        }
        m_sorted = true;
        ret = true;
    }
    return ret;
}

void
Refs::calc_stat(bool * homo_flag_ret)
{
    parr_calc_stat(dataD(), size(), homo_flag_ret);
}

/* ---------------------------- Group Separator ---------------------------- */

Refs &
Refs::app(REL_t rT, Ob * rD)
{
    expand(m_num + 1, rT, rD);
    return *this;
}

Refs &
Refs::app(Ob* rD)
{
    return Refs::app(REL_any_, rD);
    return *this;
}

Refs &
Refs::app(REL_t rT, Ob* const* rDA, size_t rN)
{
    const size_t num = m_num + rN;
    if (m_num <= REFS_PNUM) {
        if (num <= REFS_PNUM) { // keep d.b
            for (size_t i = 0; i < rN; i++) {
                d.b.d[m_num + i] = rDA[i];
                d.b.t[m_num + i] = rT;
            }
        } else {                  // d.b => d.m
            Refs::expand_room_from_b_to_m(num);
            std::fill(d.m.t + m_num, d.m.t + num, rT);
            std::copy(rDA, rDA + rN, d.m.d + m_num);
        }
    } else {                  // keep d.m
        if (num > d.m.room) { // if we exceed allocated number of elements
            expand_room_in_m(num);
        }

        // copy destination argument array
        std::fill(d.m.t + m_num, d.m.t + num, rT);
        std::copy(rDA, rDA + rN, d.m.d + m_num);
    }
    m_num = num;
    return *this;
}

Refs &
Refs::app(const REL_t* rTA, Ob * const* rDA, size_t rN)
{
    const size_t num = m_num + rN;
    if (m_num <= REFS_PNUM) {
        if (num <= REFS_PNUM) { // keep d.b
            for (size_t i = 0; i < rN; i++) {
                d.b.d[m_num + i] = rDA[i];
                d.b.t[m_num + i] = rTA[i];
            }
        } else {                  // d.b => d.m
            Refs::expand_room_from_b_to_m(num);
            std::copy(rTA, rTA + rN, d.m.t + m_num);
            std::copy(rDA, rDA + rN, d.m.d + m_num);
        }
    } else {                  // keep d.m
        if (num > d.m.room) { // if we exceed allocated number of elements
            expand_room_in_m(num);
        }

        // copy destination argument array
        std::copy(rTA, rTA + rN, d.m.t + m_num);
        std::copy(rDA, rDA + rN, d.m.d + m_num);
    }
    m_num = num;
    return *this;
}

Refs &
Refs::app(const Refs& a)
{
    return app(a.dataT(), a.dataD(), a.m_num);
}

Refs &
Refs::appOs(Ob * const* obs, size_t num, OB_t oT)
{
    for (size_t i = 0; i < num; i++) {
        Ob * ob = obs[i];
        if (ob and
            OB_matchS(ob->get_type(), oT)) {
            app(ob);
        }
    }
    return *this;
}

Refs &
Refs::app_non_unique(const Refs * b)
{
    return Refs::app(b->d.m.t, b->d.m.d, b->m_num);
}

/* ---------------------------- Group Separator ---------------------------- */

size_t
Refs::remRm(const REL_t * rM, size_t rM_N, Ob * rD_arg)
{
    int rnum = 0;               // remove count
    switch (m_num) {
    case 0: break;              // nothing needed
    case 1: {
        Ob * const rD = d.b.d[0];
        const REL_t rT = d.b.t[0];
        if ((rD_arg == nullptr or rD_arg == rD) and
            REL_matchM(rT, rM, rM_N)) {
            m_num = 0;
            rnum++;
        }
        break;
    }
    case 2: {
        Ob * rD = d.b.d[1];
        REL_t rT = d.b.t[1];
        if ((rD_arg == nullptr or rD_arg == rD) and
            REL_matchM(rT, rM, rM_N)) {
            m_num = 1;
            rnum++;
        }
        rD = d.b.d[0];
        rT = d.b.t[0];
        if ((rD_arg == nullptr or rD_arg == rD) and
            REL_matchM(rT, rM, rM_N)) {
            m_num = 0;
            rnum++;
        }
        break;
    }
    default: {
        size_t i, pi;		// normal index and pack index
        for (i = pi = 0; i < m_num; i++) {
            Ob * const rD = d.m.d[i];
            const REL_t rT = d.m.t[i];
            if (rD and // non-zero destination
                (rD_arg == nullptr or // nullptr indicates all related objects
                 rD_arg == rD) and // a specific destinating object
                REL_matchM(rT, rM, rM_N)) { // matching relation
                d.m.d[i] = 0;	// mark as empty
                rnum++;
                //       lprintf("RELATION %d/%d ZEROED.\n", i, m_num);
            } else if (rD) {	// move relation to a different position
                d.m.d[pi] = rD;
                d.m.t[pi] = rT;
                pi++;
                //       lprintf("RELATION %d/%d keeped.\n", i, m_num);
            } else {
                //       lprintf("RELATION %d/%d ALREADY EMPTY.\n", i, m_num);
            }
        }
        truncate(pi);
        m_packed = true;
    }
    }
    return rnum;
}

/* ---------------------------- Group Separator ---------------------------- */

size_t Refs::uniquify(REL_t rT, Ob* rbD)
{
    size_t ret = 0;

    sort();                     // first sort it

    // remove adjacent duplicates
    switch (m_num) {
    case 0: break;              // nothing needed
    case 1: break;              // nothing needed
    case 2:
        if (d.b.d[0] == d.b.d[1] and // if same dest
            REL_matchS(d.b.t[0], rT) and // and matching relations
            d.b.t[0] == d.b.t[1]) {      // that must be equal
            m_num = 1;          // simply forget the second one
        }
        break;
    default: {
        size_t i, pi;           // normal index and pack index
        Ob * p_rD = nullptr;      // previous relation destination
        REL_t p_rT = REL_any_;     // previous relation type
        for (i = pi = 0; i < m_num; i++) {
            if (d.m.d[i])
            {                      // if non-zero
                if (p_rD == d.b.d[i] and // if same dest
                    p_rT == d.b.t[i] and // and same type
                    REL_matchS(d.b.t[i], rT)) { // and matching relations
                    ret++;                   // increase duplicates count
                } else {                     // otherwise
                    // keep element at index \c i
                    d.m.d[pi] = d.m.d[i];
                    d.m.t[pi] = d.m.t[i];
                    pi++;
                }
            }
            p_rD = d.m.d[i];    // save destination to next loop
            p_rT = d.m.t[i];    // save type to next loop
        }
        truncate(pi);
        m_packed = true;
        break;
    }
    }

    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

size_t Refs::retarget(Ob * from, Ob * to)
{
    size_t hc = 0;              // hit counter
    switch (m_num) {
    case 0: break;              // nothing needed
    case 1: {
        if (from == nullptr or from == d.b.d[0]) { d.b.d[0] = to; hc++; }
        break;
    }
    case 2: {
        if (from == nullptr or from == d.b.d[0]) { d.b.d[0] = to; hc++; }
        if (from == nullptr or from == d.b.d[1]) { d.b.d[1] = to; hc++; }
        break;
    }
    default: {
        size_t i, pi;			// normal index and pack index
        for (i = pi = 0; i < m_num; i++) {
            if (d.m.d[i]) {
                if (d.m.d[i] == from) { // if correct source pointer
                    d.m.d[pi] = to;   // replace target pointer
                    d.m.t[pi] = d.m.t[i]; // copy relation type
                    hc++;
                }
                pi++;
            }
        }
        truncate(pi);
        m_packed = true;
        break;
    }
    }
    return hc;
}

size_t Refs::retype(REL_t from, REL_t to)
{
    size_t hc = 0;              // hit counter
    switch (m_num) {
    case 0: break;              // nothing needed
    case 1: {
        if (REL_matchS(d.b.t[0], from)) { d.b.t[0] = to; hc++; }
        break;
    }
    case 2: {
        if (REL_matchS(d.b.t[0], from)) { d.b.t[0] = to; hc++; }
        if (REL_matchS(d.b.t[1], from)) { d.b.t[1] = to; hc++; }
        break;
    }
    default: {
        size_t i, pi;			// normal index and pack index
        for (i = pi = 0; i < m_num; i++) {
            if (d.m.d[i]) {
                if (d.m.t[i] == from) { // if correct source pointer
                    d.m.t[pi] = to;   // replace target pointer
                    d.m.d[pi] = d.m.d[i]; // copy relation type
                    hc++;
                }
                pi++;
            }
        }
        truncate(pi);
        m_packed = true;
        break;
    }
    }
    return hc;
}

/* ---------------------------- Group Separator ---------------------------- */

size_t
Refs::pack()
{
    size_t ret = 0;
    switch (m_num) {
    case 0: break;              // nothing needed
    case 1: {
        if (d.b.d[0] == nullptr) { m_num = 0; ret++; }
        break;
    }
    case 2: {
        if (d.b.d[1] == nullptr) { m_num = 1; ret++; }
        if (d.b.d[0] == nullptr) { m_num = 0; ret++; }
        break;
    }
    default: {
        size_t i, pi;			// normal index and pack index
        for (i = pi = 0; i < m_num; i++) {
            if (d.m.d[i]) {
                d.m.d[pi] = d.m.d[i];
                d.m.t[pi] = d.m.t[i];
                pi++;
            }
        }
        ret = m_num - pi;
        truncate(pi);
        m_packed = true;
        break;
    }
    }
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

static bool binary_find_flag = false;

ssize_t
Refs::find(const Ob* key) const
{
    ssize_t ret = -1;
    switch (m_num) {
    case 0: break;              // nothing needed
    case 1: {
        if (d.b.d[0] == key) { ret = 0; }
        break;
    }
    case 2: {
        if      (d.b.d[0] == key) { ret = 0; }
        else if (d.b.d[1] == key) { ret = 1; }
        break;
    }
    default: {
        if (binary_find_flag) {
            //sort();
            //std::binary_search();
        } else {
            for (size_t i = 0; i < m_num; i++) {
                if (d.m.d[i] == key) { ret = i; break; }
            }
        }
        break;
    }
    }
    return ret;
}

ssize_t
Refs::find(REL_t rT, const Ob * key) const
{
    ssize_t ret = -1;
    if (rT == REL_any_ or
        (m_num > REFS_PNUM and
         not d.m.t)) {
        ret = find(key);
    } else {
        switch (m_num) {
        case 0: break;              // nothing needed
        case 1: {
            if (d.b.d[0] == key and d.b.t[0] == rT) { ret = 0; }
            break;
        }
        case 2: {
            if      (d.b.d[0] == key and d.b.t[0] == rT) { ret = 0; }
            else if (d.b.d[1] == key and d.b.t[1] == rT) { ret = 1; }
            break;
        }
        default: {
            if (binary_find_flag) {
                //sort();
            } else {
                for (size_t i = 0; i < m_num; i++) {
                    if (d.m.d[i] == key and d.m.t[i] == rT) { ret = i; break; }
                }
            }
            break;
        }
        }
    }
    return ret;
}

int
Refs::is_sorted_as(cmp_function_t * cmp)
{
    // TODO: Reuse std::is_sorted from <algorithm> header.
    int cnt = 0;		      /* number of incorrect sorted objects */
    for (size_t i = 1; i < size(); i++) {
        Ob * prev = nthD(i-1);
        Ob * curr = nthD(i);
        if (prev and curr) {
            if (cmp(&prev, &curr) > 0) {	/* if not sorted */
                cnt++;			/* increase sort fault counter */
            }
        } else {
            PWARN("prev or curr is nullptr\n");
            cnt++;			/* increase sort fault counter */
        }
    }
    return (cnt == 0);
}

bool
Refs::is_unique() const
{
    return true;
}

}
