/*!
 * \file Illustrate use of signals
 *
 * This is like the Unix `grep' command; it will report all lines
 * in a file that contain the given string; however, this one is
 * different:  the user specifies the string first, and then the
 * program prompts the user for the names of the files to check
 * the string for, one file at a time
 *
 * reading a large file will take some time, and while waiting
 * the user may change his/her mind, and withdraw the command
 * to check this particular file (the given string is still
 * valid, though, and will be used on whatever further files
 * the user specifies); this is where the signals are used
 */

#define MaxLineLength 80

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>

jmp_buf(GetFileName);

char Line[MaxLineLength],	/* one line from an input file */
  String[MaxLineLength],	/* current partial-word */
  FileName[50];			/* input file name */

int StringLength;		/* current length of the partial-word */

FILE *FPtr;

/*!
 * Read one line of the file.
 *
 * \return a value which will be the line length if it successfully
 * reads a line, or -1 if the end of the file was encountered
 */
int
ReadLine(void)
{
  char C;
  int LineLength;

  LineLength = 0;
  while (1) {
    if (fscanf(FPtr, "%c", &C) == -1)
      return -1;
    if (C != '\n')
      Line[LineLength++] = C;
    else {
      Line[LineLength] = 0;
      break;
    }
  }
  return LineLength;
}

void
FindAllMatches(void)
{
  int LL, J, MaxJ;

  FPtr = fopen(FileName, "r");
  while (1) {
    LL = ReadLine();
    if (LL == -1)
      break;
    MaxJ = LL - StringLength + 1;
    for (J = 0; J < MaxJ; J++)
      if (!strncmp(String, Line + J, StringLength)) {
	printf("%s\n", Line);
	break;
      }
  }
}

void
CtrlC(int arg)
{
  printf("OK, forget that file (arg=%d\n", arg);
  longjmp(GetFileName, 0);
}

int
main(void)
{
  char NewLineChar;

  signal(SIGINT, CtrlC);
  printf("string?\n");
  scanf("%s", String);
  scanf("%c", &NewLineChar);
  StringLength = strlen(String);

  while (1) {
    setjmp(GetFileName);
    printf("file name?\n");
    scanf("%s", FileName);
    if (!strcmp(FileName, "q"))
      exit(0);
    FindAllMatches();
  }

  return 0;
}
