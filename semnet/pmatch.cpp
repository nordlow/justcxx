#include "pmatch.hpp"
#include "../substr_match.h"
#include <regex.h>
#include <iostream>
#include <iomanip>

const char *
pMode::name() const
{
    const char * str = nullptr;
    if (m_ci) {
        switch (m_mT) {
        case MATCH_FULL: str = "Full ci"; break;
        case MATCH_PREFIX: str = "Prefix ci"; break;
        case MATCH_SUFFIX: str = "Suffix ci"; break;
        case MATCH_PARTIAL: str = "Partial ci"; break;
        case MATCH_ACRONYM: str = "Acronym ci"; break;
        case MATCH_REGEXP: str = "Regexp ci"; break;
        case MATCH_FUZZY: str = "Fuzzy ci"; break;
        }
    } else {
        switch (m_mT) {
        case MATCH_FULL: str = "Full"; break;
        case MATCH_PREFIX: str = "Prefix"; break;
        case MATCH_SUFFIX: str = "Suffix"; break;
        case MATCH_PARTIAL: str = "Partial"; break;
        case MATCH_ACRONYM: str = "Acronym"; break;
        case MATCH_REGEXP: str = "Regexp"; break;
        case MATCH_FUZZY: str = "Fuzzy"; break;
        }
    }
    return str;
}

/** Start with substring case-insensitive match. */
pMode g_search_pmc(MATCH_PARTIAL, true);

bool g_search_hit_flag = 0;
int g_search_hit_off = -1;
int g_search_hit_len = -1;

#ifndef _GNU_SOURCE
char *strcasestr(const char *haystack, const char *needle);
#endif

pHit cbuf_find_LMATCH_FULL(const char * dat, size_t datL,
                           const char * key, size_t keyL,
                           pMode pmm) {
    auto match = (pmm.ci() ?
                  cbuf_ifindBeg(dat,datL, key,keyL) :
                  cbuf_findBeg(dat,datL, key,keyL));
    return pHit(0, match == keyL ? keyL : 0);
}

pHit cbuf_find_LMATCH_PREFIX(const char * dat, size_t datL,
                             const char * key, size_t keyL,
                             pMode pmm)
{
    return pHit(0, (pmm.ci() ?
                    cbuf_ibegins(dat,datL, key,keyL) :
                    cbuf_begins(dat,datL, key,keyL)));
}

pHit cbuf_find_LMATCH_SUFFIX(const char * dat, size_t datL,
                             const char * key, size_t keyL,
                             pMode pmm)
{
    auto match = pmm.ci() ? cbuf_iends(dat,datL, key,keyL) : cbuf_ends(dat,datL, key,keyL);
    return (match ?
            pHit(0, match) :
            pHit().zero());
}

pHit cbuf_find_LMATCH_PARTIAL(const char * dat, size_t datL,
                              const char * key, size_t keyL,
                              pMode pmm)
{
    if (auto off = (pmm.ci() ?
                    const_cast<char*>(strcasestr(dat, key)) :
                    strstr(dat, key))) {                  // if hit offset
        return pHit(off - dat, keyL);
    } else {
        return pHit().zero();
    }
}

pHit cbuf_find_LMATCH_ACRONYM(const char * dat, size_t datL,
                              const char * key, size_t keyL,
                              pMode pmm,
                              size_t * offs, size_t * offsN)
{
    size_t cnt = 0;                /* pattern index (hit count) */
    for (size_t i = 0; i < datL and cnt < keyL; i++) {
        if (pmm.ci() ?
            (::tolower(dat[i]) == ::tolower(key[cnt])) :
            (dat[i] == key[cnt])) {
            offs[cnt++] = i;
        }
    }
    *offsN = cnt;                /* number of hits */
    if (cnt) {
        return bir(offs[0],
                   offs[cnt-1] + 1);
    } else {
        return pHit();
    }
}

pHit cbuf_find_LMATCH_REGEXP(const char * dat, size_t datL,
                             const char * key, size_t keyL,
                             bool icase)
{
    regex_t preg;
    if (auto comp_ret = regcomp(&preg, key, icase ? REG_ICASE : 0)) {
        size_t errbuf_size = 128;
        char errbuf[errbuf_size+1];
        auto error_ret = regerror(comp_ret, &preg, errbuf, errbuf_size);
        errbuf[error_ret] = '\0';
        /* \todo cmdbar_hint_set(errbuf); */
    } else {
        size_t nmatch = 1;
        regmatch_t rm[nmatch];
        int eflags = 0;
        auto exec_ret = regexec(&preg, dat, nmatch, rm, eflags);
        if (exec_ret == 0) {
            /* at least one match beginning in rm[0] */
            if (rm[0].rm_so >= 0) {
                return bir(rm[0].rm_so,
                           rm[0].rm_eo);
            }
        } else { /* no match */ }
        /* \todo cmdbar_hint_set(nullptr); */
    }
    regfree(&preg);
    return pHit().undefine();
}

pHit cbuf_find_LMATCH(const char * dat, size_t datL,
                      const char * key, size_t keyL,
                      pMode pmm)
{
    switch (pmm.get_type()) {
    case MATCH_FULL:
        return cbuf_find_LMATCH_FULL(dat,datL, key,keyL, pmm); break;
    case MATCH_PREFIX:
        return cbuf_find_LMATCH_PREFIX(dat,datL, key,keyL, pmm); break;
    case MATCH_SUFFIX:
        return cbuf_find_LMATCH_SUFFIX(dat,datL, key,keyL, pmm); break;
    case MATCH_PARTIAL:
        return cbuf_find_LMATCH_PARTIAL(dat,datL, key,keyL, pmm); break;
    case MATCH_ACRONYM: {
        size_t offs_ret[keyL];      /* hits are non-adjacent so we need a hit-list */
        size_t offs_num_ret;        /* number of hit offsets found */
        auto hit = cbuf_find_LMATCH_ACRONYM(dat,datL, key,keyL, pmm, offs_ret, &offs_num_ret);
        hit.set_offset(0); hit.set_length(0);
        if (offs_num_ret >= 1) {
            hit.set_offset(offs_ret[offs_num_ret-1]);
        }
        return hit;
        break;
    }
    case MATCH_REGEXP:
        return cbuf_find_LMATCH_REGEXP(dat,datL, key,keyL, pmm.ci()); break;
    case MATCH_FUZZY:
        PTODO("fuzzy search not implemented!\n");
        return pHit().undefine(); break;
    default:
        PWARN("Unknown m_mT %d!\n", pmm.get_type());
        return pHit().undefine(); break;
    }
    return pHit().undefine();
}

pHit cstr_find_LMATCH(const char * dat, const char * key, size_t keyL, pMode pmm)
{
    return cbuf_find_LMATCH(dat, strlen(dat),
                            key,keyL,
                            pmm);
}

pHit csc_find_LMATCH(const csc & dat, const csc & key, pMode pmm)
{
    return cbuf_find_LMATCH(dat.data(), dat.size(),
                            key.data(), key.size(),
                            pmm);
}
