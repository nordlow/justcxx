#include "REL_enum.hpp"
#include "../ASCII.h"
#include "../UNICODE.h"
#include "../stdio_x.h"
#include <boost/bimap.hpp>

/* ========================================================================== */

REL_t g_REL_any_ = REL_any_;

typedef boost::bimap< REL_t, REL_t > Duals;
typedef Duals::value_type Dual;
static Duals g_duals;

void REL_makeDual(REL_t relA, REL_t relB)
{
    // g_duals.insert(Dual(relA, relB));
}

__attribute__ ((constructor))
void reldualtab_init()
{
    REL_makeDual(REL_OPPOSITE_BWD, REL_OPPOSITE_FWD);
    REL_makeDual(REL_SIBLING, REL_SIBLING);
    REL_makeDual(REL_SIBLING_PREV, REL_SIBLING_NEXT);
    REL_makeDual(REL_ORIGINAL, REL_COPY);
    REL_makeDual(REL_LIMITER_LOWER, REL_LIMITEE_LOWER);

    REL_makeDual(REL_SLAVE_RELSYM_FWD, REL_MASTER_RELSYM_OWNER);
    REL_makeDual(REL_SLAVE_RELSYM_BWD, REL_MASTER_RELSYM_TARGET);

    REL_makeDual(REL_ALIGNER_VSCRIPT_SUPER, REL_ALIGNEE_VSCRIPT_SUPER);
    REL_makeDual(REL_ALIGNER_VSCRIPT_SUB, REL_ALIGNEE_VSCRIPT_SUB);

    REL_makeDual(REL_ALIGNEE_HL, REL_ALIGNEE_HR);
    REL_makeDual(REL_ALIGNEE_VT, REL_ALIGNEE_VB);

    REL_makeDual(REL_OP, REL_ARG0);
    REL_makeDual(REL_OP, REL_ARG1);

    REL_makeDual(REL_SOURCE, REL_TARGET);
    REL_makeDual(REL_CALLER, REL_CALLEE);
    REL_makeDual(REL_GENERALIZER, REL_SPECIALIZER);
    REL_makeDual(REL_GROUP, REL_MEMBER);
    REL_makeDual(REL_MODEL_VIEW, REL_VIEW_MODEL);

    REL_makeDual(REL_SLAVE_RELSYM_FWD, REL_MASTER_RELSYM_OWNER);
    REL_makeDual(REL_SLAVE_RELSYM_BWD, REL_MASTER_RELSYM_TARGET);

    REL_makeDual(REL_ALIGNER_VSCRIPT_SUPER, REL_ALIGNEE_VSCRIPT_SUPER);
    REL_makeDual(REL_ALIGNER_VSCRIPT_SUB, REL_ALIGNEE_VSCRIPT_SUB);

    REL_makeDual(REL_ALIGNEE_HL, REL_ALIGNEE_HR);
    REL_makeDual(REL_ALIGNEE_VT, REL_ALIGNEE_VB);
    REL_makeDual(REL_OP, REL_ARG0);
    REL_makeDual(REL_OP, REL_ARG1);

    REL_makeDual(REL_PRODUCER, REL_CONSUMER);
    REL_makeDual(REL_OP_CACHE, REL_CACHING_OP);
    REL_makeDual(REL_SCALE_UP2, REL_SCALE_DOWN2);

    REL_makeDual(REL_NAMED_IMAGE_FEATURE, REL_IMAGE_FEATURE_NAME);
}

