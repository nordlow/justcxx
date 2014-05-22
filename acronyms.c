/*!
 * \file find_acronyms.c
 * \brief Find acronym matches.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include "utils.h"

void
find_acronyms(const char* path, char* acr, int force_first)
{
  size_t len;
  char *buf;
  char *c, *c0, *a;
  int a_len;
  int match;

  if (acr == NULL || strcmp(acr, "") == 0) {
    return;
  }

  buf = fload(path, NULL, &len);

  match = 1;
  a_len = strlen(acr);
  a = acr;
  c0 = c = buf;
  while (c < buf + len) {
    if (force_first) {
      if (c0 == c) {
        if (*a != *c) {
          match = 0;
        }
      }
    }
    if (*c == '\n') {
      if (match && a == (acr + a_len)) {
        printf("%.*s, ", (int)(c - c0), c0);
      }
      a = acr;
      c0 = c + 1;
      match = 1;
    } else if (match && isalpha(*c)) {
      if (a < acr + a_len && *a == *c) {
        a++;
      }
    }
    c++;
  }
  puts("");

  free(buf);
}
