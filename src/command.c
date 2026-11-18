#include <assert.h>

#include "command.h"
#include "compute_context.h"

void execute_int(Command *self, ComputeContext *vm) {
  if (self->type != COMMAND_INT) {
    return;
  }
  vm_push(vm, self->data.int_command.value);
}

void execute_sum(Command *self, ComputeContext *vm) {
  if (self->type != COMMAND_SUM) {
    return;
  }
  // TODO: handle ret error
  Ret right = vm_pop(vm);
  Ret left = vm_pop(vm);
  vm_push(vm, left.value + right.value);
}

void execute_sub(Command *self, ComputeContext *vm) {
  if (self->type != COMMAND_INT) {
    return;
  }
  Ret right = vm_pop(vm);
  Ret left = vm_pop(vm);
  vm_push(vm, left.value - right.value);
}

void execute_mult(Command *self, ComputeContext *vm) {
  if (self->type != COMMAND_MULT) {
    return;
  }
  Ret right = vm_pop(vm);
  Ret left = vm_pop(vm);
  vm_push(vm, left.value * right.value);
}

void execute_div(Command *self, ComputeContext *vm) {
  if (self->type != COMMAND_DIV) {
    return;
  }

  Ret right = vm_pop(vm);
  Ret left = vm_pop(vm);
  // TODO: handle dvision by zero
  vm_push(vm, left.value / right.value);
}

void execute_drop(Command *self, ComputeContext *vm) {
  if (self->type != COMMAND_DROP) {
    return;
  }
  vm_pop(vm);
}

void execute_dup(Command *self, ComputeContext *vm) {
  if (self->type != COMMAND_DUP) {
    return;
  }
  Ret top = vm_top(vm);
  vm_push(vm, top.value);
}

// обработчики control-flow меняют счётчик:
void execute_if(Command *self, ComputeContext *ctx) {
  if (self->type != COMMAND_IF) {
    return;
  }
  Ret cond = vm_top(ctx);
  if (ctx->skip_depth > 0) {
    ctx->skip_depth++; // вложенный IF внутри пропускаемого блока
    return;
  }
  if (!cond.value)
    ctx->skip_depth = 1;
}

void execute_else(Command *self, ComputeContext *ctx) {
  if (self->type != COMMAND_ELSE) {
    return;
  }
  if (ctx->skip_depth == 1)
    ctx->skip_depth = 0; // переключаемся на ветку ELSE
  else
    ctx->skip_depth++; // skip_depth > 1 — вложенный, не трогаем
}

void execute_then(Command *self, ComputeContext *ctx) {
  if (self->type != COMMAND_THEN) {
    return;
  }
  if (ctx->skip_depth > 0)
    ctx->skip_depth--;
}

void execute_do(Command *self, ComputeContext *ctx) {
  if (self->type != COMMAND_DO) {
    return;
  }
  ctx->do_loop_limit = vm_pop(ctx).value;
  ctx->do_loop_index = vm_pop(ctx).value;
  // запоминаем индекс стартовой команды цикла как индекс команды DO
  ctx->do_start_command_index = ctx->current_command_index;
}

void execute_loop(Command *self, ComputeContext *ctx) {
  if (self->type != COMMAND_LOOP) {
    return;
  }
  ctx->do_loop_index++;

  if (ctx->do_loop_index < ctx->do_loop_limit) {
    // перекидываем индекс следующей к исполнению команды в программе на начало
    // цикла
    ctx->current_command_index = ctx->do_start_command_index;
  } else {
    // сбрасываем счётчики цикла при выполнении всех итераций
    ctx->do_loop_limit = ctx->do_loop_index = 0;
  }
}

void execute_compile(Command *self, ComputeContext *ctx) {
  if (self->type != COMMAND_COMPILE) {
    return;
  }
  ctx->compile_mode = EXPECT_CUSTOM_WORD;
}

void execute_end_compile(Command *self, ComputeContext *ctx) {
  if (self->type != COMMAND_END_COMPILE) {
    return;
  }
  ctx->compile_mode = NO_COMPILE;
}
