#pragma once
#include "avl-tree.h"
#include "stack.h"
#include "vector.h"
#include <stddef.h>

typedef struct Command Command;

typedef struct Program {
  Command *commands;
  size_t commandsAmount;
} Program;

typedef enum CompileMode {
  COMPILE,
  EXPECT_CUSTOM_WORD,
  NO_COMPILE
} CompileMode;

typedef struct ComputeContext {
  Stack *stack;     // data stack for int values
  int branch_depth; // value of branch (if-else block) depth. 0 for out of
  // if-else block scope

  AvlTree *custom_commands_tree;
  unsigned char skip_depth;
  unsigned long long do_loop_limit;
  unsigned long long do_loop_index;

  unsigned long long current_command_index;
  unsigned long long do_start_command_index;
  CompileMode compile_mode;
  Vector *commands_vector;
  Program *program;
} ComputeContext;

Ret vm_push(ComputeContext *ctx, int value);
Ret vm_pop(ComputeContext *ctx);
Ret vm_top(ComputeContext *ctx);
void vm_execute_command(ComputeContext *ctx, Command *command);
void vm_execute_program(ComputeContext *ctx, Program *program);
