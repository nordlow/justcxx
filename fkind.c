#include "fkind.h"
#include "substr_match.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

FKIND_t FKIND_from_stat_mode(mode_t mode)
{
  FKIND_t fkind = FKIND_undefined_;
  if        (S_ISREG(mode))  { fkind = FKIND_REGULAR; }
  else if   (S_ISDIR(mode))  { fkind = FKIND_DIR; }
  else if   (S_ISCHR(mode))  { fkind = FKIND_SPECIAL_CHR; }
  else if   (S_ISBLK(mode))  { fkind = FKIND_SPECIAL_BLK; }
  else if   (S_ISFIFO(mode)) { fkind = FKIND_SPECIAL_FIFO; }
#ifdef S_ISLNK
  else if   (S_ISLNK(mode))  { fkind = FKIND_SPECIAL_LNK; }
#endif
#ifdef S_ISSOCK
  else if   (S_ISSOCK(mode)) { fkind = FKIND_SPECIAL_SOCK; }
#endif
  return fkind;
}

const char * FKIND_getName(FKIND_t fkind)
{
  const char * str = NULL;
  switch (fkind) {
  case FKIND_any_: str = "Any"; break;
  case FKIND_DIR: str = "Directory"; break;
  case FKIND_SPECIAL_LNK: str = "Special Link"; break;
  case FKIND_SPECIAL_LNK_SOFT: str = "Special Soft Link"; break;
  case FKIND_SPECIAL_LNK_HARD: str = "Special Hard Link"; break;
  case FKIND_SPECIAL_CHR: str = "Special Character Device"; break;
  case FKIND_SPECIAL_BLK: str = "Special Block Device"; break;
  case FKIND_SPECIAL_FIFO: str = "Special FIFO"; break;
  case FKIND_SPECIAL_SOCK: str = "Special Socket"; break;
  case FKIND_REGULAR: str = "Regular/Ordinary"; break;
  case FKIND_undefined_: str = "Undefined"; break;
  }
  return str;
}
