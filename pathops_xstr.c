#include <string.h>
#include <ctype.h>
#include "pathops_xstr.h"
#include "stdio_x.h"

static xStr g_home_path = { 0, { 0 },0,0,0 };

__attribute__((constructor))
void pathops_init(void)
{
  xstr_init_home_path(&g_home_path);
}

int xstr_is_realpath(const xStr * in, xStr * out)
{
  int ret = 0;
  if (in->wcF) {
  } else {
    char resolved_path[PATH_MAX];
    realpath(in->p.ch, resolved_path);
    PTODO("Use result\n");
  }
  return ret;
}

void xstr_init_pathadd(xStr * full, const xStr * cdir, const xStr * sfil)
{
  size_t full_len = 0;
  char * full_cstr = NULL;
  if (cdir->wcF == 0 &&
      sfil->wcF == 0) {
    full_cstr = cbuf_pathadd(cdir->p.ch, cdir->l,
                             sfil->p.ch, sfil->l,
                             &full_len);
  } else {
    PERR("Handle this case\n");
  }
  xstr_init_real(full, full_cstr, full_len);
}

void xstr_init_pathadd_atHOME(xStr * full,
                              const char * dsub, size_t slen,
                              const char * sfil, size_t flen)
{
  PTEST("Untested!\n");
  char * fullC = cbuf_pathadd3(xstr_get_buf(&g_home_path),
                               xstr_get_size(&g_home_path),
                               dsub, slen,
                               sfil, flen);
  xstr_init_real(full, fullC, 0);
}
