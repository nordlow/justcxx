#include "xstr.h"
#include "stdio_x.h"

#include <iconv.h>
#include <stdlib.h>

void xstr_init(xStr * xA)
{
  xA->l = 0;
  xA->p.ch = NULL;
  xA->wcF = 0;
  xA->freeF = 1;
  xA->ccode = NULL;
}

int xstr_is_empty(const xStr * xA)
{
  return (xA->p.ch == NULL ||
          xA->l == 0);
}

/* ---------------------------- Group Separator ---------------------------- */

void
xstr_init_ref(xStr * xA, const char * p, size_t l)
{
#ifndef NDEBUG
  if (p == 0) { PWARN("p is NULL\n"); }
#endif

  if (l == 0) { l = p ? strlen(p) : 0; } /* if no length given try to find it */
  xA->l = l;
  xA->p.ch = (char*)p;           /* just set reference */
  xA->wcF = 0;
  xA->freeF = 0;          /* no freeing */
  xA->ccode = NULL;
}

void
xstr_init_ref_wchar(xStr * xA, const wchar_t * p, size_t l)
{
#ifndef NDEBUG
  if (p == 0) { PWARN("p is NULL\n"); }
#endif

  if (l == 0) { l = p ? wcslen(p) : 0; } /* if no length given try to find it */
  xA->l = l;
  xA->p.wch = (wchar_t*)p;           /* just set reference */
  xA->wcF = 1;
  xA->freeF = 0;          /* no freeing */
  xA->ccode = NULL;
}

/* ---------------------------- Group Separator ---------------------------- */

void
xstr_init_real(xStr * xA, char * p, size_t l)
{
#ifndef NDEBUG
  if (p == 0) { PWARN("p is NULL\n"); }
#endif

  if (l == 0) { l = p ? strlen(p) : 0; } /* if no length given try to find it */
  xA->l = l;
  xA->p.ch = p;
  xA->wcF = 0;
  xA->freeF = 1;
  xA->ccode = NULL;
}

void
xstr_init_real_wchar(xStr * xA, wchar_t * p, size_t l)
{
#ifndef NDEBUG
  if (p == 0) { PWARN("p is NULL\n"); }
#endif

  if (l == 0) { l = p ? wcslen(p) : 0; } /* if no length given try to find it */
  xA->l = l;
  xA->p.wch = p;
  xA->wcF = 1;
  xA->freeF = 1;
  xA->ccode = NULL;
}

