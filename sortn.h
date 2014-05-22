/*! \file sortn.h
 * \brief Inline Sorting of \em fixed number of explicit l-value arguments.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \license: GPL
 *
 * Macro calls in Network-Sorts on the same \em row can be executed in
 * \em parallel (useful if we implement in \em hardware using VHDL).
 *
 * This ASCII top-to-bottom \em flowchart illustrates how to
 * think/visualize for a 4-element \em Network-Sort. Network sorts are
 * suitable for hardware-implementations of sorting.
 *
 * \verbatim
 *  a  b  c  d
 *  \  /  \  /
 *   S2    S2
 *  /  \  /  \
 *  #  #  #  #
 *  \   ><   /
 *   S2    S2
 *  /  \  /  \
 * [1] #  # [4]
 *     \  /
 *      S2
 *     /  \
 *    [2][3]
 * \endverbatim
 *
 * which translated to pseudo-code becomes
 * 1. S2(a,b) S2(c,d)
 * 2. S2(a,c) S2(b,d) (\em first [1] is now in \em a and \em fourth [4] is now in \em d)
 * 3. S2(b,c) (\em second [2] is now in \em b and \em third [3] is now in \em c)
 *
 * Where S2 is a two-element inline sort.
 *
 * This algorithm can be executed in parallel in 3 S2-steps which is
 * an important measure when we implement this in parallel (hardware)
 * algorithms typically using VHDL or Verilog. Modern \em pipelined
 * CPU with many \em registers will also benefit from this code
 * optimization.
 *
 * \see http://en.wikipedia.org/wiki/In-place_algorithm
 * \see http://en.wikipedia.org/wiki/Sorting_network
 * \see http://groups.google.com/group/comp.programming/browse_thread/thread/bca6afa393bb74ce
 * \see http://en.wikipedia.org/wiki/Batcher_odd-even_mergesort
 * \see http://www.cs.uky.edu/~lewis/essays/algorithms/sortnets/sort-net.html
 */

#pragma once
#include "perm.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \em In-Place-Swap the contents of \p a and \p b. */
#define SWAP(a, b) ({ typeof(a) tmp = (a); (a) = (b); (b) = tmp; })

/*!
 * \em In-Place-Swap the contents of \p a and \p b using \em Bitwise \em XOr.
 *
 * Gives worse performance than standard \c SWAP() because we need to
 * chack that \p a != \p b.
 *
 * \see http://en.wikipedia.org/wiki/XOR_swap_algorithm
 *
 * Note: Only works on \em fixed-point (integer) types having the xor
 * operator, not on floating point types. Of course a float-pointer
 * can always be \em reinterpret-casted to an uint32_t-pointer and
 * then be xor-swapped. In C, however, this forces us to use a @em
 * separately named macro which is bad. In C++ we solve this problem
 * easily with function \em overloading.
 */
#define XOR_SWAP(a, b) { if (a != b) { (a) ^= (b); (b) ^= (a); (a) ^= (b); } }

/*! Swap the contents of \p a and \p b. */
static inline void ptr_swap(void ** a, void ** b) { SWAP(*a, *b); }

/*! Swap the contents of \p a and \p b. */
static inline void ui_swap(uint * a, uint * b) { SWAP(*a, *b); }

/*! Swap the contents of \p a and \p b. */
static inline void int32_swap(uint32_t * a, uint32_t * b) { SWAP(*a, *b); }

/*! Swap the contents of \p a and \p b. */
static inline void float_swap(float *a, float *b) { int32_swap((uint32_t *) a, (uint32_t *) b); }

/*! Swap the contents of \p a and \p b. */
static inline void int64_swap(uint64_t * a, uint64_t * b) { SWAP(*a, *b); }

/*! Swap the contents of \p a and \p b. */
static inline void size_swap(size_t *a, size_t *b) { SWAP(*a, *b); }

/*! Swap the contents of \p a and \p b. */
static inline void int_swap(int *a, int *b) { SWAP(*a, *b); }

/*! Swap the contents of \p a and \p b. */
static inline void double_swap(double *a, double *b) { int64_swap((uint64_t *) a, (uint64_t *) b); }

/* ========================================================================= */

/*!
 * Calculate the the median of the three arguments \p a, \p b, and \p c.
 */
#define MEDIAN3(a, b, c)                        \
  (a < b ?                                      \
   (b < c ? b : a < c ? c : a) :                \
   (b > c ? b : a > c ? c : a))

/* ========================================================================= */

/*!
 * Parallel-Assign-3 \p a \p b \p c from \p k \p l \p m.
 */
