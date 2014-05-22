#include "alsa_utils.h"

int
main(int argc, char *argv[])
{
  test_alsautils_params();
  /* test_alsautils_write(0); */
  test_alsautils_write(1);
  return 0;
}
