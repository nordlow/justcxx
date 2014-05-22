#include "magic_x.h"
#include "stdio_x.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef HAVE_MAGIC_H
#  include <magic.h>
#endif
#include <sys/param.h>

#define MAGNUM (65536)		/**< Number of magics stored */

typedef struct MagicDB {
  char * mags[MAGNUM];		/**< Magic Table of strings. */
  size_t magsN;    /**< Number of strings entries in magic table. */
#ifdef HAVE_MAGIC_H
  magic_t cookie;		/**< Magic Cookie (handle). */
#endif
} MagicDB;

static char * g_mags[MAGNUM];	/**< Magic table. */
static size_t g_lmag = 0;		/**< End index size_t magic table. */

#ifdef HAVE_MAGIC_H
static magic_t g_cookie;
#endif

#ifdef HAVE_MAGIC_H
int
magic_open_and_load_stubborn(magic_t * cookie, int flags)
{
  bool show_flag = false;
  int ret = -1;
  *cookie = magic_open(flags); /* open cookie handle */
  if (*cookie) {
    char path[MAXPATHLEN];	/* path buffer */
    if (ret != 0) {		/* still no magic */
      if ((ret = magic_load(*cookie, NULL)) == 0) {
        if (show_flag) { PNOTE("default magic was loaded\n"); }
      }
    }
    if (ret != 0) {		/* still no magic */
      snprintf(path, sizeof(path), "%s", "/home/per/alt/share/file/magic");
      if ((ret = magic_load(*cookie, path)) == 0) {
        if (show_flag) { PNOTE("magic at %s was loaded\n", path); }
      }
    }
    if (ret != 0) {		/* still no magic */
      snprintf(path, sizeof(path), "%s", "/usr/local/share/file/magic");
      if ((ret = magic_load(*cookie, path)) == 0) {
        if (show_flag) { PNOTE("magic at %s was loaded\n", path); }
      }
    }
    if (ret != 0) {		/* still no magic */
      snprintf(path, sizeof(path), "%s", "/usr/share/file/magic");
      if ((ret = magic_load(*cookie, path)) == 0) {
        if (show_flag) { PNOTE("magic at %s was loaded\n", path); }
      }
    }
    if (ret != 0) {		/* still no magic */
      snprintf(path, sizeof(path), "%s", "/usr/share/misc/file/magic");
      if ((ret = magic_load(*cookie, path)) == 0) {
        if (show_flag) { PNOTE("magic at %s was loaded\n", path); }
      }
    }
    if (ret != 0) {		/* still no magic */
      leprintf("no magic %s could not be loaded\n", path);
      magic_close(*cookie);
      *cookie = 0;
    }
  } else {
    PWARN("No magic could be loaded from disk!\n");
  }
  return ret;
}
#endif

__attribute__ ((constructor))
int
magicDB_init(void)
{
#ifdef HAVE_MAGIC_H
  return magic_open_and_load_stubborn(&g_cookie, 0);
#else
  return -1;
#endif
}

/*! Set this to generalize magic strings. */
static bool g_magic_generalize_flag = 0;

magicDB_index_t
magicDB_reg(const char * magic_str)
{
  size_t i;

  /* strip magic from details and register it */
  if (g_magic_generalize_flag) {
    if (strncmp(magic_str, "RPM", 3) == 0) {
      magic_str = "RPM";
    } else if (strncmp(magic_str, "PNG", 3) == 0) {
      magic_str = "PNG";
    } else if (strncmp(magic_str, "JPEG", 4) == 0) {
      magic_str = "JPEG";
    } else if (strncmp(magic_str, "ELF", 3) == 0) {
      magic_str = "ELF";
    } else if (strncmp(magic_str, "symbolic link", 13) == 0) {
      magic_str = "symbolic link";
    } else if (strncmp(magic_str, "unreadable symlink", 18) == 0) {
      magic_str = "unreadable symlink";
    } else if (strncmp(magic_str, "character special", 17) == 0) {
      magic_str = "character special";
    } else if (strncmp(magic_str, "ASCII", 5) == 0) {
      magic_str = "ASCII";
    } else {
      magic_str = "data";
    }
  }

  const size_t magic_len = strlen(magic_str);

  /* search for it */
  for (i = 0; i < g_lmag; i++) {
    if (strncmp(g_mags[i], magic_str, magic_len) == 0) {
      return i;
    }
  }

  if (i == g_lmag) {		/* if not found */
    if (g_lmag < MAGNUM) {
      g_mags[g_lmag++] = strdup(magic_str);
      /*       lprintf("idx:%d added %s\n", i, magic_str); */
    } else {
      PERR("register full!\n");
    }
  }

  return i;
}

const char *
magicDB_lookup(magicDB_index_t i)
{
  if (i < g_lmag) {
    return g_mags[i];
  } else {
    leprintf("index %d out of bounds. Should be less than %zd!", i, g_lmag);
    return 0;
  }
}

void
magicDB_clear(void)
{
#ifdef HAVE_MAGIC_H
  if (g_cookie) {
    magic_close(g_cookie);	/* close cookie handle */
  }
#else
#endif
  for (size_t i = 0; i < g_lmag; i++) {
    free(g_mags[i]);
  }
}

void
magicDB_print(void)
{
  for (size_t i = 0; i < g_lmag; i++) {
    printf("idx:%zd mag:%s\n", i, g_mags[i]);
  }
}

const char * magicDB_get(const char * fname)
{
#ifdef HAVE_MAGIC_H
  if (g_cookie) {
    return magic_file(g_cookie, fname);
  } else {
    return "data";		/* default to data */
  }
#else
  return "data";		/* default to data */
#endif
}
