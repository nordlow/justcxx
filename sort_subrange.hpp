#pragma once

#import <algorithm>

/** Sort sub-range [sub_begin, sub_end] of [begin, end].
 *
 * Describe at https://www.youtube.com/watch?v=0WlJEz2wb8Y&t=2686s
 */
template <typename I>           // I models RandomAccessIterator
void sort_subrange(I begin, I end,
                   I sub_begin, I sub_end)
{
    if (sub_begin == sub_end) { return; }
    if (sub_begin != begin)
    {
        std::nth_element(begin, sub_begin, end);
        ++sub_begin;
    }
    std::partial_sort(sub_begin, sub_begin, end);
}