REL_t REL_getDual(REL_t rel)
{
    REL_t dual = REL_undefined_;

    const auto hit = g_duals.left.find(rel);
    if (hit != g_duals.left.end()) {
        dual = hit->second;
    }

    else if (REL_is_PARENT(rel)) { dual = REL_CHILD; }
    else if (REL_is_CHILD(rel)) { dual = REL_PARENT; }

    else if (REL_is_MASTER(rel)) { dual = REL_SLAVE; }
    else if (REL_is_SLAVE(rel)) { dual = REL_MASTER; }

    else if (REL_is_PRODUCER(rel)) { dual = REL_CONSUMER; }
    else if (REL_is_CONSUMER(rel)) { dual = REL_PRODUCER; }

    else if (REL_is_TEXT(rel)) { dual = REL_LETTER; }
    else if (REL_is_LETTER(rel)) { dual = REL_TEXT; }
    else if (REL_is_CURSOR(rel)) { dual = REL_TEXT; }

    return dual;
}

static bool
REL_dualsOrdered(REL_t relA, REL_t relB)
{
    if (REL_getDual(relA) == relB) { return true; }
    if (REL_is_PARENT(relA) and
        REL_is_CHILD(relB)) { return true; }
    if (REL_is_MASTER(relA) and
        REL_is_SLAVE(relB)) { return true; }
    if (REL_is_PRODUCER(relA) and
        REL_is_CONSUMER(relB)) { return true; }
    if (REL_is_TEXT(relA) and
        REL_is_LETTER(relB)) { return true; }
    if (REL_is_TEXT(relA) and
        REL_is_CURSOR(relB)) { return true; }
    return false;
}

bool
REL_duals(REL_t relA, REL_t relB)
{
    return (REL_dualsOrdered(relA, relB) or
            REL_dualsOrdered(relB, relA));
}

bool
REL_duals_check(REL_t rT0, REL_t rT1)
{
    bool ok = true;
    if (not REL_duals(rT0, rT1)) {
        PERR("Relation Types %d (%s) and %d (%s) are not duals\n",
                rT0, REL_getName(rT0),
                rT1, REL_getName(rT1));
        ok = false;
    }
    return ok;
}

bool
REL_matchS(REL_t rT, REL_t rM1)
{
    bool match = false;
    if (rM1 == REL_any_ or /* any relation (short-circuit) */
        rT == rM1) {	/* a specific type of relation */
        match = true;
    } else {
        switch (rM1) {
        case REL_TEXT: if (REL_is_TEXT(rT)) { match = true; } break;
        case REL_LETTER: if (REL_is_LETTER(rT)) { match = true; } break;
        case REL_LETTER_EOL: if (REL_is_LETTER_EOL(rT)) { match = true; } break;
        case REL_LETTER_WS: if (REL_is_LETTER_WS(rT)) { match = true; } break;

        case REL_PARENT: if (REL_is_PARENT(rT)) { match = true; } break;
        case REL_CHILD: if (REL_is_CHILD(rT)) { match = true; } break;

        case REL_MASTER: if (REL_is_MASTER(rT)) { match = true; } break;
        case REL_SLAVE: if (REL_is_SLAVE(rT)) { match = true; } break;
        case REL_SLAVE_AXTICK: if (REL_is_SLAVE_AXTICK(rT)) { match = true; } break;
        case REL_SLAVE_LAYHINT: if (REL_is_SLAVE_LAYHINT(rT)) { match = true; } break;
        case REL_SLAVE_LAYHINT_CADJ: if (REL_is_SLAVE_CADJ(rT)) { match = true; } break;
        case REL_SIBLING: if (REL_is_SIBLING(rT)) { match = true; } break;

        case REL_PRODUCER: if (REL_is_PRODUCER(rT)) { match = true; } break;
        case REL_CONSUMER: if (REL_is_CONSUMER(rT)) { match = true; } break;

        case REL_ALIGNER: if (REL_is_ALIGNER(rT)) { match = true; } break;
        case REL_ALIGNEE: if (REL_is_ALIGNEE(rT)) { match = true; } break;
        default: break;
        }
    }
    return match;
}

bool
REL_matchM(REL_t rT, const REL_t * rM, size_t rM_N)
{
    bool match = false;
    for (size_t i = 0; i < rM_N; i++) {
        if (REL_matchS(rT, rM[i])) { match = true; break; }
    }
    return match;
}

