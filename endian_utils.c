#include "endian_utils.h"
#include <string.h>

ENDIAN_t
get_ENDIAN(void)
{
  unsigned long x = 1;
  char buf[sizeof(x)];
  memcpy(buf, &x, sizeof(x));
  if (buf[0] == 1) {
    return CPU_LE;
  } else {
    return CPU_BE;
  }
}
