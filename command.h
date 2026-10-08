#pragma once

#include "compute_context.h"
#include "stack.h"

typedef enum CommandType {
  COMMAND_INT,  // push int value to data-stack
  COMMAND_SUM,  // sum two top elements from data-stack
  COMMAND_MULT, // mult two top elements from data-stack
  COMMAND_SUB,  // sub two top elements from data-stack
  COMMAND_DIV,  // div two top elements from data-stack
  COMMAND_DUP,  // duplicate top element om data-stack
  COMMAND_DROP  // drop top element from data-stack
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

  void (*execute)(struct Command *self, ComputeContext *vm);
} Command;

void execute_int(Command *self, ComputeContext *vm);
void execute_sum(Command *self, ComputeContext *vm);
void execute_sub(Command *self, ComputeContext *vm);
void execute_mult(Command *self, ComputeContext *vm);
void execute_div(Command *self, ComputeContext *vm);
void execute_drop(Command *self, ComputeContext *vm);
void execute_dup(Command *self, ComputeContext *vm);