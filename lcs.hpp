/*! \file lcs.hpp
 * \brief Longest Common Subsequence.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * Finds Longest Common Subsequence. O(m*n) where m and n are the sequence lengths.
 *
 * For example, for the strings "computer" and "houseboat" this algorithm
 * returns a value of 3, specifically the string "out".
 *
 * \see http://www.biotec.icb.ufmg.br/cabi/artigos/seminarios2/subsequence_algorithm.pdf
 * \see http://wordaligned.org/articles/longest-common-subsequence
 * \see https://en.wikipedia.org/wiki/Longest_common_subsequence_problem
 * \see https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Longest_common_substring#C.2B.2B
 * \see http://rosettacode.org/wiki/Longest_common_subsequence
 * \see http://www.algorithmist.com/index.php/Longest_Common_Subsequence
 * \see https://comeoncodeon.wordpress.com/2009/11/13/number-of-distinct-lcs/
 * \see http://puddleofriddles.blogspot.se/2012/03/longest-common-subsequence-problem.html
 *
 * \see levenshtein_distance
 *
 * \todo Generalize to Bidirectional Access Containers.
 */

#pragma once
#include <iterator>
#include <vector>
#include <algorithm>

#ifdef __GNUC__
#    define GCC_VERSION_AT_LEAST(x,y) (__GNUC__ > x || __GNUC__ == x && __GNUC_MINOR__ >= y)
#else
#    define GCC_VERSION_AT_LEAST(x,y) 0
#endif

#define HAVE_C99_VARIABLE_LENGTH_ARRAYS GCC_VERSION_AT_LEAST(4,0)

/*! Longest Common Subsequence between \p x and \p y. */
template<class C> inline pure
size_t lcs(const C& x,
           const C& y)
{
    auto m = x.size(), n = y.size();
#ifdef HAVE_C99_VARIABLE_LENGTH_ARRAYS
    size_t l[m+1][n+1];         // length counters
#else
#endif
    if (m < n) { std::swap(x,y); } // make outer loop the longest
    for (size_t i = 0; i <= m; i++) {
        for (size_t j = 0; j <= n; j++) {
            if (i == 0 or j == 0) {
                l[i][j] = 0;
            } else {
                if (x[i-1] == y[j-1])
                    l[i][j] = l[i-1][j-1] + 1;
                else
                    l[i][j] = std::max(l[i][j-1],
                                       l[i-1][j]);
            }
        }
    }
    return l[m][n];
}

/*!
 * Longest Common Subsequence between \p x and \p y.
 */
template<class C>
inline pure
size_t nlcs(const C& x,
            const C& y)
{
    auto m = x.size(), n = y.size();
#ifdef HAVE_C99_VARIABLE_LENGTH_ARRAYS
    size_t l[m+1][n+1];         // length counters
#else
#endif
    typedef C::value_type V;
    size_t d[m+1][n+1];
    for (size_t i = 0; i <= m; i++) {
        for (size_t j = 0; j <= n; j++) {
            if (i == 0 or j == 0) {
                d[i][j] = 1;
            } else {
                d[i][j] = 0;
                if (x[i-1] == y[j-1]) {
                    d[i][j] = d[i-1][j-1];
                } else {
                    if (l[i-1][j] == l[i][j])
                        d[i][j] = (d[i][j] + d[i-1][j]) % 23102009;
                    if (l[i][j-1] == l[i][j])
                        d[i][j] = (d[i][j] + d[i][j-1]) % 23102009;
                    if (l[i-1][j-1] == l[i][j]) {
                        if (d[i][j] < d[i-1][j-1])
                            d[i][j] += 23102009;
                        d[i][j] = (d[i][j] -
                                   d[i-1][j-1]);
                    }
                }
            }
        }
    }
    return d[m][n];
}

// ----------------------------------------------------------------------------

/* C++ implementation of "A linear space algorithm for computing maximal common
  subsequences" D. S. Hirschberg http://portal.acm.org/citation.cfm?id=360861
*/
typedef std::vector<int> lengths;

/* The "members" type is used as a sparse set for LCS calculations.  Given a
  sequence, xs, and members, m, then if m[i] is true, xs[i] is in the LCS.
*/
typedef std::vector<bool> members;

