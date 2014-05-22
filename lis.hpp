/*!
 * \file lis.hpp
 * \brief Longest Increasing Subsequence.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-04-05 23:16
 *
 * Finds longest strictly increasing subsequence. O(n*log(k)).
 *
 * \see http://www.algorithmist.com/index.php/Longest_Increasing_Subsequence.cpp
 * \see http://stackoverflow.com/questions/3992697/longest-increasing-subsequence
 */

#pragma once

#include <vector>

/* Finds Longest Strictly Increasing Subsequence of \p a. O(n log k) algorithm. */
template<typename C>
inline pure
C lis(const C& a)
{
    typedef C::value_type V;
	C p(a.size());
	int u, v;

    C b;                        // result
	if (a.empty()) return b;
	b.push_back(0);

	for (size_t i = 1; i < a.size(); i++) {
        // If next element a[i] is greater than last element of current longest
        // subsequence a[b.back()], just push it at back of "b" and continue
		if (a[b.back()] < a[i])
        {
			p[i] = b.back();
			b.push_back(i);
			continue;
		}

        // Binary search to find the smallest element referenced by b which is
        // just bigger than a[i] Note : Binary search is performed on b (and not
        // a). Size of b is always <=k and hence contributes O(log k) to
        // complexity.
		for (u = 0, v = b.size()-1; u < v;) {
			int c = (u + v) / 2;
			if (a[b[c]] < a[i]) u=c+1; else v=c;
		}

        // Update b if new value is smaller then previously referenced value
		if (a[i] < a[b[u]]) {
			if (u > 0) p[i] = b[u-1];
			b[u] = i;
		}
	}
	for (u = b.size(), v = b.back(); u--; v = p[v]) b[u] = v;
    return b;
}

#include <cstdio>
template<typename C>
int test_lis()
{
	int a[] = { 1, 9, 3, 8, 11, 4, 5, 6, 4, 19, 7, 1, 7 };
	C seq(a, a+sizeof(a)/sizeof(a[0])); // seq : Input Vector
	C lis;                              // lis : Vector containing indexes of longest subsequence
    lis(seq, lis);
    //Printing actual output
	for (size_t i = 0; i < lis.size(); i++)
		printf("%d ", seq[lis[i]]);
    printf("\n");
	return 0;
}
