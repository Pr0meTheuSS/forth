#include <stdio.h>
#include <stdlib.h>

#include "test_program.h"

const char programTargetMessage[] = "Forth programms interpreter.";
const char version[] = "v0.0.1";
const char author[] = " by Y.Olimpiev";

int main(void) {
  char cliWelcomeMessage[100];
  snprintf(cliWelcomeMessage, sizeof(cliWelcomeMessage), "%s %s %s",
           programTargetMessage, version, author);

  printf("%s\n", cliWelcomeMessage);
  test_forth_program();
  return EXIT_SUCCESS;
}
