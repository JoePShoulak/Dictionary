#include <string>    /* for strings          */
#include <stdio.h>   /* for I/O              */
#include <stdlib.h>  /* for good measure     */

/* TODO: Get to work, then include in server */

void Crash(char errorType[])
{
  char errorMessage[128];
  for (int i=0; i<128; ++i)
  {
    errorMessage[i] = 0;
  }
  errorMessage = "DICTIONARY: Fatal " + errorType + " error.\n";
  fprintf(stderr, errorMessage);
  exit(EXIT_FAILURE);
}
