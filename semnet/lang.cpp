#include "lang.hpp"
#include "seq.hpp"
#include "file.hpp"
#include <boost/algorithm/string.hpp>

namespace semnet { namespace patterns {

virtual std::ostream& Lang::show(std::ostream& os) const {
    Wrap::show(os);
    os << " language: " << get_pathL();
    Base::show_rand(os);
    return os;
}

Lang::Lang(const csc name, Base * sub) : Wrap(name, sub)
{
    // Function arguments
    const char * locale_name = "sv_SE.utf8"; // default is either nullptr or ""

    std::locale loc0(locale_name ? locale_name : ""); // Any UTF-8 locale would do I guess

    typedef std::codecvt<wchar_t, char, std::mbstate_t> utf8_codecvt_t; // conversion type
    const utf8_codecvt_t& cvt = std::use_facet<utf8_codecvt_t>(loc0); // conversion
    std::locale loc(loc0, &cvt); // decoding locale

    csc dict_code;          // dictionary code
    if (dict_code.empty()) {
        // pick at most first 3 bytes of language name in lowercase
        dict_code = boost::algorithm::to_lower_copy(name.substr(0, std::min<size_t>(name.length(), 3)));
    }
    // csc bitsn = "";
    // auto path = csc("/usr/share/dictd/freedict-") + dict_code + bitsn + ".index";
    // auto file = filesystem::File::load_path(path); // index file

    // TODO: Move to some semnet::filesystem::File:: member-function open_stream()
    // TODO: - DECODE_UNZIP: use http://www.boost.org/doc/libs/1_49_0/libs/iostreams/doc/index.html
    // Bring along \see http://stackoverflow.com/questions/7889032/utf-8-compliant-iostreams
    // std::wifstream(path) ifs;
    // ifs.imbue(loc); // move to a new locale => convert UTF-8 to UCS-4

    // Parse .index
    // 1. skip 6 lines

    // Open contents of .dz by finding by finding contents of CompFile.
}

}}