#define PASSIGN3(a,b,c, k,l,m) {                \
    typeof(a) ta = k;                           \
    typeof(b) tb = l;                           \
    typeof(c) tc = m;                           \
    a = ta;                                     \
    b = tb;                                     \
    c = tc;                                     \
  }

/*!
 * Parallel-Assign-4 \p a \p b \p c \p d from \p k \p l \p m \p n.
 */
#define PASSIGN4(a,b,c,d, k,l,m,n) {             \
    typeof(a) ta = k;                            \
    typeof(b) tb = l;                            \
    typeof(c) tc = m;                            \
    typeof(d) td = n;                            \
    a = ta;                                      \
    b = tb;                                      \
    c = tc;                                      \
    d = td;                                      \
  }

/* ========================================================================= */

/*!
 * In-Place-Sort-2 \p a and \p b.
 * \complexity[time] 1 CMP, [0 or 2] MOV
 * \complexity[code]: 1 CMP, 3 MOV
 */
#define IP_SORT2(a,b) { if (a > b) SWAP(a,b); }

/*! Standard-Sort-2 \p a and \p b. */
#define SORT2(a,b) IP_SORT2(a,b)

/* ========================================================================= */

/*!
 * \em Assign-Sort-3 \p a, \p b and \p c into \p k, \p l and \p m.
 * \complexity[time] (\em Small!): 3 CMP, 3 MOV
 * \complexity[code] (\em Large!): 5 CMP, 6*3 MOV
 */
#define ASORT3(a,b,c, k,l,m) {                  \
    if (b < c)                                  \
      if (a < c)                                \
        if (a < b)                              \
        { k=a; l=b; m=c; }                      \
        else                                    \
        { k=b; l=a; m=c; }                      \
      else                                      \
      { k=b; l=c; m=a; }                        \
    else                                        \
      if (a < b)                                \
        if (a < c)                              \
        { k=a; l=c; m=b; }                      \
        else                                    \
        { k=c; l=a; m=b; }                      \
      else                                      \
      { k=c; l=b; m=a; }                        \
  }

/*!
 * In-Place-Sort-3 \p a, \p b and \p c.
 * \complexity[time] 3 CMP, 3 MOV
 * \complexity[code]: 3 CMP, 3 MOV
 */
#define IP_SORT3(a,b,c) {                        \
    if (b < c)                                  \
      if (a < c)                                \
        if (a < b)                              \
          ;                                     \
        else                                    \
        { SWAP(a,b); }                          \
      else                                      \
      { PERM3_231(a,b,c); }                     \
    else                                        \
      if (a < b)                                \
        if (a < c)                              \
        { SWAP(b,c); }                          \
        else                                    \
        { PERM3_312(a,b,c); }                   \
      else                                      \
      { SWAP(a,c); }                           \
  }

/*! Standard-Sort-3 \p a, \p b and \p c. */
#define SORT3(a,b,c) {                          \
    typeof(a) s;                                \
    typeof(b) t;                                \
    typeof(c) u;                                \
    ASORT3(a,b,c, s,t,u);                       \
    a=s;                                        \
    b=t;                                        \
    c=u;                                        \
  }

/*! \em Network-Sort-3 \p a, \p b and \p c. */
#define NSORT3(a,b,c) {                         \
    SORT2(a,b);                                 \
    SORT2(b,c);                                 \
    SORT2(a,b);                                 \
  }

#undef SORT3
#define SORT3(a,b,c) IP_SORT3(a,b,c)

/* ========================================================================= */

/*!
 * Assign-Sort-4 \p a, \p b, \p c and \p d into \p k, \p l, \p m and \p n .
 * \complexity[time] (\em Small!): 5 CMP, 4 MOV
 * \complexity[code] (\em Large!): 23 CMP, 24*4 MOV
 */
