#include "command.h"
#include "compute_context.h"
#include "logger.h"
#include "map_word_to_command.h"
#include "reader/reader.h"
#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const char programTargetMessage[] = "Forth programms interpreter.";
const char version[] = "v0.2.0";
const char author[] = " by Y.Olimpiev";

int main(void) {
  const char log_file[] = "log.log";
  FILE *flog = fopen(log_file, "a+");

  logger_set_stream(flog);
  logger_set_level(LOG_LVL_TRACE);

  int ret_code = EXIT_SUCCESS;
  char cliWelcomeMessage[64];
  snprintf(cliWelcomeMessage, sizeof(cliWelcomeMessage), "%s %s %s",
           programTargetMessage, version, author);

  printf("%s\n", cliWelcomeMessage);

  char run_timestamp[64];
  time_t now = time(NULL);
  struct tm *local = localtime(&now);

  size_t n = strftime(run_timestamp, sizeof(run_timestamp), "%Y-%m-%d %H:%M:%S",
                      local);
  if (n == 0) {
    return 1;
  }
  printf("session start at: %s\n", run_timestamp);
  LOG_DEBUG("main_loop", "session start at: %s", run_timestamp);

  ComputeContext *compute_context = create_compute_context();
  // TODO: implement REPL - move while-circle to other entity
  Reader *reader = create_reader(stdin);
  Word current_word;
  while (reader_get_next_word(reader, &current_word)) {
    LOG_DEBUG("repl", "%s - read", current_word.word);
    Command current_command;
    if (!map_word_to_command(current_word, &current_command)) {
      LOG_ERROR("repl", "Error: unknown command");
      ret_code = EXIT_FAILURE;
      goto cleanup;
    } else {
      LOG_DEBUG("repl", "%s - mapped to command", current_word.word);
      vm_execute_command(compute_context, &current_command);
    }
  }
cleanup:
  destroy_reader(reader);
  destroy_compute_context(compute_context);
  return ret_code;
}
