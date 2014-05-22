#include "aesc.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* ========================================================================= */

#define AESC (0x1b)		/**< Escape */

/*!
 * Default to non-color support for terminal.
 */
static bool g_AESC_terminal_support_flag = 0;

__attribute__ ((constructor))
int AESC_init(void)
{
  const char * term = getenv("TERM");
  if (term &&
      (strncmp(term, "xterm", 5) == 0 ||
       strncmp(term, "rxvt", 4) == 0 ||
       strcmp(term, "vt102") == 0)) {
    g_AESC_terminal_support_flag = 1;
  }

  return g_AESC_terminal_support_flag;
}

int AESC_isSupported(void)
{
  return g_AESC_terminal_support_flag;
}

void AESC_DATTR_fprint_1(FILE * stream, AESC_DATTR_t a)
{
  if (AESC_isSupported()) { fprintf(stream, "%c[%dm", AESC, a); }
}

void AESC_DATTR_fprint_2(FILE * stream,
                         AESC_DATTR_t a,
                         AESC_DATTR_t b)
{
  if (AESC_isSupported()) { fprintf(stream, "%c[%d;%dm", AESC, a, b); }
}

void AESC_DATTR_fprint_3(FILE * stream,
                         AESC_DATTR_t a,
                         AESC_DATTR_t b,
                         AESC_DATTR_t c)
{
  if (AESC_isSupported()) { fprintf(stream, "%c[%d;%d;%dm", AESC, a, b, c); }
}

/* ========================================================================= */
