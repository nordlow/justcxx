/*!
 * \file parallel_merge_sort.hpp
 * \brief Parallel Stable Merge Sort
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-03-20 21:15
 * \origin http://drdobbs.com/high-performance-computing/229301290
 */

#pragma once

template< class T >
inline void parallel_merge_sort_simplest_r( T* src, size_t l, size_t r, T* dst, bool srcToDst = true ) // srcToDst specifies direction for this level of recursion
{
    if ( r == l ) {         // termination/base case of sorting a single element
        if ( srcToDst )  dst[ l ] = src[ l ]; // copy the single element from src to dst
        return;
    }
    size_t m = ( r + l ) / 2;
                                //tbb::parallel_invoke(				// Intel's     Threading Building Blocks (TBB)
    Concurrency::parallel_invoke( // Microsoft's Parallel Pattern Library  (PPL)
        [&] { parallel_merge_sort_simplest_r( src, l,     m, dst, !srcToDst ); }, // reverse direction of srcToDst for the next level of recursion
        [&] { parallel_merge_sort_simplest_r( src, m + 1, r, dst, !srcToDst ); } // reverse direction of srcToDst for the next level of recursion
                                                );
    if ( srcToDst ) merge_parallel_L5( src, l, m, m + 1, r, dst, l );
    else merge_parallel_L5( dst, l, m, m + 1, r, src, l );
}

template< class T >
inline void parallel_merge_sort_simplest( T* src, size_t l, size_t r, T* dst, bool srcToDst = true )	// srcToDst specifies direction for this level of recursion
{
    if ( r < l ) return;
    parallel_merge_sort_simplest_r( src, l, r, dst, srcToDst );
}

template< class T >
inline void parallel_merge_sort_hybrid_rh( T* src, size_t l, size_t r, T* dst, bool srcToDst = true )
{
    if ( r == l ) {    // termination/base case of sorting a single element
        if ( srcToDst )  dst[ l ] = src[ l ];    // copy the single element from src to dst
        return;
    }
    if (( r - l ) <= 48 ) {
        insertionSortSimilarToSTLnoSelfAssignment( src + l, r - l + 1 );		// in both cases sort the src
        //stable_sort( src + l, src + r + 1 );	// STL stable_sort can be used instead, but is slightly slower than Insertion Sort
        if ( srcToDst )	for( size_t i = l; i <= r; i++ )	dst[ i ] = src[ i ];	// copy from src to dst, when the result needs to be in dst
        return;	
    }
    size_t m = ( r + l ) / 2;
    //tbb::parallel_invoke(				// Intel's     Threading Building Blocks (TBB)
    Concurrency::parallel_invoke(		// Microsoft's Parallel Pattern Library  (PPL)
        [&] { parallel_merge_sort_hybrid_rh( src, l,     m, dst, !srcToDst ); },		// reverse direction of srcToDst for the next level of recursion
        [&] { parallel_merge_sort_hybrid_rh( src, m + 1, r, dst, !srcToDst ); }		// reverse direction of srcToDst for the next level of recursion
                                                );
    if ( srcToDst ) merge_parallel_L5( src, l, m, m + 1, r, dst, l );
    else			merge_parallel_L5( dst, l, m, m + 1, r, src, l );
}

template< class T >
inline void parallel_merge_sort( T* src, size_t l, size_t r, T* dst )
{
    parallel_merge_sort_hybrid( src, l, r, dst, true );  // srcToDst = true
}

template< class T >
inline void parallel_merge_sort_pseudo_inplace( T* srcDst, size_t l, size_t r, T* aux )
{
    parallel_merge_sort_hybrid( srcDst, l, r, aux, false );  // srcToDst = false
}

template< class T >
inline void parallel_merge_sort_hybrid_rh_1( T* src, size_t l, size_t r, T* dst, bool srcToDst = true )
{
    if ( r == l ) {         // termination/base case of sorting a single element
        if ( srcToDst )  dst[ l ] = src[ l ]; // copy the single element from src to dst
        return;
    }
    if (( r - l ) <= 48 && !srcToDst ) { // 32 or 64 or larger seem to perform well
        insertionSortSimilarToSTLnoSelfAssignment( src + l, r - l + 1 ); // want to do dstToSrc, can just do it in-place, just sort the src, no need to copy
//stable_sort( src + l, src + r + 1 );	// STL stable_sort can be used instead, but is slightly slower than Insertion Sort
        return;	
    }
    size_t m = (( r + l ) / 2 );
//tbb::parallel_invoke(				// Intel's     Threading Building Blocks (TBB)
    Concurrency::parallel_invoke( // Microsoft's Parallel Pattern Library  (PPL)
        [&] { parallel_merge_sort_hybrid_rh_1( src, l,     m, dst, !srcToDst ); }, // reverse direction of srcToDst for the next level of recursion
        [&] { parallel_merge_sort_hybrid_rh_1( src, m + 1, r, dst, !srcToDst ); } // reverse direction of srcToDst for the next level of recursion
                                                );
    if ( srcToDst ) merge_parallel_L5( src, l, m, m + 1, r, dst, l );
    else merge_parallel_L5( dst, l, m, m + 1, r, src, l );
}
