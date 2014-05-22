#include <stdio.h>
#include <stdlib.h>

/*!
 * Testing this function on linux-2.6.13-gentoo-r3 with glibc-2.3.5
 * gave an overhead of <b>16 bytes</b> for every zero length region allocated.
 */
void
test_realloc(void)
{
  void * ret = realloc(NULL, 0);
  printf("realloc(NULL, 0) => %p\n", ret);
  free(ret);

  size_t len = 1024*1024;
  void *x[len];
  int ch = 0;

  printf("Press any key to call allocate %zd regions of length 0\n", len);
  ch = getchar();
  for (size_t i = 0; i < len; i++) { x[i] = malloc(0); }

  printf("Press any key to call free all %zd regions of length 0\n", len);
  ch = getchar();
  for (size_t i = 0; i < len; i++) { free(x[i]); }
}

int
main(int argc, char *argv[])
{
  test_realloc();
  return 0;
}
