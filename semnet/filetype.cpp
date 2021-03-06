#include "filetype.hpp"
#include "patt.hpp"
#include "know_dfmts.hpp"
#include "alt.hpp"
#include "ctx.hpp"
#include "regfile.hpp"

namespace semnet { namespace filesystem {

FileType::Reg FileType::ms_reg;

FileType::FileType(patterns::Base * nfmt,
                   patterns::Base * cfmt,
                   const char * name,
                   const char * doc,
                   DFMT_t dfmt,
                   PRECOG_t precog)
    : m_nfmt(nfmt ? patterns::gen::ctx(patterns::PCTX_IN_FILE_NAME, nfmt) : nullptr),
      m_cfmt(cfmt ? patterns::gen::ctx(patterns::PCTX_IN_FILE_NAME, cfmt) : nullptr),
      m_name(name ? name : ""),
      m_doc(doc ? doc : ""),
      m_dfmt(dfmt),
      m_precog(precog)
{
    assert(nfmt or cfmt);
    if (not nfmt) { m_precog = PRECOG_CONTENTS; }
    if (not cfmt) { m_precog = PRECOG_NAME; }
    ms_reg.insert(this);
}

bool
FileType::match(RegFile* file, bir roi, PRECOG_t override_precog) const
{
    auto precog = override_precog != PRECOG_any_ ? override_precog : m_precog;
    switch (precog) {
    case PRECOG_NAME:
        return m_nfmt->match_name_of(file);
        break;
    case PRECOG_CONTENTS:
        return m_cfmt->match_contents_of(file, roi);
        break;
    case PRECOG_NAME_AND_CONTENTS:
        return (m_nfmt->match_name_of(file) and
                m_cfmt->match_contents_of(file, roi));
        break;
    case PRECOG_NAME_OR_CONTENTS:
        return (m_nfmt->match_name_of(file) or
                m_cfmt->match_contents_of(file, roi));
        break;
    default:
        return false;
        break;
    }
}

FileType *
FileType::of(RegFile* file, bir roi, PRECOG_t override_precog)
{
    for (auto ftype : ms_reg) {
        if (ftype->match(file, roi, override_precog)) {
            return ftype;         // return first hit
        }
    }
    return nullptr;
}

}}
