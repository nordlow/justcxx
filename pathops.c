#include "pathops.h"
#include "stdio_x.h"
#include <string.h>
#include <ctype.h>

char * cstr_pathapp(char * cdir, const char * sfil)
{
  size_t clen = strlen(cdir), flen = strlen(sfil);
  cdir = realloc(cdir, clen+1 + flen+1);
  cdir[clen] = PATH_SEP;
  memcpy(cdir + clen+1, sfil, flen);
  cdir[clen+1 + flen] = '\0';
  return cdir;
}

/* ---------------------------- Group Separator ---------------------------- */

char * cbuf_pathadd(const char * cdir, size_t clen,
                    const char * sfil, size_t flen,
                    size_t *full_len_ret)
{
  bool canon_flag = 1;	 /* flags that we should mutating_canonicalize path  */
  size_t cskip = 0, fskip = 0;
  if (canon_flag) {
    size_t i;
    for (i = 0; i < clen && cdir[clen-1-i] == PATH_SEP; i++)
      ;
    cskip = i;
    for (i = 0; i < flen && sfil[       i] == PATH_SEP; i++)
      ;
    fskip = i;
  }

  const size_t cadj = clen-cskip;
  const size_t fadj = flen-fskip;

  const size_t full_len = cadj+1 + fadj;
  char * full = malloc(full_len+1);
  memcpy(full,          cdir,       cadj); full[cadj] = PATH_SEP;
  memcpy(full + cadj+1, sfil+fskip, fadj); full[full_len] = '\0';

  if (full_len_ret) { *full_len_ret = full_len; }

  return full;
}

pure char * cstr_pathadd(const char * cdir, const char * sfil)
{
  size_t clen = strlen(cdir), flen = strlen(sfil);
  return cbuf_pathadd(cdir, clen, sfil, flen, NULL);
}

/* ---------------------------- Group Separator ---------------------------- */

pure char * cbuf_pathadd3(const char * cdir, size_t clen,
                                           const char * dsub, size_t slen,
                                           const char * sfil, size_t flen)
{
  bool canon_flag = 1;	 /* flags that we should mutating_canonicalize path  */
  size_t cskip = 0, sskip = 0, fskip = 0;
  if (canon_flag) {
    size_t i;
    for (i = 0; i < clen && cdir[clen-1-i] == PATH_SEP; i++)
      ;
    cskip = i;
    for (i = 0; i < slen && dsub[slen-1-i] == PATH_SEP; i++)
      ;
    sskip = i;
    for (i = 0; i < flen && sfil[       i] == PATH_SEP; i++)
      ;
    fskip = i;
  }

  const size_t cadj = clen-cskip;
  const size_t sadj = slen-sskip;
  const size_t fadj = flen-fskip;

  const size_t full_len = cadj+1 + sadj+1 + fadj;
  char * full = malloc(full_len+1);
  memcpy(full,                   cdir,       cadj); full[cadj] = PATH_SEP;
  memcpy(full + cadj+1,          dsub,       sadj); full[cadj+1 + sadj] = PATH_SEP;
  memcpy(full + cadj+1 + sadj+1, sfil+fskip, fadj); full[full_len] = '\0';

  return full;
}

pure char * cstr_pathadd3(const char * cdir, const char * dsub, const char * sfil)
{
  size_t clen = strlen(cdir), slen = strlen(dsub), flen = strlen(sfil);
  return cbuf_pathadd3(cdir, clen, dsub, slen, sfil, flen);
}

/* ---------------------------- Group Separator ---------------------------- */

pure char * cbuf_pathadd2_atHOME(const char * dsub, size_t slen)
{
  /* size_t full_len = 0; */
  /* return cbuf_pathadd(xstr_get_buf(&g_home_path), */
  /*                     xstr_get_size(&g_home_path), */
  /*                     dsub, strlen(dsub), */
  /*                     &full_len); */
  return NULL;
}

/* ---------------------------- Group Separator ---------------------------- */

pure char * cstr_pathadd_atHOME(const char * dsub, const char * sfil)
{
  /* return cbuf_pathadd3(xstr_get_buf(&g_home_path), */
  /*                      xstr_get_size(&g_home_path), */
  /*                      dsub, strlen(dsub), */
  /*                      sfil, strlen(sfil)); */
  return NULL;
}

/* ---------------------------- Group Separator ---------------------------- */

