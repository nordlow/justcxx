/*! \file match.hpp
 * \brief Various specific string matchings.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <string>
#include <vector>
#include <limits>

/*! Return true if \p str has the extension \p ext.
 *
 * For example return true if str is "file.txt" and ext is "txt".
 */
inline pure
bool extmatch(const std::string & str,
              const std::string & ext)
{
    const size_t n = str.size();
    const size_t ne = ext.size();
    if (n >= 1 + ne) // contains at least a dot plus extension
    {
        return ((str[n - (1 + ne)] == '.') and // has a dot and
                (str.compare(n - ne, ne, ext) == 0)); // the right extension
    } else {
        return false;		// extension to large to fit in string
    }
}

/*! Match the Data \p a with the Pattern b both of length \p n.
 *
 * \param[in] a contains the source data of length \p n.
 * \param[in] b contains the pattern data of length \p n.
 *
 * \param[in] case_w is the weight of case-differences.
 * \param[in] case_w is the weight of character displacements.
 * \param[in] case_w is the weight of missing characters.
 *
 * \return distance measure between data and pattern as a sum of ranks
 */
template<typename T>
inline pure
int fuzzy_match(const T * a, const T * b, int n,
                int case_w = 1,
                int disp_w = 2,
                int miss_w = 4)
{
    int ranksum = 0;
    for (int i = 0; i < n; i++) {
        int hit_cnt = 0;
        int disp_min = n;
        for (int j = 0; j < n; j++) {
            if (a[i] == b[j]) {
                disp_min = std::min(disp_min, std::abs(i - j));
                hit_cnt++;
            }
        }
        int case_min = 0;
        int miss = (hit_cnt == 0);
        int rank = (case_min * case_w +
                    disp_min * disp_w +
                    miss * miss_w);
        ranksum += rank;
    }
    return ranksum;
}

/*!
 * Return the offset in data that gives the best match with
 * pattern \p patt along with other ranks as pair.
 *
 * Used in applications where fuzzy (humane) pattern-searching is desired.
 * Can also be used to detect anagrams in vocabularies.
 *
 * \return the offset, or data_num if pattern was larger than data.
 */
template<typename T>
inline pure
std::pair<int,
          std::vector<size_t>> fuzzy_find_best_match(const T * data,
                                                     size_t data_num,
                                                     const T * patt,
                                                     size_t patt_num)
{
    size_t best_i = data_num; // indicate error
    std::vector<size_t> ranks;  // match ranks
    int min_rank = std::numeric_limits<int>::max();
    const int thresh = (int)patt_num * patt_num;
    if (data_num >= patt_num) {
        size_t try_num = data_num - patt_num;
        ranks.resize(try_num);
        for (size_t i = 0; i < try_num; i++) {
            int rank = fuzzy_match(data + i, patt, patt_num);
            ranks[i] = rank;
            if (rank < thresh) {
                //cout << rank / (patt_num * patt_num / 2);
            } else {
                //cout << " ";
            }
            if (min_rank > rank) {
                min_rank = rank;
                best_i = i;
            }
        }
    } else {
        ranks.resize(0);
    }
    //cout << endl;
    return std::make_pair(best_i, ranks);
}
