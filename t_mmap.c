#include <stdio.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int test_mmap(size_t size, bool align_flag)
{
  int ret = 0;
  const int fd = open("dummy", O_RDWR | O_CREAT | O_TRUNC, 0666);

  void * buf = NULL;
  if (align_flag) {
    buf = memalign(sysconf(_SC_PAGESIZE), size);
  } else {
    buf = malloc(size);
  }

  if (align_flag)
    printf("ALIGNED: \n");
  else
    printf("UNALIGNED: \n");

  if (buf) {
    void *abuf = NULL;        /* alignment adjusted buffer */
    if ((abuf = mmap(buf, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
      printf("mmap(%p, SHARED): %s\n", buf, strerror(errno));
      ret = -1;                 /* signal error */
    } else {
      printf("mmap(%p, SHARED) => %p\n", buf, abuf);
      munmap(buf, size);
      if (ret == -1) {
        printf("munmap(%p): %s\n", buf, strerror(errno));
      }
    }
  }

  if (buf) {
    void *abuf = NULL;        /* alignment adjusted buffer */
    if ((abuf = mmap(buf, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0)) == MAP_FAILED) {
      printf("mmap(%p, SHARED|FIXED): %s\n", buf, strerror(errno));
      ret = -1;                 /* signal error */
    } else {
      printf("mmap(%p, SHARED|FIXED) => %p\n", buf, abuf);
      munmap(buf, size);
      if (ret == -1) {
        printf("munmap(%p): %s\n", buf, strerror(errno));
      }
    }
  }

  if (buf) {
    void *abuf = NULL;        /* alignment adjusted buffer */
    if ((abuf = mmap(buf, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_FIXED, fd, 0)) == MAP_FAILED) {
      printf("mmap(%p, PRIVATE|FIXED): %s\n", buf, strerror(errno));
      ret = -1;                 /* signal error */
    } else {
      printf("mmap(%p, PRIVATE|FIXED) => %p\n", buf, abuf);
      int ret = munmap(buf, size);
      if (ret == -1) {
        printf("munmap(%p): %s\n", buf, strerror(errno));
      }
    }
  }

  free(buf);
  close(fd);

  return ret;
}

int main(int argc, char * argv[])
{
  printf("pagesize:%ld\n", sysconf(_SC_PAGE_SIZE));
  test_mmap(4096, true);
  return 0;
}
