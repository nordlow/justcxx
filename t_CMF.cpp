/*!
 * \file t_CMF.c
 * \brief Testa Centraliserad Multidimensionell Följning.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#include "tvasp/CMF.h"

int main(int argc, char * argv[])
{
  cmf_t cmf;

  if (argc != 1 + 2) {
    printf("usage: %s file1.fix file2.fix\n\n", argv[0]);
    return -1;
  }

  cmf_finit2(&cmf, argv[1], argv[2]);

  for (int i = 0; i < 5; i++) {
    cmf_forward(&cmf);
    sigtab_print(&cmf.stab);
  }

  cmf_clear(&cmf);
  return 0;
}
