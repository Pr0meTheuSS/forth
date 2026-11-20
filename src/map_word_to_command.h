#pragma once

#include "command.h"
#include "compute_context.h"
#include "reader/reader.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  const char *name;
  Command command;
} CmdEntry;

static CmdEntry cmds[] = {
    {"+", {.type = COMMAND_SUM, .execute = execute_sum, .command_name = "+"}},
    {"-", {.type = COMMAND_SUB, .execute = execute_sub, .command_name = "-"}},
    {"*", {.type = COMMAND_MULT, .execute = execute_mult, .command_name = "*"}},
    {"/", {.type = COMMAND_DIV, .execute = execute_div, .command_name = "/"}},
    {":",
     {.type = COMMAND_COMPILE,
      .execute = execute_compile,
      .command_name = ":"}},
    {";",
     {.type = COMMAND_END_COMPILE,
      .execute = execute_end_compile,
      .command_name = ";"}},
    {"DUP",
     {.type = COMMAND_DUP, .execute = execute_dup, .command_name = "DUP"}},
    {"SWAP",
     {.type = COMMAND_SWAP, .execute = execute_swap, .command_name = "SWAP"}},
    {"ROT",
     {.type = COMMAND_ROT, .execute = execute_rot, .command_name = "ROT"}},
    {"OVER",
     {.type = COMMAND_OVER, .execute = execute_over, .command_name = "OVER"}},
    {"DROP",
     {.type = COMMAND_DROP, .execute = execute_drop, .command_name = "DROP"}},
    {"DO", {.type = COMMAND_DO, .execute = execute_do, .command_name = "DO"}},
    {"LOOP",
     {.type = COMMAND_LOOP, .execute = execute_loop, .command_name = "LOOP"}},
    {"IF", {.type = COMMAND_IF, .execute = execute_if, .command_name = "IF"}},
    {"ELSE",
     {.type = COMMAND_ELSE, .execute = execute_else, .command_name = "ELSE"}},
    {"THEN",
     {.type = COMMAND_THEN, .execute = execute_then, .command_name = "THEN"}},
    {".",
     {.type = COMMAND_PRINT, .execute = execute_print, .command_name = "."}},
};

/* возвращает опкод или -1, если команда не найдена */
inline static Command lookup_cmd(const char *str) {
  for (size_t i = 0; i < sizeof(cmds) / sizeof(cmds[0]); ++i)
    if (strcmp(cmds[i].name, str) == 0)
      return cmds[i].command;
  return (Command){.type = UNKNOWN};
}

static bool map_word_to_command(const Word word, Command *command) {
  assert(command);
  strncpy(command->command_name, word.word, sizeof(word.word));
  Command cmd = lookup_cmd(word.word);
  // TODO: make fallback to parse int more pretty
  if (cmd.type == UNKNOWN) {
    char *endptr;
    long num = strtol(word.word, &endptr, 10);
    if (endptr == &word.word[strlen(word.word)]) {
      cmd = (Command){
          .type = COMMAND_INT, .execute = execute_int, .command_name = "int"};
      cmd.data.int_command.value = (int)num;
    } else {
      // // TODO handle custom command
      // cmd =
      //     (Command){.type = COMMAND_CUSTOM, .execute =
      //     execute_custom_command};
      // strncpy(cmd.command_name, word.word, sizeof(cmd.command_name));
      return false;
    }
  }
  memcpy(command, &cmd, sizeof(*command));
  return true;
}
