#pragma once

#include "stack.h"

typedef struct VM {
  Stack stack;
} VM;

typedef enum { COMMAND_INT, COMMAND_SUM, COMMAND_MULT } CommandType;

typedef struct {
  int value;
} IntCommand;

typedef union {
  IntCommand int_command;
} CommandData;

typedef struct {
  CommandType type;
  CommandData data;

  void (*execute)(VM *vm, const CommandData *data);
} Command;

void vm_push(VM *vm, int value) {
  Ret ret = push(&(vm->stack), value);
  if (ret.error) {
    // TODO: make VM errors
    exit(1);
  }
}

int vm_pop(VM *vm) {
  Ret ret = pop(&(vm->stack));
  if (ret.error) {
    // TODO: make VM errors
    exit(1);
  }
  return ret.value;
}

void execute_int(VM *vm, const CommandData *data) {
  vm_push(vm, data->int_command.value);
}

void execute_sum(VM *vm, const CommandData *data) {
  (void)data;
  int b = vm_pop(vm);
  int a = vm_pop(vm);
  vm_push(vm, a + b);
}

void execute_mult(VM *vm, const CommandData *data) {
  (void)data;
  int b = vm_pop(vm);
  int a = vm_pop(vm);
  vm_push(vm, a * b);
}
