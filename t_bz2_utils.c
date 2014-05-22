#include "../bz2_utils.h"

/*!
 * \file t_bzip2utils.c
 *
 * Test my simplified C api for compressing byte-arrays using libbzip2
 * and bzlib.h.
 */

#include <string.h>
#include <stdio.h>

int
main(int argc, char * argv[])
{
  return test_bzip2();
}
