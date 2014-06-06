#include "compfile.hpp"
#include "know_dfmts.hpp"
#include "patt.hpp"
// #include <boost/iostreams/filtering_streambuf.hpp>
// #include <boost/iostreams/copy.hpp>
// #include <boost/iostreams/filter/gzip.hpp>
// #include <boost/iostreams/filter/bzip2.hpp>

namespace semnet {
namespace filesystem {

// inline
// CompFile * make_CompFile(int fd)
// {
//     using namespace patterns;
//     if (gp_gz_dfmt->match_contents_of(fd)) {
//         return new GZFile(fd);
//     } else if (gp_bz_dfmt->match_contents_of(fd)) {
//         return new BZFile(fd);
//     } else if (gp_xz_dfmt->match_contents_of(fd)) {
//         return new XZFile(fd);
//     } else if (gp_lzma_dfmt->match_contents_of(fd)) {
//         return new LZMAFile(fd);
//     } else {
//         return nullptr;
//     }
// }

}
}