void xstr_init_real_xstr(xStr * xA, const xStr * xB)
{
  if (xA->wcF) {
    if (xB->wcF) { xstr_init_real_wchar(xA, xB->p.wch, xB->l); }
    else { PTODO("xA->wch && xB->ch\n"); }
  } else {
    if (xB->wcF) { PTODO("xA->ch && xB->wch\n"); }
    else { xstr_init_real(xA, xB->p.ch, xB->l); }
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
xstr_init_copy(xStr * xA, const char * p, size_t l)
{
#ifndef NDEBUG
  if (p == 0) { PWARN("p is NULL\n"); }
#endif

  if (l == 0) { l = p ? strlen(p) : 0; } /* if no length given try to find it */
  xA->l = l;
  xA->p.ch = (char*)malloc((xA->l+1) * sizeof(char));  /* allocate l plus terminator */
  memcpy(xA->p.ch, p, xA->l);  /* copy contents */
  xA->p.ch[xA->l] = '\0';         /* terminator */
  xA->wcF = 0;
  xA->freeF = 1;           /* should be \c freed() */
  xA->ccode = NULL;
}

void
xstr_init_copy_wchar(xStr * xA, const wchar_t * p, size_t l)
{
#ifndef NDEBUG
  if (p == 0) { PWARN("p is NULL\n"); }
#endif

  if (l == 0) { l = p ? wcslen(p) : 0; } /* if no length given try to find it */
  xA->l = l;
  xA->p.wch = (wchar_t*)malloc((xA->l+1) * sizeof(wchar_t));  /* allocate l plus terminator */
  wmemcpy(xA->p.wch, p, xA->l);  /* copy contents */
  xA->p.wch[xA->l] = L'\0';       /* terminator */
  xA->wcF = 1;
  xA->freeF = 1;            /* should be \c freed() */
  xA->ccode = NULL;
}

void xstr_init_copy_xstr(xStr * xA, const xStr * xB)
{
  if (xB->wcF) { xstr_init_copy_wchar(xA, xB->p.wch, xB->l); }
  else              { xstr_init_copy(xA, xB->p.ch, xB->l); }
  xA->ccode = xB->ccode ? strdup(xB->ccode) : NULL; /* copy character coding */
}

/* ---------------------------- Group Separator ---------------------------- */

void xstr_init_home_path(xStr * xA)
{
  PTEST("Untested!\n");
  xstr_init_copy(xA, getenv("HOME"), 0);
}

/* ---------------------------- Group Separator ---------------------------- */

void
xstr_expand_to_wchar(xStr * xA)
{
  if (!xA->wcF) {               /* if not already expanded to wchar_t */
    if (xA->l >= 1) {
      wchar_t p_wc[xA->l]; /* space for \p xA->p.ch converted to \c wchar_t */
      mbstate_t ps;        /* multi-byte state */
      const char *pT = xA->p.ch;         /* because \c mbsrtowcs() changes this */
      size_t lW = mbsrtowcs(p_wc, &pT, xA->l, &ps); /* convert to \c wchar_t */
      if (lW != xA->l) { PERR("lW:%zd != l:%zd\n", lW, xA->l); }

      if (0) {
        /* setup output buffer */
        wchar_t bufO[xA->l+1];  /* length plus terminator */
        wchar_t *bufOp = &bufO[0];
        size_t sizeO = (xA->l) * sizeof(wchar_t);

        /* setup input buffer */
        char *bufIp = xA->p.ch;
        size_t sizeI = (xA->l) * sizeof(char);

        /* convert */
        const char * to_code = "WCHAR_T";
        const char * from_code = xA->ccode ? xA->ccode : "UTF-8"; /* default to UTF-8 */
        iconv_t id = iconv_open(to_code, from_code); /* open */
        if (id == (iconv_t)-1) { lperror("iconv_open()"); }    /* error check */
        size_t numC = iconv(id, &bufIp,&sizeI, &bufOp,&sizeO); /* use */
        if (numC == (size_t)-1) { lperror("iconv()"); }        /* error check */
        iconv_close(id);        /* close */

        /* length */
        size_t lenO = bufOp-&bufO[0];
        bufO[lenO] = L'\0';

        /* print */
        printf("bufO:%.*ls\n", (unsigned int)lenO, bufO);
        endline();
      }

      if (xA->freeF) { free(xA->p.ch); }
      xA->p.wch = (wchar_t*)malloc((lW+1) * sizeof(wchar_t));  /* allocate \c l plus terminator */
      xA->freeF = 1;

      wmemcpy(xA->p.wch, p_wc, lW); /* copy contents */
      xA->p.wch[xA->l] = L'\0';      /* terminator */
      xA->l = lW;

      if (xA->ccode) { free(xA->ccode); xA->ccode = NULL; } /* expand character encoding */
    }
    xA->wcF = 1;                /* now stored as wchar_t */
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
xstr_ins_cbuf(xStr * xA, size_t i, const char * p, size_t l)
{
#ifndef NDEBUG
  if (p == 0) { PWARN("p is NULL\n"); }
#endif
  if (i > xA->l) { PWARN("truncating index i=%zd to existing length l=%zd\n", i, l); }

  if (l == 0) { l = p ? strlen(p) : 0; } /* if no length given try to find it */

  if (xA->wcF) {
    wchar_t p_wc[l];            /* space for \p p converted to \c wchar_t */
    mbstate_t ps;               /* multi-byte state */
    const char *pT = p;         /* because \c mbsrtowcs() changes this */
    size_t lW = mbsrtowcs(p_wc, &pT, l, &ps); /* convert to \c wchar_t */

    xstr_ins_wcbuf(xA, i, p_wc, lW); /* NOTE: Reuse */
  } else {
    xA->p.ch = realloc(xA->p.ch, (xA->l+l+1) * sizeof(char));  /* allocate l plus terminator */
    /* move "up" existing second part */
    memmove(xA->p.ch + i+l, xA->p.ch + i, xA->l - i);
    memcpy(xA->p.ch + i, p, l); /* copy new contents */
    xA->l += l;
    xA->p.ch[xA->l] = '\0';              /* terminator */
    xA->freeF = 1;                /* should be \c freed() */
  }
}

void
xstr_ins_wcbuf(xStr * xA, size_t i, const wchar_t * p, size_t l)
{
#ifndef NDEBUG
  if (p == 0) { PWARN("p is NULL\n"); }
#endif
  if (i > xA->l) { PWARN("truncating index i=%zd to existing length l=%zd\n", i, l); }

  if (l == 0) { l = p ? wcslen(p) : 0; } /* if no length given try to find it */

  if (!xA->wcF) { xstr_expand_to_wchar(xA); }

  xA->p.wch = realloc(xA->p.wch, (xA->l+l+1) * sizeof(wchar_t));  /* allocate l plus terminator */
  /* move "up" existing second part */
  wmemmove(xA->p.wch + i+l, xA->p.wch + i, xA->l - i);
  wmemcpy(xA->p.wch + i, p, l); /* copy new contents */
  xA->l += l;
  xA->p.wch[xA->l] = L'\0';              /* terminator */
  xA->freeF = 1;                /* should be \c freed() */
}

/* ---------------------------- Group Separator ---------------------------- */

void
xstr_app_cbuf(xStr * xA, const char * p, size_t l)
{
#ifndef NDEBUG
  if (p == 0) { PWARN("p is NULL\n"); }
#endif

  if (l == 0) { l = p ? strlen(p) : 0; } /* if no length given try to find it */

  if (xA->wcF) {
    wchar_t p_wc[l];            /* space for \p p converted to \c wchar_t */
    mbstate_t ps;               /* multi-byte state */
    const char *pT = p;         /* because \c mbsrtowcs() changes this */
    size_t lW = mbsrtowcs(p_wc, &pT, l, &ps); /* convert to \c wchar_t */
    /* \TODO: Use iconv instead. */

    xstr_app_wcbuf(xA, p_wc, lW); /* NOTE: Reuse */
  } else {
    xA->l += l;
    xA->p.ch = realloc(xA->p.ch, (xA->l+1) * sizeof(char));  /* allocate l plus terminator */
    memcpy(xA->p.ch + xA->l - l, p, l);  /* copy contents */
    xA->p.ch[xA->l] = '\0';              /* terminator */
    xA->freeF = 1;                /* should be \c freed() */
  }
}

void
xstr_app_wcbuf(xStr * xA, const wchar_t * p, size_t l)
{
#ifndef NDEBUG
  if (p == 0) { PWARN("p is NULL\n"); }
#endif

  if (l == 0) { l = p ? wcslen(p) : 0; } /* if no length given try to find it */

  if (!xA->wcF) { xstr_expand_to_wchar(xA); }

  xA->l += l;
  xA->p.wch = realloc(xA->p.wch, (xA->l+1) * sizeof(wchar_t));  /* allocate l plus terminator */
  wmemcpy(xA->p.wch + xA->l - l, p, l);  /* copy contents */
  xA->p.wch[xA->l] = L'\0';              /* terminator */
  xA->freeF = 1;                /* should be \c freed() */
}

void xstr_app_xstr(xStr * xA, const xStr * xB)
{
  if (xB->wcF) { xstr_app_wcbuf(xA, xB->p.wch, xB->l); }
  else              { xstr_app_cbuf(xA, xB->p.ch, xB->l); }
}

void xstr_app_cstr(xStr * xA, const char * cB)
{
  xstr_app_cbuf(xA, cB, strlen(cB));
}

/* ---------------------------- Group Separator ---------------------------- */

void
xstr_prep_cbuf(xStr * xA, const char * p, size_t l)
{
#ifndef NDEBUG
  if (p == 0) { PWARN("p is NULL\n"); }
#endif

  if (l == 0) { l = p ? strlen(p) : 0; } /* if no length given try to find it */

  if (xA->wcF) {
    wchar_t p_wc[l];            /* space for \p p converted to \c wchar_t */
    mbstate_t ps;               /* multi-byte state */
    const char *pT = p;         /* because \c mbsrtowcs() changes this */
    size_t lW = mbsrtowcs(p_wc, &pT, l, &ps); /* convert to \c wchar_t */

    xstr_prep_wcbuf(xA, p_wc, lW); /* NOTE: Reuse */
  } else {
    xA->l += l;
    xA->p.ch = realloc(xA->p.ch, (xA->l+1) * sizeof(char));  /* allocate l plus terminator */
    memmove(xA->p.ch + l, xA->p.ch, xA->l - l);  /* move existing contents up in memory */
    memcpy(xA->p.ch, p, l);                /* copy contents */
    xA->p.ch[xA->l] = '\0';      /* terminator */
    xA->freeF = 1;          /* should be \c freed() */
  }
}

void
xstr_prep_wcbuf(xStr * xA, const wchar_t * p, size_t l)
{
#ifndef NDEBUG
  if (p == 0) { PWARN("p is NULL\n"); }
#endif

  if (l == 0) { l = p ? wcslen(p) : 0; } /* if no length given try to find it */

  if (!xA->wcF) { xstr_expand_to_wchar(xA); }

  xA->l += l;
  xA->p.wch = realloc(xA->p.wch, (xA->l+1) * sizeof(wchar_t));  /* allocate l plus terminator */
  wmemmove(xA->p.wch + l, xA->p.wch, xA->l - l);  /* move existing contents up in memory */
  wmemcpy(xA->p.wch, p, l);                /* copy contents */
  xA->p.wch[xA->l] = L'\0';      /* terminator */
  xA->freeF = 1;          /* should be \c freed() */
}

void xstr_prep_xstr(xStr * xA, const xStr * xB)
{
  if (xB->wcF) { xstr_prep_wcbuf(xA, xB->p.wch, xB->l); }
  else              { xstr_prep_cbuf(xA, xB->p.ch, xB->l); }
}

void xstr_prep_cstr(xStr * xA, const char * cB)
{
  xstr_prep_cbuf(xA, cB, strlen(cB));
}

/* ---------------------------- Group Separator ---------------------------- */

int xstr_eq_cbuf(const xStr * xA, const char * p, size_t l)
{
  int ret = 0;
  if (xA->l == l) {              /* efficient to do this first */
    if (xA->wcF) {
      wchar_t p_wc[l];            /* space for \p p converted to \c wchar_t */
      mbstate_t ps;               /* multi-byte state */
      const char *pT = p;         /* because \c mbsrtowcs() changes this */
      size_t lW = mbsrtowcs(p_wc, &pT, l, &ps); /* convert to \c wchar_t */

      ret = xstr_eq_wcbuf(xA, p_wc, lW);
    } else {
      ret = (l == 0 ||              /* optimize for empty case */
             memcmp(xA->p.ch, p, l) == 0);
    }
  }
  return ret;
}

int xstr_eq_wcbuf(const xStr * xA, const wchar_t * p, size_t l)
{
  int ret = 0;
  if (xA->wcF) {
    ret = ((xA->l == l) &&        /* efficient to do this first */
           (l == 0 ||             /* optimize for empty case */
            wmemcmp(xA->p.wch, p, l) == 0));
  } else {
    wchar_t p_wc[xA->l];  /* space for \p p converted to \c wchar_t */
    mbstate_t ps;               /* multi-byte state */
    const char *pT = xA->p.ch;         /* because \c mbsrtowcs() changes this */
    size_t lW = mbsrtowcs(p_wc, &pT, xA->l, &ps); /* convert to \c wchar_t */

    ret = ((lW == l) &&           /* efficient to do this first */
           (lW == 0 ||            /* optimize for empty case */
            wmemcmp(p_wc, p, l) == 0));
  }
  return ret;
}

int xstr_eq_xstr(const xStr * xA, const xStr * xB)
{
  if (xB->wcF) { return xstr_eq_wcbuf(xA, xB->p.wch, xB->l); }
  else              { return xstr_eq_cbuf(xA, xB->p.ch, xB->l); }
}

int xstr_eq_cstr(const xStr * xA, const char * cB)
{
  return xstr_eq_cbuf(xA, cB, strlen(cB));
}

/* ---------------------------- Group Separator ---------------------------- */

void xstr_fprint(FILE * stream, const xStr * xA)
{
  if (!xstr_is_empty(xA)) {
    if (xA->wcF) {
      printf("%.*ls", (unsigned int) xA->l, xA->p.wch);
      /* for (size_t i = 0; i < xA->l; i++) { putwchar(xA->p.wch[i]); } */
      /* wprintf("%.*s", xA->l, xA->p.wch); */
    } else {
      printf("%.*s", (unsigned int) xA->l, xA->p.ch);
    }
  }
}

void xstr_print(const xStr * xA)
{
  xstr_fprint(stdout, xA);
}

/* ---------------------------- Group Separator ---------------------------- */

void xstr_empty(xStr * xA)
{
  if (xA->freeF) { free(xA->p.ch); }
  xA->p.ch = NULL;
  xA->wcF = 0;
  xA->l = 0;
  if (xA->ccode) { free(xA->ccode); xA->ccode = NULL; } /* expand character encoding */
}

void xstr_delete(xStr * xA)
{
  xstr_clear(xA);
  free(xA);
}

/* ---------------------------- Group Separator ---------------------------- */

int xstr_test(void)
{
  printf("LC_ALL:%s\n", getenv("LC_ALL"));
  printf("LC_CTYPE:%s\n", getenv("LC_CTYPE"));

  xStr a; xstr_init(&a);
  xStr b; xstr_init_ref(&b, "ref:xxx", 0);
  xStr c; xstr_init_real(&c, strdup("real:xxx:,"), 0);
  xStr d; xstr_init_copy(&d, "copy:xxx,,", 0);

  wchar_t wch = L'$';
  xstr_prep_wcbuf(&c, &wch, 1);

  xstr_app_xstr(&a, &b);
  xstr_app_xstr(&a, &c);
  xstr_app_xstr(&a, &d);

  xstr_prep_xstr(&a, &b);
  xstr_prep_xstr(&a, &c);
  xstr_prep_xstr(&a, &d);

  printf("a: "); xstr_print(&a); endline();
  printf("b: "); xstr_print(&b); endline();
  printf("c: "); xstr_print(&c); endline();
  printf("d: "); xstr_print(&d); endline();

  xstr_clear(&a);
  xstr_clear(&b);
  xstr_clear(&c);
  xstr_clear(&d);

  return 0;
}
