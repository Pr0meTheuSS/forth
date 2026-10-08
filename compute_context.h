#pragma once
#include "stack.h"

typedef struct ComputeContext {
  Stack stack;
} ComputeContext;

Ret vm_push(ComputeContext *ctx, int value);
Ret vm_pop(ComputeContext *ctx);
Ret vm_top(ComputeContext *ctx);