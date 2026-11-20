#include "reader/reader.h"
#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const char programTargetMessage[] = "Forth programms interpreter.";
const char version[] = "v0.1.0";
const char author[] = " by Y.Olimpiev";

int main(void) {
  char cliWelcomeMessage[64];
  snprintf(cliWelcomeMessage, sizeof(cliWelcomeMessage), "%s %s %s",
           programTargetMessage, version, author);

  printf("%s\n", cliWelcomeMessage);

  char run_timestamp[64];
  time_t now = time(NULL);
  struct tm *local = localtime(&now);

  // Формат: YYYY-MM-DD HH:MM:SS
  size_t n = strftime(run_timestamp, sizeof(run_timestamp), "%Y-%m-%d %H:%M:%S",
                      local);
  if (n == 0) {
    // Буфер слишком мал — надо увеличить
    return 1;
  }
  printf("session start at: %s\n", run_timestamp);
  // TODO: implement REPL
  Reader *reader = create_reader(stdin);
  Word current_word;
  while (reader_get_next_word(reader, &current_word)) {
    printf("%s - read\n", current_word.word);
  }

  destroy_reader(reader);
  return EXIT_SUCCESS;
}
