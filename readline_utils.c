#include "readline_utils.h"
#include <unistd.h>

#ifdef HAVE_READLINE_READLINE_H
#  include <readline/readline.h>
#  include <readline/history.h>
#endif

char * readline_with_prompt(const char *prompt)
{
  char * line = NULL;
#ifdef HAVE_READLINE_READLINE_H
  line = readline(prompt);
  if (line && strcmp(line, "") != 0) {
    add_history(line);		/* Only add non-empty lines. */
  } else {
    puts("");
  }
#endif
  return line;
}
