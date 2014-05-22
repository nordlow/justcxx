#include <stdio.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
  daemon(0,0);                  /* make it a process a daemon */
  sleep(10);                    /* sleep for ten second */
  return 0;
}
