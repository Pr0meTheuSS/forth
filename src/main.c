#include <stdio.h>
#include <stdlib.h>

const char programTargetMessage[] = "Forth programms interpreter.";
const char version[] = "v0.0.3";
const char author[] = " by Y.Olimpiev";

int main(void) {
  char cliWelcomeMessage[64];
  snprintf(cliWelcomeMessage, sizeof(cliWelcomeMessage), "%s %s %s",
           programTargetMessage, version, author);

  printf("%s\n", cliWelcomeMessage);
  // TODO: implement REPL
  return EXIT_SUCCESS;
}
