#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../utils.h"

/*!
 * Test writing of files to disk.
 *
 * I have used it to benchmark and error-check NFS mounted file systems.
 */
int
test_fwrite(const char *dir)
{
  FILE *f;
  char fname[1024];
  char path[1024];
  uint m, n;
  uint i, j;
  char *buf;

  printf("m:"); scanf("%d", &m);
  printf("n:"); scanf("%d", &n);

  for (i = 0; i <= m; i++) {
    for (j = 1; j <= n; j++) {
      sprintf(fname, "fwritetest_%05d_%05d", i, j);
      sprintf(path, "%s/%s", dir, fname);

      f = fopen(path, "w");
      if (f) {
	lprintf("successfully opened file: %s\n", path);
      } else {
	leprintf("error: could not open file: %s: %s\n", path, strerror(errno));
	return -1;
      }

      buf = (char*)malloc(j);
      size_t ret = fwrite(buf, 1, j, f);

      if (ret == j) {
	lprintf("wrote %d bytes to file: %s\n", j, path);
      } else {
	leprintf("error: could not write to file: %s: %s\n",
		 path, strerror(errno));
	return -1;
      }

      free(buf);
      fclose(f);
    }
  }

  return 0;
}

int main(int argc, char *argv[])
{
  return test_fwrite(".");
}
