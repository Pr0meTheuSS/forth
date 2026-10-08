#include <assert.h>

#include "command.h"

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
