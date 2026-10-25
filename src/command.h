#pragma once

#include "stack.h"

typedef struct ComputeContext ComputeContext;

ComputeContext *create_compute_context();
void destroy_compute_context(ComputeContext *);

typedef enum CommandType {
  COMMAND_INT,  // push int value to data-stack
  COMMAND_SUM,  // sum two top elements from data-stack
  COMMAND_MULT, // mult two top elements from data-stack
  COMMAND_SUB,  // sub two top elements from data-stack
  COMMAND_DIV,  // div two top elements from data-stack
  COMMAND_DUP,  // duplicate top element om data-stack
  COMMAND_DROP, // drop top element from data-stack
  COMMAND_IF,
  COMMAND_ELSE,
  COMMAND_THEN
} CommandType;

typedef struct IntCommand {
  int value;
} IntCommand;

typedef union CommandData {
  IntCommand int_command;
} CommandData;

typedef struct Command {
  CommandType type;
  CommandData data;

  void (*execute)(struct Command *self, ComputeContext *ctx);
} Command;

void execute_int(Command *self, ComputeContext *ctx);
void execute_sum(Command *self, ComputeContext *ctx);
void execute_sub(Command *self, ComputeContext *ctx);
void execute_mult(Command *self, ComputeContext *ctx);
void execute_div(Command *self, ComputeContext *ctx);
void execute_drop(Command *self, ComputeContext *ctx);
void execute_dup(Command *self, ComputeContext *ctx);
void execute_if(Command *self, ComputeContext *ctx);
void execute_else(Command *self, ComputeContext *ctx);
void execute_then(Command *self, ComputeContext *ctx);