#define ASORT4(a,b,c,d, k,l,m,n) {              \
    if (c < d)                                  \
      if (b < d)                                \
        if (b < c)                              \
          if (a < c)                            \
            if (a < b)                          \
            { k=a; l=b; m=c; n=d; }             \
            else                                \
            { k=b; l=a; m=c; n=d; }             \
          else                                  \
            if (a < d)                          \
            { k=b; l=c; m=a; n=d; }             \
            else                                \
            { k=b; l=c; m=d; n=a; }             \
        else                                    \
          if (a < b)                            \
            if (a < c)                          \
            { k=a; l=c; m=b; n=d; }             \
            else                                \
            { k=c; l=a; m=b; n=d; }             \
          else                                  \
            if (a < d)                          \
            { k=c; l=b; m=a; n=d; }             \
            else                                \
            { k=c; l=b; m=d; n=a; }             \
      else                                      \
        if (a < d)                              \
          if (a < c)                            \
          { k=a; l=c; m=d; n=b; }               \
          else                                  \
          { k=c; l=a; m=d; n=b; }               \
        else                                    \
          if (a < b)                            \
          { k=c; l=d; m=a; n=b; }               \
          else                                  \
          { k=c; l=d; m=b; n=a; }               \
    else                                        \
      if (b < c)                                \
        if (b < d)                              \
          if (a < d)                            \
            if (a < b)                          \
            { k=a; l=b; m=d; n=c; }             \
            else                                \
            { k=b; l=a; m=d; n=c; }             \
          else                                  \
            if (a < c)                          \
            { k=b; l=d; m=a; n=c; }             \
            else                                \
            { k=b; l=d; m=c; n=a; }             \
        else                                    \
          if (a < b)                            \
            if (a < d)                          \
            { k=a; l=d; m=b; n=c; }             \
            else                                \
            { k=d; l=a; m=b; n=c; }             \
          else                                  \
            if (a < c)                          \
            { k=d; l=b; m=a; n=c; }             \
            else                                \
            { k=d; l=b; m=c; n=a; }             \
      else                                      \
        if (a < c)                              \
          if (a < d)                            \
          { k=a; l=d; m=c; n=b; }               \
          else                                  \
          { k=d; l=a; m=c; n=b; }               \
        else                                    \
          if (a < b)                            \
          { k=d; l=c; m=a; n=b; }               \
          else                                  \
          { k=d; l=c; m=b; n=a; }               \
  }

/*!
 * In-Place-Sort-4 \p a, \p b, \p c and \p d.
 */
#define IP_SORT4(a,b,c,d) {                      \
    if (c < d)                                  \
      if (b < d)                                \
        if (b < c)                              \
          if (a < c)                            \
            if (a < b)                          \
            { /* already sorted */ }            \
            else                                \
            { SWAP(a,b); }                      \
          else                                  \
            if (a < d)                          \
            { PERM3_231(a,b,c); }               \
            else                                \
            { PERM4_2341(a,b,c,d); }            \
        else                                    \
          if (a < b)                            \
            if (a < c)                          \
            { SWAP(b,c); }                      \
            else                                \
            { PERM3_312(a,b,c); }               \
          else                                  \
            if (a < d)                          \
            { SWAP(a,c); }                      \
            else                                \
            { PERM3_231(a,c,d); }               \
      else                                      \
        if (a < d)                              \
          if (a < c)                            \
          { PERM3_231(b,c,d); }                 \
          else                                  \
          { PERM4_3142(a,b,c,d); }              \
        else                                    \
          if (a < b)                            \
          { PERM4_3412(a,b,c,d); }              \
          else                                  \
          { PERM4_3421(a,b,c,d); }              \
    else                                        \
      if (b < c)                                \
        if (b < d)                              \
          if (a < d)                            \
            if (a < b)                          \
            { SWAP(c,d); }                      \
            else                                \
            { PERM4_2143(a,b,c,d); }            \
          else                                  \
            if (a < c)                          \
            { PERM4_2413(a,b,c,d); }            \
            else                                \
            { PERM3_231(a,b,d); }               \
        else                                    \
          if (a < b)                            \
            if (a < d)                          \
            { PERM3_312(b,c,d); }               \
            else                                \
            { PERM4_4123(a,b,c,d); }            \
          else                                  \
            if (a < c)                          \
            { PERM3_312(a,c,d); }               \
            else                                \
            { SWAP(a,d); }                      \
      else                                      \
        if (a < c)                              \
          if (a < d)                            \
          { SWAP(b,d); }                        \
          else                                  \
          { PERM3_312(a,b,d); }                 \
        else                                    \
          if (a < b)                            \
          { PERM4_4312(a,b,c,d); }              \
          else                                  \
          { PERM4_4321(a,b,c,d); }              \
  }

#define IP_SORT4_ALT(a,b,c,d) {                  \
 typeof(a) s;                                   \
 typeof(b) t;                                   \
 typeof(c) u;                                   \
 typeof(d) v;                                   \
 ASORT4(a,b,c,d, s,t,u,v);                      \
 a=s;                                           \
 b=t;                                           \
 c=u;                                           \
 d=v;                                           \
}

/*!
 * \em Network-Sort-4 \p a, \p b, \p c, and \p d.
 * \complexity[code,time]: 5 CMP, 3*5=15 MOV
 */
#define NSORT4(a,b,c,d) {                       \
    SORT2(a,b); SORT2(c,d);                     \
    SORT2(a,c); SORT2(b,d);                     \
    SORT2(b,c);                                 \
  }

