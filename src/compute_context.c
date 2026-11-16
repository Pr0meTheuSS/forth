#include "compute_context.h"
#include "avl-tree.h"
#include "command.h"

#include <assert.h>
#include <stdlib.h>

ComputeContext *create_compute_context() {
  ComputeContext *ret = (ComputeContext *)malloc(sizeof(ComputeContext));
  if (!ret) {
    return NULL;
  }
  ret->tree = create_avl_tree();
  if (!ret->tree) {
    return NULL;
  }

  ret->branch_depth = 0;
  ret->skip_depth = 0;
  ret->stack = create_stack();
  ret->current_command_index = 0;
  ret->do_loop_index = 0;
  ret->do_loop_limit = 0;
  ret->do_start_command_index = 0;
  ret->compile_mode = false;
  return ret;
}

void destroy_compute_context(ComputeContext *ctx) {
  if (!ctx) {
    return;
  }
  destroy_avl_tree(ctx->tree);
  destroy_stack(ctx->stack);
  free(ctx);
}

int is_command_flow_control(const Command *command) {
  return (command->type == COMMAND_IF || command->type == COMMAND_ELSE ||
          command->type == COMMAND_THEN);
}

void vm_execute_program(ComputeContext *ctx, Program *program) {
  assert(ctx);
  assert(program);

  for (; ctx->current_command_index < program->commandsAmount;) {
    Command *current_command = &program->commands[ctx->current_command_index];

    vm_execute_command(ctx, current_command);
  }
}

void vm_execute_command(ComputeContext *ctx, Command *command) {
  // if (ctx->compile_mode) {
  //     avl_tree_find(ctx->tree, Key key);
  // }

  if (ctx->skip_depth && !is_command_flow_control(command)) {
    return;
  }
  command->execute(command, ctx);
  ctx->current_command_index++;
}

Ret vm_push(ComputeContext *ctx, int value) {
  Ret ret = push(ctx->stack, value);
  if (ret.error) {
    return (Ret){.value = 0, .error = -1};
  }
  return ret;
}

Ret vm_pop(ComputeContext *ctx) {
  Ret ret = pop(ctx->stack);
  if (ret.error) {
    return (Ret){.value = 0, .error = -1};
  }
  return ret;
}

Ret vm_top(ComputeContext *ctx) {
  Ret ret = top(ctx->stack);
  if (ret.error) {
    return (Ret){.value = 0, .error = -1};
  }
  return ret;
}
