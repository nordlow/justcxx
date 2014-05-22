#include "slocate.h"
#include "utils.h"

char * slocate_call(const char * arg)
{
  char * out = NULL;

  const char * command = "slocate -r \"\\.fix$\"";
  const char * type = "r";

  FILE * stream = popen(command, type);
  if (stream == NULL) { lperror("popen()"); return out; }

  size_t nread;
  size_t nmemb = 4096;
  char buf[nmemb];
  while (!feof(stream) &&
	 !ferror(stream) &&
	 (nread = fread(buf, 1, nmemb, stream))) {
    printf("%.*s", (int)nread, buf);
  }
  printf("\n");

  fclose(stream);
  return out;
}
