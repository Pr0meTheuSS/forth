#pragma once
#include "stack.h"

typedef struct Command Command;
typedef struct ComputeContext {
  Stack *stack;     // data stack for int values
  int branch_depth; // value of branch (if-else block) depth. 0 for out of
                    // if-else block scope
  unsigned char skip_depth;
} ComputeContext;

Ret vm_push(ComputeContext *ctx, int value);
Ret vm_pop(ComputeContext *ctx);
Ret vm_top(ComputeContext *ctx);
void vm_execute_command(ComputeContext *ctx, Command *command);
