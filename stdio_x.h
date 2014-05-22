/*!
 * \file stdio_x.h
 * \brief Extensions to Standard Input & Output (stdio.h)
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cc_features.h"
#include "aesc.h"
#include "pnw_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG_CHECK_ALL
#  define DEBUG_CHECK_INDEX
#  define DEBUG_CHECK_TYPE
#  define DEBUG_CHECK_ZERO
#  define DEBUG_CHECK_SIZE
#  define DEBUG_CHECK_MEM
#endif

#define GFUN __FUNCTION__
#define GPFUN __PRETTY_FUNCTION__

/*! \name My \c printf \em wrappers using the GCC constant \c __FUNCTION__. */
/* @{ */
#if (defined(__clang__) || defined(__GNUC__))
#define lprintf(format, args...) printf("%s: " format , __FUNCTION__, ##args)
#define eprintf(format, args...) fprintf(stderr, format , ##args)
#define leprintf(format, args...) fprintf(stderr, "%s: " format , __FUNCTION__, ##args)
#define lperror(error) { fprintf (stderr, "%s: ", __FUNCTION__); perror(error); }
#endif
/* @} */

/*! \name My \c printf \em wrappers using the GCC constant \c __FUNCTION__ and ANSI ESC Control Sequences. */
/* @{ */
#define printf_a(attr, format, args...) AESC_DATTR_fprint_1(stdout, attr); printf(format, ##args); AESC_DATTR_fprint_1(stdout, AESC_Reset_All);
#define fprintf_a(attr, stream, format, args...) AESC_DATTR_fprint_1(stdout, attr); fprintf(stream, format, ##args); AESC_DATTR_fprint_1(stdout, AESC_Reset_All);
#define dprintf_a(attr, fd, format, args...) AESC_DATTR_fprint_1(stdout, attr); dprintf(fd, format, ##args); AESC_DATTR_fprint_1(stdout, AESC_Reset_All);
#define lprintf_a(attr, format, args...) AESC_DATTR_fprint_1(stdout, attr); lprintf(format , __FUNCTION__, ##args); AESC_DATTR_fprint_1(stdout, AESC_Reset_All);
#define eprintf_a(attr, format, args...) AESC_DATTR_fprint_1(stdout, attr); eprintf(format , __FUNCTION__, ##args); AESC_DATTR_fprint_1(stdout, AESC_Reset_All);
#define leprintf_a(attr, format, args...) AESC_DATTR_fprint_1(stdout, attr); leprintf(format , __FUNCTION__, ##args); AESC_DATTR_fprint_1(stdout, AESC_Reset_All);
/* @} */

#ifdef DEBUG_CHECK_ALL

#  define PNOTE(format, args...) {                \
    printf("*** ");                               \
                                                  \
    AESC_DATTR_fprint_1(stdout, AESC_FG_Blue);    \
    printf("%s", GFUN);                           \
    AESC_DATTR_fprint_1(stdout, AESC_Reset_All);  \
                                                  \
    printf(": ");                                 \
                                                  \
    AESC_DATTR_fprint_1(stdout, AESC_FG_Green);   \
    printf("NOTE: " format, ##args);              \
    AESC_DATTR_fprint_1(stdout, AESC_Reset_All);  \
  }

#  define PTEST(format, args...) {              \
 printf("*** ");                                \
                                                \
 AESC_DATTR_fprint_1(stdout, AESC_FG_Blue);     \
 printf("%s", GFUN);                            \
 AESC_DATTR_fprint_1(stdout, AESC_Reset_All);   \
                                                \
 printf(": ");                                  \
                                                \
 AESC_DATTR_fprint_1(stdout, AESC_FG_Cyan);     \
 printf("TEST: " format, ##args);               \
 AESC_DATTR_fprint_1(stdout, AESC_Reset_All);   \
}

#  define PTODO(format, args...) {                \
    printf("*** ");                               \
                                                  \
    AESC_DATTR_fprint_1(stdout, AESC_FG_Blue);    \
    printf("%s", GFUN);                           \
    AESC_DATTR_fprint_1(stdout, AESC_Reset_All);  \
                                                  \
    printf(": ");                                 \
                                                  \
    AESC_DATTR_fprint_1(stdout, AESC_FG_Magenta); \
    printf("TODO: " format, ##args);              \
    AESC_DATTR_fprint_1(stdout, AESC_Reset_All);  \
  }

#  define PTOWRITE(format, args...) {           \
 printf("*** ");                                \
                                                \
 AESC_DATTR_fprint_1(stdout, AESC_FG_Blue);     \
 printf("%s", GFUN);                            \
 AESC_DATTR_fprint_1(stdout, AESC_Reset_All);   \
                                                \
 printf(": ");                                  \
                                                \
 AESC_DATTR_fprint_1(stdout, AESC_FG_Magenta);	\
 printf("TOWRITE: " format, ##args);            \
 AESC_DATTR_fprint_1(stdout, AESC_Reset_All);   \
}

#  define PWARN(format, args...) {                \
    printf("*** ");                               \
                                                  \
    AESC_DATTR_fprint_1(stdout, AESC_FG_Blue);    \
    printf("%s", GFUN);                           \
    AESC_DATTR_fprint_1(stdout, AESC_Reset_All);	\
                                                  \
    printf(": ");                                 \
                                                  \
    AESC_DATTR_fprint_1(stdout, AESC_FG_Yellow);	\
    printf("WARN: " format, ##args);              \
    AESC_DATTR_fprint_1(stdout, AESC_Reset_All);	\
  }