/*!
 * Standard-Sort-4 \p a, \p b, \p c, \p d.
 */
#define SORT4(a,b,c,d) IP_SORT4(a,b,c,d)

/* ========================================================================= */

/*!
 * \em Network-Sort-5 \p a, \p b, \p c, \p d, \p e.
 */
#define NSORT5(a,b,c,d,e) {                     \
    SORT3(a,b,c); SORT2(d,e);                   \
    SORT2(a,d); SORT2(c,e);                     \
    SORT3(b,c,d);                               \
  }

/*!
 * \em Network-Sort-5 \p a, \p b, \p c, \p d, \p e.
 * \see http://www.cs.uky.edu/~lewis/essays/algorithms/sortnets/sort-net.html
 */
#define NSORT5_ALT(a,b,c,d,e) {                 \
    SORT2(a,b); SORT2(d,e);                     \
    SORT2(c,e);                                 \
    SORT2(c,d);                                 \
    SORT2(b,e);                                 \
    SORT2(b,c);                                 \
    SORT2(a,d);                                 \
    SORT2(a,b); SORT2(c,d);                     \
  }

/*!
 * Standard-Sort-5 \p a, \p b, \p c, \p d, \p e.
 */
#define SORT5(a,b,c,d,e) NSORT5(a,b,c,d,e)

/* ========================================================================= */

/*!
 * \em Network-Sort-6 \p a, \p b, \p c, \p d, \p e, \p f.
 */
#define NSORT6(a,b,c,d,e,f) {                   \
    SORT3(a,b,c); SORT3(d,e,f);                 \
    SORT2(a,d); SORT2(b,e); SORT2(c,f);         \
    SORT2(c,d);                                 \
    SORT2(b,c); SORT2(d,e);                     \
    SORT2(c,d);                                 \
  }

/*!
 * Standard-Sort-6 \p a, \p b, \p c, \p d, \p e, \p f.
 */
#define SORT6(a,b,c,d,e,f) NSORT6(a,b,c,d,e,f)

/* ========================================================================= */

/*!
 * \em Network-Sort-7 \p a, \p b, \p c, \p d, \p e, \p f, \p g.
 */
#define NSORT7(a,b, c,d,e, f,g) {               \
    SORT2(a,b); SORT3(c,d,e); SORT2(f,g);       \
    SORT3(a,c,f); SORT3(b,e,g);                 \
    NSORT5(b,c,d,e,f);                          \
  }

/*!
 * Standard-Sort-7 \p a, \p b, \p c, \p d, \p e, \p f, \p g.
 */
#define SORT7(a,b,c,d,e,f,g) NSORT7(a,b,c,d,e,f,g)

/* ========================================================================= */

/*!
 * \em Network-Sort-8 \p a, \p b, \p c, \p d, \p e, \p f, \p g, \p h.
 * \see http://en.wikipedia.org/wiki/Batcher_odd-even_mergesort
 * \complexity[code,time] (\em Optimal!): 19 CMP, 3*19 MOV
 */
#define NSORT8(a,b,c,d,e,f,g,h) {                       \
    SORT2(a,b); SORT2(c,d); SORT2(e,f); SORT2(g,h);     \
    SORT2(a,c); SORT2(b,d); SORT2(e,g); SORT2(f,h);     \
    SORT2(a,e); SORT2(f,g); SORT2(b,c); SORT2(d,h);     \
    SORT2(b,f); SORT2(c,g);                             \
    SORT2(c,e); SORT2(d,f);                             \
    SORT2(b,c); SORT2(d,e); SORT2(f,g);                 \
  }

/*!
 * Standard-Sort-8 \p a, \p b, \p c, \p d, \p e, \p f, \p g, \p h.
 */
#define SORT8(a,b,c,d,e,f,g,h) NSORT8(a,b,c,d,e,f,g,h)

/*!
 * Standard-Sort-N the array \p x at offset \p i of length \p n.
 */
#define SORTN(x,n) {                                                  \
    switch (n) {                                                        \
    case 2: SORT2((x)[0], (x)[1]); break;                               \
    case 3: SORT3((x)[0], (x)[1], (x)[2]);; break;                      \
    case 4: SORT4((x)[0], (x)[1], (x)[2], (x)[3]);; break;              \
    case 5: SORT5((x)[0], (x)[1], (x)[2], (x)[3], (x)[4]);; break;      \
    case 6: SORT6((x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5]);; break; \
    case 7: SORT7((x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5], (x)[6]);; break; \
    case 8: SORT8((x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5], (x)[6], (x)[7]); break; \
    }                                                                   \
  }

#define SORTN_MAX (8)

#ifdef __cplusplus
}
#endif
