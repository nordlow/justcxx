#include "statutils.h"

uid_t g_uid;
gid_t g_gid;

__attribute__ ((constructor))
void statutils_init_uid_gid(void)
{
  g_uid = getuid();
  g_gid = getgid();
}