const char *
REL_getName(REL_t rel)
{
    const char * str = nullptr;

    switch (rel) {
    case REL_any_: str = "Any"; break;

    case REL_PARENT: str = "Parent"; break;
    case REL_CHILD: str = "Child"; break;

    case REL_SIBLING: str = "Sibling"; break;
    case REL_SIBLING_PREV: str = "Sibling-Prev"; break;
    case REL_SIBLING_NEXT: str = "Sibling-Next"; break;

    case REL_SOURCE: str = "Source"; break;
    case REL_TARGET: str = "Target"; break;

    case REL_CALLER: str = "Caller"; break;
    case REL_CALLEE: str = "Callee"; break;

    case REL_GENERALIZER: str = "Generalizer"; break;
    case REL_SPECIALIZER: str = "Specializer"; break;

    case REL_GROUP: str = "Group"; break;
    case REL_MEMBER: str = "Member"; break;

    case REL_MASTER: str = "Master"; break;
    case REL_MASTER_TALKBUBBLE: str = "Master-TalkBubble"; break;
    case REL_MASTER_PLOT: str = "Master-Plot"; break;
    case REL_MASTER_PLOT_VT: str = "Master-Plot-VT"; break;
    case REL_MASTER_PLOT_VB: str = "Master-Plot-VB"; break;
    case REL_MASTER_PLOT_HL: str = "Master-Plot-HL"; break;
    case REL_MASTER_PLOT_HR: str = "Master-Plot-HR"; break;

    case REL_SLAVE: str = "Slave"; break;

    case REL_SLAVE_TITLENAME: str = "Slave-Name"; break;

    case REL_SLAVE_TYPENAME: str = "Slave-Typename"; break;
    case REL_SLAVE_DOC: str = "Slave-Documentation"; break;
    case REL_SLAVE_KCOMB: str = "Slave-KeyCombination"; break;
    case REL_SLAVE_ICON: str = "Slave-Icon"; break;
    case REL_SLAVE_ATTR: str = "Slave-Attribute"; break;

    case REL_SLAVE_TALKBUBBLE: str = "Slave-TalkBubble"; break;
    case REL_SLAVE_PROGRESS: str = "Slave-Progress"; break;

    case REL_SLAVE_AXTICK: str = "Slave-Axtick"; break;
    case REL_SLAVE_AXTICK_VT: str = "Slave-Axtick-X-Vertical-Top"; break;
    case REL_SLAVE_AXTICK_VB: str = "Slave-Axtick-X-Vertical-Bottom"; break;
    case REL_SLAVE_AXTICK_HL: str = "Slave-Axtick-Y-Horizontal-Left"; break;
    case REL_SLAVE_AXTICK_HR: str = "Slave-Axtick-Y-Horizontal-Right"; break;

    case REL_SLAVE_LAYHINT: str = "LayHint"; break;
    case REL_SLAVE_LAYHINT_CADJ: str = "LayHint-CADJ"; break;

    case REL_SLAVE_LAYHINT_CADJ_LEFT_GROW: str = "LayHint-Content-Adjust-Left-Grow"; break;
    case REL_SLAVE_LAYHINT_CADJ_RIGHT_GROW: str = "LayHint-Content-Adjust-Right-Grow"; break;
    case REL_SLAVE_LAYHINT_CADJ_TOP_GROW: str = "LayHint-Content-Adjust-Top-Grow"; break;
    case REL_SLAVE_LAYHINT_CADJ_BOT_GROW: str = "LayHint-Content-Adjust-Bottom-Grow"; break;

    case REL_SLAVE_LAYHINT_CADJ_LEFT_SHRINK: str = "LayHint-Content-Adjust-Left-Shrink"; break;
    case REL_SLAVE_LAYHINT_CADJ_RIGHT_SHRINK: str = "LayHint-Content-Adjust-Right-Shrink"; break;
    case REL_SLAVE_LAYHINT_CADJ_TOP_SHRINK: str = "LayHint-Content-Adjust-Top-Shrink"; break;
    case REL_SLAVE_LAYHINT_CADJ_BOT_SHRINK: str = "LayHint-Content-Adjust-Bottom-Shrink"; break;

    case REL_OPPOSITE_BWD: str = "Backward-Opposite"; break;
    case REL_OPPOSITE_FWD: str = "Forward-Opposite"; break;

    case REL_SLAVE_SELSYM: str = "Selsym"; break;
    case REL_SLAVE_SELSYM_UL: str = "Selsym-UL"; break;
    case REL_SLAVE_SELSYM_UR: str = "Selsym-UR"; break;
    case REL_SLAVE_SELSYM_LL: str = "Selsym-LL"; break;
    case REL_SLAVE_SELSYM_LR: str = "Selsym-LR"; break;

    case REL_MODESYM_OWNEE: str = "Modesym-Ownee"; break;
    case REL_MODESYM_OWNER: str = "Modesym-Owner"; break;

    case REL_SLAVE_RELSYM_FWD: str = "Relsym-Forward"; break;
    case REL_MASTER_RELSYM_OWNER: str = "Relsym-Owner"; break;
    case REL_MASTER_RELSYM_TARGET: str = "Relsym-Target"; break;
    case REL_SLAVE_RELSYM_BWD: str = "Relsym-Backward"; break;

    case REL_LIMITER_LOWER: str = "Lower-Limiter"; break;
    case REL_LIMITEE_LOWER: str = "Lower-Limitee"; break;
    case REL_LIMITER_UPPER: str = "Upper-Limiter"; break;
    case REL_LIMITEE_UPPER: str = "Upper-Limitee"; break;

    case REL_PRODUCER: str = "Producer"; break;
    case REL_CONSUMER: str = "Consumer"; break;
    case REL_PRODUCER_BINCALL: str = "Producer-BinaryCall"; break;
    case REL_CONSUMER_BINCALL: str = "Consumer-BinaryCall"; break;
    case REL_PRODUCER_TALKBUBBLE: str = "Producer-Frustration-Message"; break;
    case REL_CONSUMER_TALKBUBBLE: str = "Consumer-Frustration-Message"; break;

    case REL_ORIGINAL: str = "Original"; break;
    case REL_COPY: str = "Copy"; break;

    case REL_ALIGNER: str = "Aligner"; break;
    case REL_ALIGNEE: str = "Alignee"; break;

    case REL_ALIGNEE_V: str = "Vertical-Alignee"; break;
    case REL_ALIGNEE_H: str = "Horizontal-Alignee"; break;

    case REL_ALIGNEE_VT: str = "Alignee Vertical Top"; break;
    case REL_ALIGNEE_VB: str = "Alignee Vertical Bottom"; break;
    case REL_ALIGNEE_HL: str = "Alignee Horizontal Left"; break;
    case REL_ALIGNEE_HR: str = "Alignee Horizontal Right"; break;

    case REL_ALIGNER_VSCRIPT_SUB: str = "Subscript-Aligner"; break;
    case REL_ALIGNEE_VSCRIPT_SUB: str = "Subscript-Alignee"; break;
    case REL_ALIGNER_VSCRIPT_SUPER: str = "Superscript-Aligner"; break;
    case REL_ALIGNEE_VSCRIPT_SUPER: str = "Superscript-Alignee"; break;

    case REL_VFITTER: str = "Vertical-Fitter"; break;
    case REL_VFITTEE: str = "Vertical-Fittee"; break;
    case REL_HFITTER: str = "Horizontal-Fitter"; break;
    case REL_HFITTEE: str = "Horizontal-Fittee"; break;

    case REL_DIGIT: str = "Digit"; break;
    case REL_LETTER_DIGIT: str = "Letter-Digit"; break;

    case REL_LETTER_WS: str = "Letter-WhiteSpace"; break;
    case REL_LETTER_WS_SPACE: str = "Letter-Space"; break;
    case REL_LETTER_WS_HTAB: str = "Letter-Horizontal-Tab"; break;
    case REL_LETTER_WS_VTAB: str = "Letter-Vertical-Tab"; break;

    case REL_LETTER: str = "Letter"; break;
    case REL_LETTER_BOL: str = "Letter-BOL"; break;
    case REL_LETTER_EOL: str = "Letter-EOL"; break;
    case REL_LETTER_EOL_MAC: str = "Letter-EOL-Mac"; break;
    case REL_LETTER_EOL_UNIX: str = "Letter-EOL-UNIX"; break;
    case REL_LETTER_EOL_WIN: str = "Letter-EOL-Windows"; break;
    case REL_LETTER_BOT: str = "Letter-BOT"; break;
    case REL_LETTER_EOT: str = "Letter-EOT"; break;
    case REL_LETTER_BOI_C: str = "Letter-BOI-C"; break;
    case REL_LETTER_EOI_C: str = "Letter-EOI-C"; break;

    case REL_LETTER_ABBREV: str = "Letter-Abbreviation"; break;
    case REL_LETTER_ABBREV_BEG: str = "Letter-Abbreviation-Begin"; break;
    case REL_LETTER_ABBREV_END: str = "Letter-Abbreviation-End"; break;

    case REL_MARK: str = "Mark"; break;
    case REL_CURSOR: str = "Cursor"; break;

    case REL_TEXT: str = "Line"; break;

    case REL_OP: str = "Op"; break;
    case REL_ARG: str = "Argument"; break;
    case REL_ARG0: str = "First-Argument"; break;
    case REL_ARG1: str = "Second-Argument"; break;

    case REL_VIEW_MODEL: str = "View-Model"; break;
    case REL_MODEL_VIEW: str = "Model-View"; break;

    case REL_OP_CACHE: str = "Operation-Cache"; break;
    case REL_CACHING_OP: str = "Caching-Op"; break;

    case REL_SCALE_DOWN2: str = "Scale-Up-by-2"; break;
    case REL_SCALE_UP2: str = "Scale-Down-by-2"; break;

    case REL_NAMED_IMAGE_FEATURE: str = "Named Image Feature"; break;
    case REL_IMAGE_FEATURE_NAME: str = "Image Feature Name"; break;

    case REL_ALTERNATE: str = "Alternate"; break;
    case REL_STYLESHEET: str = "Stylesheet"; break;
    case REL_START: str = "Start"; break;
    case REL_CONTENTS: str = "Contents"; break;
    case REL_INDEX: str = "Index"; break;
    case REL_GLOSSARY: str = "Gloassary"; break;
    case REL_COPYRIGHT: str = "Copyright"; break;
    case REL_CHAPTER: str = "Chapter"; break;
    case REL_SECTION: str = "Section"; break;
    case REL_SUBSECTION: str = "SubSection"; break;
    case REL_APPENDIX: str = "Appendix"; break;
    case REL_HELP: str = "Help"; break;
    case REL_BOOKMARK: str = "Bookmark"; break;

    case REL_undefined_: str = "Undefined"; break;
    }

    return str;
}

REL_t
REL_LETTER_from_wchar(wchar_t val)
{
    REL_t rel = REL_LETTER;
    switch (val) {
    case '0'...'9': rel = REL_LETTER_DIGIT; break;
    case ASCII_SPACE: rel = REL_LETTER_WS_SPACE; break;
    case ASCII_HT: rel = REL_LETTER_WS_HTAB; break;
    case ASCII_VT: rel = REL_LETTER_WS_VTAB; break;
    case ASCII_CR: rel = REL_LETTER_EOL_MAC; break;
    case ASCII_LF: rel = REL_LETTER_EOL_UNIX; break;
    case UNICODE_HORIZONTAL_ELLIPSIS: rel = REL_LETTER_ABBREV; break;
    default: break;
    }
    return rel;
}

/* ========================================================================== */
