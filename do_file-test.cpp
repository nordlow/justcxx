/*!
 * \file do_file-test.cpp
 * \brief Test do_file.h
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-04-04 12:48
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "fshow.hpp"
#include "do_file.h"

// TODO: Use <kbd>C-h</kbd> and <code>format</code>

void fprintf_html(FILE* file,
                  const char *title = NULL,
                  const char *lang = NULL)
{
  const char * format = "\
<html lang=\"%s\">\n\
<head>\n\
\
<title>%s</title>\n\
\
</head>\n\
<body>\n\
First paragraph.\n\
<p></p>\n\
Second paragraph.\n\
</body>\n\
</html>\n\
<kbd>C-h</kbd>\n\
";
  if (not lang) { lang = getenv("LANG"); } // default language
  fprintf(file, format, lang, title);
}

int main(int argc, char * argv[], char *envp[])
{
  const char * outfile = "/tmp/output.html";

  FILE * fi = fopen(outfile, "wb+");
  fprintf_html(fi, "Program Report Title");
  fclose(fi);

  pid_t child_pid = fork();
  if (child_pid == 0) {         /* in child process */
    do_file("/tmp/output.html", envp, NULL, false); /* If do_file returns, it must have failed. */
    printf("Unknown command\n");
    exit(0);
  }
  else {
    /* This is run by the parent.  Wait for the child to terminate. */
    pid_t tpid;
    int child_status;
    do {
      tpid = wait(&child_status);
      if (tpid != child_pid) {
        /* TODO: process tpid terminated */
      }
    } while (tpid != child_pid);
    return child_status;
  }

  return 0;
}
