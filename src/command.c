#include <assert.h>

#include "command.h"
#include "compute_context.h"

void execute_int(Command *self, ComputeContext *vm) {
  assert(self->type == COMMAND_INT);
  vm_push(vm, self->data.int_command.value);
}

void execute_sum(Command *self, ComputeContext *vm) {
  assert(self->type == COMMAND_SUM);
  // TODO: handle ret error
  Ret right = vm_pop(vm);
  Ret left = vm_pop(vm);
  vm_push(vm, left.value + right.value);
}

void execute_sub(Command *self, ComputeContext *vm) {
  assert(self->type == COMMAND_INT);

  Ret right = vm_pop(vm);
  Ret left = vm_pop(vm);
  vm_push(vm, left.value - right.value);
}

void execute_mult(Command *self, ComputeContext *vm) {
  assert(self->type == COMMAND_MULT);

  Ret right = vm_pop(vm);
  Ret left = vm_pop(vm);
  vm_push(vm, left.value * right.value);
}

void execute_div(Command *self, ComputeContext *vm) {
  assert(self->type == COMMAND_DIV);

  Ret right = vm_pop(vm);
  Ret left = vm_pop(vm);
  // TODO: handle dvision by zero
  vm_push(vm, left.value / right.value);
}

void execute_drop(Command *self, ComputeContext *vm) {
  assert(self->type == COMMAND_DROP);
  vm_pop(vm);
}

void execute_dup(Command *self, ComputeContext *vm) {
  assert(self->type == COMMAND_DUP);

  Ret top = vm_top(vm);
  vm_push(vm, top.value);
}

// обработчики control-flow меняют счётчик:
void execute_if(Command *self, ComputeContext *ctx) {
  assert(self->type == COMMAND_IF);
  Ret cond = vm_top(ctx);
  if (ctx->skip_depth > 0) {
    ctx->skip_depth++; // вложенный IF внутри пропускаемого блока
    return;
  }
  if (!cond.value)
    ctx->skip_depth = 1;
}

void execute_else(Command *self, ComputeContext *ctx) {
  assert(self->type == COMMAND_ELSE);
  if (ctx->skip_depth == 1)
    ctx->skip_depth = 0; // переключаемся на ветку ELSE
  else
    ctx->skip_depth++; // skip_depth > 1 — вложенный, не трогаем
}

void execute_then(Command *self, ComputeContext *ctx) {
  assert(self->type == COMMAND_THEN);
  if (ctx->skip_depth > 0)
    ctx->skip_depth--;
}

void execute_do(Command *self, ComputeContext *ctx) {
  assert(self->type == COMMAND_DO);

  ctx->do_loop_limit = vm_pop(ctx).value;
  ctx->do_loop_index = vm_pop(ctx).value;
  // запоминаем индекс стартовой команды цикла как индекс команды DO
  ctx->do_start_command_index = ctx->current_command_index;
}

void execute_loop(Command *self, ComputeContext *ctx) {
  assert(self->type == COMMAND_LOOP);
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
  assert(self->type == COMMAND_COMPILE);
  // ctx->compile_mode = true;
  // Key newCompileWord;
  // struct {
  //     char name[64],
  //     Program* subProgram
  // } compileWorldEnrty;
}

void execute_end_compile(Command *self, ComputeContext *ctx) {
  assert(self->type == COMMAND_END_COMPILE);
  // ctx->compile_mode = false;
}