/*
  Fill the LCS sequence from the members of a sequence, xs
  x - an iterator into the sequence xs
  xs_in_lcs - members of xs
  lcs - an output results iterator
*/
template<class It, class Ot>
void set_lcs(It x, members const & xs_in_lcs, Ot lcs)
{
    for (members::const_iterator xs_in = xs_in_lcs.begin();
         xs_in != xs_in_lcs.end(); ++xs_in, ++x)
    {
        if (*xs_in)
        {
            *lcs++ = *x;
        }
    }
}

/*
  Calculate LCS row lengths given iterator ranges into two sequences.
  On completion, `lens` holds LCS lengths in the final row.
*/
template<class It>
void lcs_lens(It xlo, It xhi, It ylo, It yhi, lengths & lens)
{
    // Two rows of workspace.
    // Careful! We need the 1 for the leftmost column.
    lengths curr(1 + distance(ylo, yhi), 0);
    lengths prev(curr);

    for (It x = xlo; x != xhi; ++x)
    {
        swap(prev, curr);
        int i = 0;
        for (It y = ylo; y != yhi; ++y, ++i)
        {
            curr[i + 1] = *x == *y
                ? prev[i] + 1
                : std::max(curr[i], prev[i + 1]);
        }
    }
    swap(lens, curr);
}

/*
  Recursive LCS calculation.
  See Hirschberg for the theory!
  This is a divide and conquer algorithm.
  In the recursive case, we split the xrange in two.
  Then, by calculating lengths of LCSes from the start and end
  corners of the [xlo, xhi] x [ylo, yhi] grid, we determine where
  the yrange should be split.

  xo is the origin (element 0) of the xs sequence
  xlo, xhi is the range of xs being processed
  ylo, yhi is the range of ys being processed
  Parameter xs_in_lcs holds the members of xs in the LCS.
*/
template<class It>
void
calculate_lcs(It xo, It xlo, It xhi, It ylo, It yhi, members & xs_in_lcs) {
    unsigned const nx = distance(xlo, xhi);

    if (nx == 0) {
        // empty range. all done
    } else if (nx == 1) {
        // single item in x range.
        // If It's in the yrange, mark its position in the LCS
        xs_in_lcs[distance(xo, xlo)] = find(ylo, yhi, *xlo) != yhi;
    } else {
        // split the xrange
        It xmid = xlo + nx / 2;

        // Find LCS lengths at xmid, working from both ends of the range
        lengths ll_b, ll_e;
        std::reverse_iterator<It> hix(xhi), midx(xmid), hiy(yhi), loy(ylo);

        lcs_lens(xlo, xmid, ylo, yhi, ll_b);
        lcs_lens(hix, midx, hiy, loy, ll_e);

        // Find the optimal place to split the y range
        lengths::const_reverse_iterator e = ll_e.rbegin();
        int lmax = -1;
        It y = ylo, ymid = ylo;

        for (lengths::const_iterator b = ll_b.begin();
             b != ll_b.end(); ++b, ++e)
        {
            if (*b + *e > lmax)
            {
                lmax = *b + *e;
                ymid = y;
            }
            // Care needed here!
            // ll_b and ll_e contain one more value than the range [ylo, yhi)
            // As b and e range over dereferenceable values of ll_b and ll_e,
            // y ranges over iterator positions [ylo, yhi] _including_ yhi.
            // That's fine, y is used to split [ylo, yhi), we do not
            // dereference It. However, y cannot go beyond ++yhi.
            if (y != yhi)
            {
                ++y;
            }
        }
        // Split the range and recurse
        calculate_lcs(xo, xlo, xmid, ylo, ymid, xs_in_lcs);
        calculate_lcs(xo, xmid, xhi, ymid, yhi, xs_in_lcs);
    }
}

// Calculate an LCS of (xs, ys), returning the result in an_lcs.
template<class Seq>
void lcs(Seq const & xs, Seq const & ys, Seq & an_lcs) {
    members xs_in_lcs(xs.size(), false);
    calculate_lcs(xs.begin(), xs.begin(), xs.end(),
                  ys.begin(), ys.end(), xs_in_lcs);
    set_lcs(xs.begin(), xs_in_lcs, back_inserter(an_lcs));
}

#include <iostream>
#include <string>
size_t test_lcs()
{
    std::string x,y;
    size_t t,a,b;
    scanf("%d",&t);
    while (t--) {
        cin >> x >> y;
        a = lcs(x,y);
        b = nlcs(x,y);
        printf("%d %d\n",a,b);
    }
}
