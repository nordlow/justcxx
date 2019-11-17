/*!
 * \file xstr.h
 * \brief Partial (Sub) C-String.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-09-20 12:33
 */

#pragma once

#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Partial (Sub) String.
 */
typedef struct {
  /*!
   * Length of string + 1 terminator.
   *
   * Number of char (bytes) in \c ch or wchar_t in \c wch.
   */
  size_t     l;

  union {
    char    *ch;                /**< Byte character. */
    wchar_t *wch;               /**< Wide character. */
  } p;                          /**< Pointer to beginning of sub-string (\em possibly nil-terminated) */

  /*!
   * Character Encoding.
   *
   * Use as argument to \c iconv_open(). Do iconv -l in shell to see
   * list of supported formats.
   */
  char * ccode;

  unsigned int wcF:1;                 /**< Flags for wchar_t data. */
  unsigned int freeF:1;               /**< Flags that \c p should be freed upon clearing. */
} xStr;

/* ---------------------------- Group Separator ---------------------------- */

void xstr_init(xStr * xA);

int xstr_is_empty(const xStr * xA);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Initialization of an \c xStr \em reference to a pointer \p p
 *       of length \p l.
 *
 * No freeing of \p p will be made upon destruction (\c xstr_clear()).
 */
/* @{ */
void xstr_init_ref(xStr * xA, const char * p, size_t l);
void xstr_init_ref_wchar(xStr * xA, const wchar_t * p, size_t l);
/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Initialization using a pointer \p p of length \p l.
 *
 * Freeing of \p p will be performed upon destruction (\c xstr_clear()).
 */
/* @{ */
void xstr_init_real(xStr * xA, char * p, size_t l);
void xstr_init_real_wchar(xStr * xA, wchar_t * p, size_t l);
void xstr_init_real_xstr(xStr * xA, const xStr * xB);
/* @{ */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Initialization by copy the contents from pointer \p p of
 *       length \p l.
 */
/* @{ */
void xstr_init_copy(xStr * xA, const char * p, size_t l);
void xstr_init_copy_wchar(xStr * xA, const wchar_t * p, size_t l);
void xstr_init_copy_xstr(xStr * xA, const xStr * xB);
/* @} */

/* ---------------------------- Group Separator ---------------------------- */

void xstr_init_home_path(xStr * xA);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Inserting at a given offset \p i.
 */
/* @{ */
void xstr_ins_cbuf(xStr * xA, size_t i, const char * p, size_t l);
void xstr_ins_wcbuf(xStr * xA, size_t i, const wchar_t * p, size_t l);
void xstr_ins_xstr(xStr * xA, size_t i, const xStr * xB);
/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Getters.
 */
/* @{ */

/*! Get a pointer to the first \em byte in the \p xA */
static inline char * xstr_get_buf(const xStr * xA) { return xA->p.ch; }

/*! Get element of \p xA at index \p i */
static inline unsigned char xstr_nth(const xStr * xA, size_t i) { return xA->p.ch[i]; }

/*! Get element of \p xA at index \p i */
static inline char* xstr_nthptr(const xStr * xA, size_t i) { return &xA->p.ch[i]; }

/*! Get size of \p xA in bytes. */
static inline size_t xstr_get_size(const xStr * xA) { return xA->wcF ? xA->l * sizeof(wchar_t) : xA->l; }
/* @} */

/* ---------------------------- Group Separator ---------------------------- */

void xstr_expand_to_wchar(xStr * xA);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Appending.
 */
/* @{ */
void xstr_app_cbuf(xStr * xA, const char * p, size_t l);
void xstr_app_wcbuf(xStr * xA, const wchar_t * p, size_t l);
void xstr_app_xstr(xStr * xA, const xStr * xB);
void xstr_app_cstr(xStr * xA, const char * cB);
/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Prepending.
 */
/* @{ */
void xstr_prep_cbuf(xStr * xA, const char * p, size_t l);
void xstr_prep_wcbuf(xStr * xA, const wchar_t * p, size_t l);
void xstr_prep_xstr(xStr * xA, const xStr * xB);
void xstr_prep_cstr(xStr * xA, const char * cB);
/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Removing.
 */
/* @{ */
//static inline void xstr_backspace(xStr * xA) { PTODO("Backspace!\n"); }
/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Equality.
 */
/* @{ */
int xstr_eq_cbuf(const xStr * xA, const char * p, size_t l);
int xstr_eq_wcbuf(const xStr * xA, const wchar_t * p, size_t l);
int xstr_eq_xstr(const xStr * xA, const xStr * xB);
int xstr_eq_cstr(const xStr * xA, const char * cB);
/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Printing.
 */
/* @{ */
void xstr_fprint(FILE * stream, const xStr * xA);
void xstr_print(const xStr * xA);
/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Clearing.
 */

/* @{ */

/*!
 * Reset/Reinitialize/Empty Contents of \p xA for further use.
 */
void xstr_empty(xStr * xA);

/*!
 * Clear Dynamically Allocated (Heap) Data. \em No further use of \p xA is allowed.
 */
static inline
void xstr_clear(xStr * xA)
{
  if (xA->freeF) { free(xA->p.ch); }
  /* xA->p.ch = NULL; */
  /* xA->wcF = 0; */
  /* xA->l = 0; */
  /* if (xA->ccode) { free(xA->ccode); xA->ccode = NULL; } /\* expand character encoding *\/ */
}

/*!
 * Clear Dynamically Allocated (Heap) Data and then Deallocate the
 * structure pointed to by \p xA. \em No further use of \p xA is
 * allowed.
 */
void xstr_delete(xStr * xA);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \name Testing.
 */
/* @{ */
int xstr_test(void);
/* @} */

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
