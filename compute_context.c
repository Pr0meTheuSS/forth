#include "compute_context.h"

Ret vm_push(ComputeContext *ctx, int value) {
  Ret ret = push(&(ctx->stack), value);
  if (ret.error) {
    return (Ret){.value = 0, .error = -1};
  }
  return ret;
}

Ret vm_pop(ComputeContext *ctx) {
  Ret ret = pop(&(ctx->stack));
  if (ret.error) {
    return (Ret){.value = 0, .error = -1};
  }
  return ret;
}

Ret vm_top(ComputeContext *ctx) {
  Ret ret = top(&(ctx->stack));
  if (ret.error) {
    return (Ret){.value = 0, .error = -1};
  }
  return ret;
}
