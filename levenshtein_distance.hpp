/*! \file levenshtein_distance.hpp
 * \brief Levenshtein and Damerau-Levenshtein Distances.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * \see directory \c ternary_tree Levensthein Search
 * \see lis, lcs
 *
 * \todo Generalize to Bidirectional Access Containers.
 */

#pragma once
#include <algorithm>
#include "algorithm_x.hpp"
#include <limits>
#include "permn.hpp"
#include <type_traits> // <boost/type_traits/is_pointer.hpp>, <boost/integer_traits.hpp>
#include "cc_features.h"

namespace pnw { namespace distance {

template<class D = size_t>
inline pure
D damerau_levenshtein(char s, char t,
                      D max_distance = std::numeric_limits<D>::max(),
                      D insert_weight = static_cast<D>(10),
                      D delete_weight = static_cast<D>(7),
                      D replace_weight = static_cast<D>(5),
                      D transposition_weight = static_cast<D>(3))
{
    return s == t;
}

template<class D = size_t>
inline pure
D damerau_levenshtein(wchar_t s, wchar_t t,
                      D max_distance = std::numeric_limits<D>::max(),
                      D insert_weight = static_cast<D>(10),
                      D delete_weight = static_cast<D>(7),
                      D replace_weight = static_cast<D>(5),
                      D transposition_weight = static_cast<D>(3))
{
    return s == t;
}

/*! Get \em Damerau-Levenshtein (Edit) Distance (LD) metric between the \em sequences \p s and \p t.
 *
 * \p s and \p t are sequences of typically either
 * - letters (characters) \c char, \c wchar_t -
 * - words or char wchar_t
 * - sentences or functions
 * - lexical tokens (words and interpuncutationss) (in either human or programming languages)
 *
 * The \em Levenshtein distance is defined as the minimum number of \em edits needed
 * to transform one \p s to \p t, with the allowable edit operations being \em
 * insertion, \em deletion or \em substitution (\em replacement) of a single
 * character.
 *
 * When specified we optionally allow \em transposition of two \em adjacent
 * characters (when \p transposition_weight is non-zero) aswell which then
 * instead computes the \em Damerau–Levenshtein distance.
 *
 * One of the strings \p s and \p t is typically short, while the other is
 * arbitrarily long.
 *
 * Typically used int
 * - \em Spell checking.
 * - \em Speech recognition.
 * - \em DNA analysis.
 * - \em Plagiarism detection.
 * - \em Similarity Measure between sequences such as strings and (cellular) network encodings.
 * - \em Productivity Measuring especially in programming by studying parse tree of tokens.
 *
 * \see http://www.levenshtein.net/
 * \see https://secure.wikimedia.org/wikibooks/en/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C.2B.2B
 * \see https://secure.wikimedia.org/wikipedia/en/wiki/Damerau%E2%80%93Levenshtein_distance
 * \see http://stackoverflow.com/questions/3183149/most-efficient-way-to-calculate-levenshtein-distance
 * \see http://stevehanov.ca/blog/index.php?id=114
 *
 * \tparam T is typically a \em string type and generally a random access container.
 * \tparam D Distance Precision.
 * \param s_ First (String) Argument.
 * \param t_ Second (String) Argument.
 *
 * \application Propose grouping (folder) names for of multiple selections of files and
 * suggest to create (if it doesn't exit) and move files to that folder.
 *
 * Defined recursive calls to \c damerau_levenshtein_distance inside
 * body and overload damerau_levenshtein_distance for \© char and \c wchar_t
 *
 * \todo Make this work for UTF-8/16 arrays, like Alexandrescou mentions in his
 * Google Tech Talk on the topic \em "Three Cool Things About D".
 */
template<class T, class D = size_t>
inline pure
D damerau_levenshtein(const T& s_, const T& t_,
                      D max_distance = std::numeric_limits<D>::max(),
                      D insert_weight = static_cast<D>(10),
                      D delete_weight = static_cast<D>(7),
                      D replace_weight = static_cast<D>(5),
                      D transposition_weight = static_cast<D>(3))
{
    // reorder s and t to minimize memory usage
    bool ook = s_.size() >= t_.size(); // argument ordering ok flag
    const T& s = ook ? s_ : t_; // assure \c s becomes the \em longest
    const T& t = ook ? t_ : s_; // assure \c t becomes the \em shortest

    const D m = s.size();
    const D n = t.size();

    if (m == 0) { return n; }
    if (n == 0) { return m; }

    // Adapt the algorithm to use less space, O(3*min(n,m)) instead of O(mn),
    // since it only requires that the previous row/column and current row/column be stored at
    // any one time.
#ifdef HAVE_C99_VARIABLE_LENGTH_ARRAYS
    D cc_[n+1], pc_[n+1], sc_[n+1]; // current, previous and second-previous column on stack
#elif HAVE_CXX11_UNIQUE_PTR
    std::unique_ptr<D[]> cc_(new D[n+1]);      // current column
    std::unique_ptr<D[]> pc_(new D[n+1]);      // previous column
    std::unique_ptr<D[]> sc_(new D[n+1]);      // second previous column
#else
    auto cc_ = new D[n+1];      // current column
    auto pc_ = new D[n+1];      // previous column
    auto sc_ = new D[n+1];      // second previous column
    //std::vector<D> cc_(n+1), pc_(n+1), sc_(n+1); // current, previous and second previous column
#endif
    D * cc = &cc_[0], * pc = &pc_[0], * sc = &sc_[0]; // pointers for efficient swapping

    // initialize previous column
    for (D i = 0; i < n+1; ++i) { pc[i] = i * insert_weight; }

    // second previous column \c sc will be defined in second \c i iteration in outer-loop

    const auto D_max = std::numeric_limits<D>::max();

    // Computing the Levenshtein distance is based on the observation that if we
    // reserve a matrix to hold the Levenshtein distances between all prefixes
    // of the first string and all prefixes of the second, then we can compute
    // the values in the matrix by flood filling the matrix, and thus find the
    // distance between the two full strings as the last value computed.
    // This algorithm, an example of bottom-up dynamic programming, is
    // discussed, with variants, in the 1974 article The String-to-string
    // correction problem by Robert A. Wagner and Michael J.Fischer.
    for (D i = 0; i < m; ++i) {
        cc[0] = i+insert_weight;
        auto tmin = D_max; // row/column total min
        for (D j = 0; j < n; ++j) {
            // TODO Use sub_dist
            //auto sub_dist = damerau_levenshtein(s[i], t[j]); // recurse if for example T is an std::vector<std::string>
            cc[j+1] = pnw::min(pc[j+1] + insert_weight,         // insertion
                               cc[j] + delete_weight,           // deletion
                               pc[j] + (s[i] == t[j] ? 0 : replace_weight)); // substitution

            // transposition
            if (not is_max(transposition_weight)) { // if transposition should be allowed
                if (i > 0 and j > 0 and // we need at least two characters
                    s[i-1] == t[j] and  // and first must be equal second
                    s[i]   == t[j-1]    // and vice versa
                    ) {
                    cc[j+1] = std::min(cc[j+1],
                                       sc[j-1] + transposition_weight);
                }
            }

            if (not is_max(max_distance)) {
                tmin = std::min(tmin, cc[j+1]);
            }
        }

        if ((not is_max(max_distance)) and
            tmin >= max_distance) {
            // if no element is smaller than \p max_distance
            return max_distance;
        }

        if (transposition_weight) {
            perm3_231(pc, cc, sc); // rotate pointers
        } else {
            std::swap(cc, pc);
        }
    }

#if !(defined(HAVE_C99_VARIABLE_LENGTH_ARRAYS) || defined(HAVE_CXX11_UNIQUE_PTR))
    delete [] cc_;
    delete [] pc_;
    delete [] sc_;
#endif
    return pc[n];
}

/*! Get \em Levenshtein (Edit) Distance (LD) metric between the \em sequences \p s and \p t.
 * Computing LD is also called Optimal String Alignment (OSA).
 */
template<class T, class D = size_t>
inline pure
D levenshtein(const T& s, const T& t,
              D max_distance = std::numeric_limits<D>::max(),
              D insert_weight = static_cast<D>(10),
              D delete_weight = static_cast<D>(7),
              D replace_weight = static_cast<D>(5))
{
    return damerau_levenshtein(s, t, max_distance, insert_weight, delete_weight, replace_weight,
                               std::numeric_limits<D>::max());
}

/*! Get \em Levenshtein (Edit) Distance (LD) metric between the \em arrays \p s and \p t.
 * Computing LD is also called Optimal String Alignment (OSA).
 */
template<class D = size_t>
inline pure
D levenshtein(const char * s, const char * t,
              D max_distance = std::numeric_limits<D>::max(),
              D insert_weight = static_cast<D>(10),
              D delete_weight = static_cast<D>(7),
              D replace_weight = static_cast<D>(5))
{
    return levenshtein(csc(s),
                       csc(t),
                       max_distance, insert_weight, delete_weight, replace_weight);
}

/* ---------------------------- Group Separator ---------------------------- */

template<class T, class D = size_t>
inline pure
D test_levenshtein_symmetry(const T& s, const T& t,
                            D max_distance = std::numeric_limits<D>::max())
{
    D st = levenshtein(s, t, max_distance, static_cast<D>(1),static_cast<D>(1),static_cast<D>(1));
    D ts = levenshtein(t, s, max_distance, static_cast<D>(1),static_cast<D>(1),static_cast<D>(1));
    bool sym = (st == ts); // symmetry
    return sym ? st : std::numeric_limits<D>::max();
}

}
}