int
cbuf_pathparseSRCV(const char * buf, int size,
                   int * base_off, int * base_len,
                   int ver_offs[VER_DEPTH_MAX],
                   int ver_lens[VER_DEPTH_MAX],
                   int * ver_depth,
                   int * ar_off, int * ar_len,
                   int * zip_off, int * zip_len)
{
  int d;			/* version depth counter */

  /* version flag that describes the two states version search can be in */
  int begf;

  int i = 0;			/* begin at first character index */

  /* basename part */
  *base_off = i;
  for (; i < size; i++) {
    if (buf[i] == '-' &&
        i + 1 < size &&
        isdigit((int)buf[i + 1])) {
      break;			/* looking at regexp "-[0-9]" */
    }
  }
  *base_len = i - *base_off;

  /* separator part */
  i++;

  /* version part */
  d = 0;
  begf = FALSE;
  for (; i < size; i++) {
    if (begf) {			/* if version has started */
      if (buf[i] == '.' ||
          ! isalnum((int)buf[i])) { /* if version ends here */
        ver_lens[d] = i - ver_offs[d];
        d++;
        begf = FALSE;
      }
    } else {
      if (isdigit((int)buf[i])) { /* if version begins here */
        ver_offs[d] = i;
        begf = TRUE;
      } else {
        break;
      }
    }
  }

  /* possibly last version part */
  if (begf) {
    ver_lens[d] = i - ver_offs[d];
    d++;
  }

  *ver_depth = d;		/* set version depth */

  /* skip dots */
  for (; i < size && buf[i] == '.'; i++);

  /* archive part */
  *ar_off = i;
  for (; i < size; i++) {
    if (buf[i] == '.') {
      break;			/* looking at regexp "-[0-9]" */
    }
  }
  *ar_len = i - *ar_off;

  /* skip dots */
  for (; i < size && buf[i] == '.'; i++);

  /* zip part */
  *zip_off = i;
  for (; i < size; i++) {
    if (buf[i] == '.') {
      break;			/* looking at regexp "-[0-9]" */
    }
  }
  *zip_len = i - *zip_off;

  return d;
}

void test_pathparseSRCV_cstr(const char * buf)
{
  int d;
  int base_off, base_len;
  int ver_offs[VER_DEPTH_MAX];
  int ver_lens[VER_DEPTH_MAX];
  int ver_depth;
  int ar_off, ar_len, zip_off, zip_len;

  printf("FULL=\"%.*s\" => ", (int)strlen(buf), buf);

  cbuf_pathparseSRCV(buf, strlen(buf),
                     &base_off, &base_len,
                     ver_offs, ver_lens, &ver_depth,
                     &ar_off, &ar_len,
                     &zip_off, &zip_len);

  printf("BASE=\"%.*s\" ", base_len, buf + base_off);

  for (d = 0; d < ver_depth; d++) {
    printf("VER[%d]=\"%.*s\" ", d, ver_lens[d], buf + ver_offs[d]);
  }

  printf("AR=\"%.*s\" ", ar_len, buf + ar_off);
  printf("COMPR=\"%.*s\" ", zip_len, buf + zip_off);

  printf("\n");
}

/* ---------------------------- Group Separator ---------------------------- */

#if 0
#define BUFLEN 257

#define DIRSEP       '>'
#define ISDIRSEP(c)  ((c) == '>' || (c) == '<')

char *
cstr_path_tilde_expand(const char *dirname)
{
  struct passwd *user;
  static char olddir[BUFLEN];
  static char oldrealdir[BUFLEN];
  static char temp[BUFLEN];
  int i, j;

  if (dirname[0] != '~')		/* Not a tilde...return param */
    return(dirname);
  if (!strcmp(olddir,dirname)) {	/* Same as last time */
    return(oldrealdir);			/* so return old answer. */
  } else {
    j = (int)strlen(dirname);
    for (i = 0; i < j; i++)		/* find username part of string */
      if (!ISDIRSEP(dirname[i]))
        temp[i] = dirname[i];
      else break;
    temp[i] = '\0';			/* tie off with a NULL */
    if (i == 1) {			/* if just a "~" */
      user = getpwnam(whoami());	/*  get info on current user */
    } else {
      user = getpwnam(&temp[1]);	/* otherwise on the specified user */
    }
  }
  if (user != NULL) {			/* valid user? */
    strcpy(olddir, dirname);		/* remember the directory */
    strcpy(oldrealdir,user->pw_dir);	/* and their home directory */
    strcat(oldrealdir,&dirname[i]);
    return(oldrealdir);
  } else {				/* invalid? */
    strcpy(olddir, dirname);		/* remember for next time */
    strcpy(oldrealdir, dirname);
    return(oldrealdir);
  }
}

#endif