#  define PERR(format, args...) {               \
 printf("*** ");                                \
                                                \
 AESC_DATTR_fprint_1(stdout, AESC_FG_Blue);     \
 printf("%s", GFUN);                            \
 AESC_DATTR_fprint_1(stdout, AESC_Reset_All);   \
                                                \
 printf(": ");                                  \
                                                \
 AESC_DATTR_fprint_1(stdout, AESC_FG_Red);      \
 printf("ERR: " format, ##args);                \
 AESC_DATTR_fprint_1(stdout, AESC_Reset_All);   \
                                                \
 raise(SIGSTOP);                                \
}

#else  /* empty macros for optimal performance */
#  define PNOTE(format, args...)
#  define PTEST(format, args...)
#  define PTODO(format, args...)
#  define PTOWRITE(format, args...)
#  define PWARN(format, args...)
#  define PERR(format, args...)
#endif

/*! Print \p n spaces to \p stream. */
void fspaces(FILE * stream, size_t n);
/*! Print \p n spaces to \c stdout. */
void spaces(size_t n);

/*! Print the char ch n times in a row at stream. */
static inline void mputchar(FILE * stream, size_t n, char ch)
{
  size_t i;
  for (i = 0; i < n; i++) {
    putc(ch, stream);
  }
}
/*! Print \p n spaces to \p stream. */
static inline void mputspace(FILE * stream, size_t n) { mputchar(stream, n, ' '); }
/*! Print separator at stream. */
static inline void putsep(FILE * stream)
{
  mputchar(stream, 79, '=');
  putc('\n', stream);
}

/*!
 * Print a newline character to \c stdout.
 */
static inline void endline(void) { putc('\n', stdout); }

/*!
 * Print a newline character to \p stream.
 */
static inline void fendline(FILE * stream) { putc('\n', stream); }

/* ---------------------------- Group Separator ---------------------------- */

/*! Determine whether \p c is an ASCII \em binary digit or not.
 */
static inline pure int isbdigit(char c) { return c >= '0' && c <= '1'; }
/*! Determine whether \p c is an ASCII \em octal digit or not.
 */
static inline pure int isodigit(char c) { return c >= '0' && c <= '7'; }

/* ---------------------------- Group Separator ---------------------------- */

/*! Print each byte in \p buf of length \p buflen as a \em hexadecimal
 * char to \p stream.
 */
void chararray_fprint_hex(FILE* stream, const uchar *buf, size_t len);
/*! Print each byte in \p buf of length \p buflen as a \em hexadecimal
 * char.
 */
void chararray_print_hex(const uchar * buf, size_t len);
/*! Print each byte in \p buf of length \p buflen as a char if it is an
 * ASCII alphanumeric character, otherwise in the hexadecimal form
 * 0x??.
 */
void chararray_print_humanely(const uchar *buf, size_t len);

/* ---------------------------- Group Separator ---------------------------- */

/*! Print \em Internal Binary Digits of \p a. */
void int_binfprint(FILE * stream, int a);

/*! Print \em Internal Binary Digits of \p a.
 * \return 0 upon success, -1 otherwise.
 */
int float_binfprint(FILE * stream, float a);

/*! Print \p s to \p stream in a \em human readable format using \em SI-prefixes.
 *
 * See: http://en.wikipedia.org/wiki/SI_prefix
 */
void double_fsiunit_show(FILE * stream, double a);

static inline void siunit_show(double a) { double_fsiunit_show(stdout, a); }

/*! Determine Size of the file \p fd in bytes and return it. */
static inline off_t fd_get_fsize(int fd) {
  struct stat st; fstat(fd, &st);
  return st.st_size;
}

/*! Determine Size of the file \p stream in bytes and return it. */
static inline off_t get_fsize(FILE * stream) { return fd_get_fsize(fileno(stream)); }

/*! Check if the file \p path exists.
 * Compare with \c file-exists-p.
 * \return 1 if \p path exists as a file, 0 otherwise.
 */
static inline int fexist(const char * path) {
  struct stat statbuf;
  int ret = stat(path, &statbuf);
  return (ret == 0);
}

/*! Check if the file \p path exists.
 * Compare with \c file-exists-p.
 * \return 1 if \p path exists as a file, 0 otherwise.
 */
static inline int lexist(const char * path) {
  struct stat statbuf;
  int ret = lstat(path, &statbuf);
  return (ret == 0);
}

/*! Find a New (Non-Existing) \em Humanely Named Copy of the file named \p name.
 *
 * \example
 * - "file.txt"          => "file (copy 1).txt
 * - "file (copy 1).txt" => "file (copy 2).txt
 * - ...
 *
 * \return copy name
 */
size_t find_file_new_COPY(const char * cdir, size_t clen,
                          const char * lsub, size_t llen,
                          char * copy_lsub, size_t copy_lsub_room);

static inline int uint2str(unsigned int x, char * buf, size_t len) { return snprintf(buf, len, "%u", x); }

/*! Read the \em Contents of the file at \p path into the memory buffer
 * at \p buf_ret and setting the length pointed to by\p len_ret.
 *
 * \return the data reallocated from \p buf or \c NULL if read failed.
 */
char *fload(const char *path, char *buf, size_t *len_ret);

/*! Read the \em Contents of the file at \p path into the memory buffer
 * at \p buf_ret and setting the length pointed to by\p len_ret.
 *
 * \return the return value from \c read()
 */
int fload_alt(const char * path, char ** buf_ret, size_t * len_ret);

/*! Create/Open a Temporary File. */
static inline FILE* fopen_tmp(void) {
#if defined (_WIN32) || defined (__MSDOS__) || defined (__DJGPP__)
  FILE *s;
  tmpfile_s(&s);
  return s;
#else
  return tmpfile();
#endif
}

#ifdef __cplusplus
}
#endif
