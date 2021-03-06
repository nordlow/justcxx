#include "OB_enum.hpp"
#include "string_x.h"

namespace semnet {

OB_t g_OB_any_ = OB_any_;

/* ---------------------------- Group Separator ---------------------------- */

bool
OB_matchS(OB_t oT, OB_t oM1)
{
    bool match = false;
    if (oM1 == OB_any_ or /* any relation (short-circuit) */
        oT == oM1) {	/* a specific type of relation */
        match = true;
    } else {
        switch (oM1) {
        case OB_FILE_: if (OB_is_FILE(oT)) { match = true; } break;
        case OB_PATT_: if (OB_is_PATT(oT)) { match = true; } break;
        default: break;
        }
    }
    return match;
}

bool
OB_matchM(OB_t oT, const OB_t * oM, size_t oM_N)
{
    bool match = false;
    if (oM == 0 or oM_N == 0) { // short-circuit
        match = 1;
    } else if (oM_N == 1) {
        match = OB_matchS(oT, oM[0]);
    } else {
        for (size_t i = 0; i < oM_N; i++) {
            if (OB_matchS(oT, oM[i])) { match = true; break; }
        }
    }
    return match;
}

/* ---------------------------- Group Separator ---------------------------- */

const char * OB_getName(OB_t oT)
{
    const char * str = nullptr;

    switch (oT) {
    case OB_any_: str = "ob___"; break;

    case OB_URI: str = "URI"; break;
    case OB_PATH: str = "Path"; break;
    case OB_DEB: str = "Deb"; break;
    case OB_APTDIR: str = "APTDir"; break;
    case OB_APTFILE: str = "APTFile"; break;
    case OB_COMPFILE: str = "COMPFile"; break;

    case OB_DIR: str = "Dir"; break;
    case OB_FILE_: str = "File"; break;
    case OB_REALFILE: str = "RegFile"; break;
    case OB_SYMLINKFILE: str = "SymLinkFile"; break;
    case OB_ELFFILE: str = "ELFFile"; break;

    case OB_FILETYPE: str = "FileType"; break;
    case OB_DIRTYPE: str = "DirType"; break;

    case OB_ELFSYMTAB: str = "ELFSymTab"; break;
    case OB_ELFSYM: str = "ELFSym"; break;

    case OB_FILEATTR: str = "FileAttr"; break;
    case OB_DUP: str = "Dup"; break;

    case OB_PATT_: str = "Base"; break;
    case OB_ANY: str = "Any"; break;
    case OB_BIT: str = "Bit"; break;
    case OB_LIT: str  = "Lit"; break;
    case OB_ALT: str  = "Alt"; break;
    case OB_COMB: str  = "Seq"; break;
    case OB_REP: str  = "Rep"; break;
    case OB_SIT: str  = "Sit"; break;
    case OB_REQ: str  = "Req"; break;

    case OB_HIT: str = "Hit"; break;
    case OB_SCAN: str = "Scan"; break;
    case OB_LOP: str = "LOp"; break;
    case OB_GOP: str = "GOp"; break;
    case OB_FOP: str = "FOp"; break;
    case OB_NUMBER: str = "Number"; break;

        // Visual Objects

    case OB_VSYM:  str = "vSym"; break;
    case OB_VGLYPH:  str = "vGlyph"; break;
    case OB_VTEX:  str = "vTex"; break;
    case OB_VVIS:  str = "vVis"; break;
    case OB_VTXT:  str = "vTxt"; break;

    case OB_VDIR:  str = "vDir"; break;
    case OB_VFILE:  str = "vFile"; break;
    case OB_VPERM: str = "vPerm"; break;

    case OB_UNIT: str = "Unit"; break;

    case OB_VBIT:  str = "vBit"; break;
    case OB_VNUMZ:  str = "vNumZ"; break;

    case OB_VSET:  str = "vSet"; break;

    case OB_VOP:  str = "vOp"; break;
    case OB_VAPP:  str = "vApp"; break;

    case OB_VBUTT: str = "vButt"; break;
    case OB_VPBAR: str = "vPBar"; break;

    case OB_VTIMER: str = "vTimer"; break;
    case OB_VTIME: str = "vTime"; break;
    case OB_VDATE: str = "vDate"; break;
    case OB_VCLOCK: str = "vClock"; break;

    case OB_undefined_: str = "__undefined"; break;
    }

    return str;
}

OB_t
OB_lookup(const char * str, bool case_flag)
{
    OB_t oT = OB_undefined_;
    char * stri = nullptr;         // case-folded version of \p str
    if (not case_flag) {
        stri = strdup(str); strtolower(stri);
    } else {
        stri = (char*)str;		/* we won't change it! */
    }
    if      (strcmp(stri, "vsym") == 0) { oT = OB_VSYM; }
    else if (strcmp(stri, "vglyph") == 0) { oT = OB_VGLYPH; }
    else if (strcmp(stri, "vtex") == 0) { oT = OB_VTEX; }
    else if (strcmp(stri, "vvis") == 0) { oT = OB_VVIS; }
    else if (strcmp(stri, "vtxt") == 0) { oT = OB_VTXT; }
    else if (strcmp(stri, "vdir") == 0) { oT = OB_VDIR; }
    else if (strcmp(stri, "vfile") == 0) { oT = OB_VFILE; }
    else if (strcmp(stri, "vset") == 0) { oT = OB_VSET; }
    else if (strcmp(stri, "vbit") == 0) { oT = OB_VBIT; }
    else if (strcmp(stri, "vnumz") == 0) { oT = OB_VNUMZ; }
    else if (strcmp(stri, "vop") == 0) { oT = OB_VOP; }
    else if (strcmp(stri, "vapp") == 0) { oT = OB_VAPP; }
    else if (strcmp(stri, "vbutt") == 0) { oT = OB_VBUTT; }
    else if (strcmp(stri, "vpbar") == 0) { oT = OB_VPBAR; }
    else if (strcmp(stri, "vtimer") == 0) { oT = OB_VTIMER; }
    else if (strcmp(stri, "vtime") == 0) { oT = OB_VTIME; }
    else if (strcmp(stri, "vdate") == 0) { oT = OB_VDATE; }
    else if (strcmp(stri, "vclock") == 0) { oT = OB_VCLOCK; }
    else if (strcmp(stri, "vperm") == 0) { oT = OB_VPERM; }
    else {                                oT = OB_undefined_; }
    if (not case_flag) { free(stri); }
    return oT;
}

int
OB_completeName(char * str, pMode pmm,
                 int results_ret[OB_NUM],
                 uint match_idxB_ret[OB_NUM])
{
    int ret = 0;

    const bool case_flag = not pmm.ci();
    char * stri = nullptr;
    if (case_flag) {
        stri = (char*)str;		/* we won't change it! */
    } else {
        stri = strdup(str); strtolower(stri);
    }

    const size_t len = strlen(stri);
    for (OB_t oT = OB_FIRST; oT != OB_NUM; oT = static_cast<OB_t>(oT + 1)) {
        results_ret[oT] = 0;
        match_idxB_ret[oT] = 0;
        switch (pmm.get_type()) {
        case MATCH_PREFIX:
            if (::strncmp(OB_getName(oT), str, len) == 0) {
                results_ret[oT] = +1;
                match_idxB_ret[oT] = 0;
                ret++;
            }
            break;
        default:
            PTODO("Handle pmm.mT:%d\n", pmm.get_type());
            break;
        }
    }

    if (not case_flag) { free(stri); }

    return ret;
}

}
