#include "timing.h"
#include "stdio_x.h"
#include "csc.hpp"
#include "show.hpp"
#include "distance.hpp"
#include "cyclic_index.hpp"
#include "enforce.hpp"
#include "utf8/utf8.h"

#include <vector>

using std::cout;
using std::endl;

template<class T, class D = size_t>
void levenshtein_show(const T & s,
                      const T & t)
{
    cout << "levenshtein(\""
         << s << "\", \""
         << t << "\"): "
         << pnw::distance::levenshtein<T, D>(s, t)
         << endl;
}

template<class T, class D = size_t>
void damerau_levenshtein_show(const T & s,
                              const T & t)
{
    cout << "damerau_levenshtein(\""
         << s << "\", \""
         << t << "\"): "
         << pnw::distance::damerau_levenshtein<T, D>(s, t)
         << endl;
}

template<class T, class D = size_t>
int test_levenshtein_assert()
{
    const D insert_w = 10;
    const D delete_w = 7;
    const D replace_w = 5;
    const D transposition_w = 3;
    const D max = std::numeric_limits<D>::max();
    using namespace pnw::distance;
    enforce_eq(1, test_levenshtein_symmetry(T("a"), T("ab")));
    enforce_eq(1*insert_w, levenshtein(T("a"),  T("ab"), max, insert_w, delete_w, replace_w));
    enforce_eq(2*replace_w, levenshtein(T("aa"), T("bb"), max, insert_w, delete_w, replace_w)); // 2 substitution/replace
    enforce_eq(2, test_levenshtein_symmetry(T("abcd"), T("ad"))); // 2 deletes
    enforce_eq(3, test_levenshtein_symmetry(T("abcdef"), T("abcxxxdef")));
    enforce_eq(9, test_levenshtein_symmetry(T("abcdef"), T("111abc222def333"))); // 9 inserts
    enforce_eq(1*transposition_w, damerau_levenshtein(T("ab"), T("ba"), max, insert_w, delete_w, replace_w, transposition_w));
    enforce_eq(2*transposition_w, damerau_levenshtein(T("abcd"), T("badc"), max, insert_w, delete_w, replace_w, transposition_w));
    return 0;
}

typedef std::pair<unsigned int, unsigned int> uirange;

template<class T, class D = size_t>
int test_levenshtein_performance(const uirange & n = uirange(1,100))
{
    int ret = 0;
    const D insert_w = 10;
    const D delete_w = 7;
    const D replace_w = 5;
    const D max = std::numeric_limits<D>::max();
    pTimer times[n.second - n.first];
    for (unsigned int i = n.first; i != n.second; i++) {
        auto & t = times[i - n.first];
        ptimer_init_REALTIME(&t);
        ptimer_tic(&t);
        ret += not (pnw::distance::levenshtein(T(i,'a'), T(i,'b'), max, insert_w, delete_w, replace_w) == i*replace_w);
        ptimer_toc(&t);
        ptimer_fprint_sec_usec9(stdout, t); printf("s"); endline();
    }
    cout << __FUNCTION__ << ": " << (ret ? "FAILURE" : "SUCCESS") << endl;
    return ret;
}

template<class T>
void print(const std::vector<T> & a)
{
    cout << a.size() << endl;
}

template<class T, class D = size_t>
int test_levenshtein()
{
    levenshtein_show<T>(T("a"), T("ab"));
    levenshtein_show<T>(T("b"), T("ab"));
    levenshtein_show<T>(T("ab"), T("ba"));
    levenshtein_show<T>(T("yy0123456789zzzzz0123456789yy"),
                        T("0x12x34x56x78x9zzzzz0x12x34x56x78x9"));
    damerau_levenshtein_show<T>(T("ab"), T("ba"));
    typedef T Word;                // \todo Use hashed id a la \c wordID.
    typedef std::vector<Word> Sentence; // sentence is a vector words
    Sentence s = { T("alpha"), T("beta"), T("gamma") };
    Sentence t = { T("alpha"), T("beta"), T("kappa") };
    // damerau_levenshtein_show(s, t);
    return (test_levenshtein_assert<T, D>() +
            test_levenshtein_performance<T, D>(uirange(10000,10001)));
}

template<class D = size_t>
int test_levenshtein_all()
{
    return (test_levenshtein<std::string, D>() +
            test_levenshtein<csc, D>());
}

int main(int argc, char *argv[])
{
    return (test_levenshtein_all<uint32_t>() +
            test_levenshtein_all<uint64_t>());
}
