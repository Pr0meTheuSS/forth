#include "compute_context.h"
#include "command.h"
#include "structures/avl_tree/avl-tree.h"
#include "structures/vector/vector.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

ComputeContext *create_compute_context() {
  ComputeContext *ret = (ComputeContext *)malloc(sizeof(ComputeContext));
  if (!ret) {
    return NULL;
  }

  ret->branch_depth = 0;
  ret->skip_depth = 0;
  ret->current_command_index = 0;
  ret->do_loop_index = 0;
  ret->do_loop_limit = 0;
  ret->do_start_command_index = 0;
  ret->compile_mode = NO_COMPILE;

  ret->custom_commands_tree = create_avl_tree(command_comparator);
  if (!ret->custom_commands_tree) {
    return NULL;
  }
  ret->stack = create_stack();
  ret->commands_vector = create_vector();
  return ret;
}

void destroy_compute_context(ComputeContext *ctx) {
  if (!ctx) {
    return;
  }
  const AvlTree *iter =
      avl_tree_get_next_iterator(ctx->custom_commands_tree, NULL);

  while (iter) {
    CustomWordSubProgramNode *node =
        (CustomWordSubProgramNode *)iter->key->data;
    destroy_vector(node->subProgram->commands);
    free(node->subProgram);
    iter = avl_tree_get_next_iterator(ctx->custom_commands_tree, iter);
  }

  destroy_avl_tree(ctx->custom_commands_tree);
  destroy_vector(ctx->commands_vector);
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

  size_t commands_amount = vector_size(program->commands);
  Command current_command;

  for (; ctx->current_command_index < commands_amount;) {

    bool ok = vector_get(program->commands, ctx->current_command_index,
                         &current_command, sizeof(current_command));
    if (!ok) {

      // terminate_compute_context();
      abort();
    }
    vm_execute_command(ctx, &current_command);
  }
}

void vm_execute_command(ComputeContext *ctx, Command *command) {
  if (ctx->compile_mode == COMPILE) {
    if (command->type == COMMAND_END_COMPILE) {
      ctx->compile_mode = NO_COMPILE;
      return;
    }
    vector_append(ctx->commands_vector, command, sizeof(*command));
    ctx->current_command_index++;
    return;
  }

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
