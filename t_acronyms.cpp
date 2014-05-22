#include <stdio.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "utils.h"
#include "acronyms.h"

void
test_find_acronyms(void)
{
  char *line;
  using_history();
  while (true) {
    line = readline_with_prompt("enter acronym: ");
    if (not line) {
      break;
    }
    find_acronyms("/usr/share/dict/words", line, 1);
    free(line);
  }
}

int
main(void)
{
  test_find_acronyms();
  return 0;
}
