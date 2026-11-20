#include "reader/reader.h"
#include <stdio.h>
#include <stdlib.h>

const char programTargetMessage[] = "Forth programms interpreter.";
const char version[] = "v0.1.0";
const char author[] = " by Y.Olimpiev";

int main(void) {
  char cliWelcomeMessage[64];
  snprintf(cliWelcomeMessage, sizeof(cliWelcomeMessage), "%s %s %s",
           programTargetMessage, version, author);

  printf("%s\n", cliWelcomeMessage);

  // TODO: implement REPL
  Reader *reader = create_reader(stdin);
  Word current_word;
  while (reader_get_next_word(reader, &current_word)) {
    printf("%s - read\n", current_word.word);
  }

  destroy_reader(reader);
  return EXIT_SUCCESS;
}
