#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "compute_context.h"
#include "structures/avl_tree/avl-tree.h"
#include "structures/vector/vector.h"

int command_comparator(const void *left, const void *right) {
  CustomWordSubProgramNode *leftNode = (CustomWordSubProgramNode *)left;
  CustomWordSubProgramNode *rightNode = (CustomWordSubProgramNode *)right;
  return strcmp(leftNode->command_name, rightNode->command_name);
}

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

void execute_register_custom_word(Command *self, ComputeContext *ctx) {
  if (self->type != COMMAND_REGISTER_CUSTOM_WORD) {
    return;
  }
  if (ctx->compile_mode != EXPECT_CUSTOM_WORD) {
    return;
  }

  AvlTree *tree = ctx->custom_commands_tree;
  CustomWordSubProgramNode subProgramNode;
  subProgramNode.subProgram = malloc(sizeof(Program));
  strncpy(subProgramNode.command_name, self->command_name,
          sizeof(subProgramNode.command_name));
  strncpy(ctx->current_compiling_command_name, self->command_name,
          sizeof(self->command_name));

  bool ret = avl_tree_insert(tree, &subProgramNode, sizeof(subProgramNode));
  if (!ret) {
    // TODO: graceful fail
    abort();
  }
  ctx->compile_mode = COMPILE;
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

  AvlTree *tree = ctx->custom_commands_tree;

  const Key *custom_command =
      avl_tree_find(tree, ctx->current_compiling_command_name,
                    sizeof(ctx->current_compiling_command_name));
  if (!custom_command) {
    // TODO: fail
  }
  CustomWordSubProgramNode *subProgramNode = custom_command->data;
  subProgramNode->subProgram->commands = create_vector();

  size_t commandsAmount = vector_size(ctx->commands_vector);
  for (size_t i = 0; i < commandsAmount; ++i) {
    Command command;
    bool ok = vector_get(ctx->commands_vector, i, &command, sizeof(command));
    if (!ok) {
      // TODO: graceful shutdown
      abort();
    }
    ok = vector_append(subProgramNode->subProgram->commands, &command,
                       sizeof(command));
    if (!ok) {
      // TODO: graceful shutdown
      abort();
    }
  }

  // erase_vector(ctx->commands_vector)
}

void execute_custom_command(Command *self, ComputeContext *ctx) {
  if (self->type != UNKNOWN) {
    return;
  }
  AvlTree *tree = ctx->custom_commands_tree;

  CustomWordSubProgramNode subProgramNode;
  strncpy(subProgramNode.command_name, self->command_name,
          sizeof(subProgramNode.command_name));

  const Key *customCommandNode =
      avl_tree_find(tree, &subProgramNode, sizeof(subProgramNode));
  if (!customCommandNode) {
    // TODO: graceful fail
    abort();
  }

  CustomWordSubProgramNode *subProgramNodePtr = customCommandNode->data;
  Program *subProgram = subProgramNodePtr->subProgram;

  ComputeContext *inner_ctx = create_compute_context();
  Stack *inner_stack = inner_ctx->stack;
  inner_ctx->stack = ctx->stack;

  vm_execute_program(inner_ctx, subProgram);

  inner_ctx->stack = inner_stack;
  destroy_compute_context(inner_ctx);
}
