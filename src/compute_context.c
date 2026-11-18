#include "compute_context.h"
#include "command.h"
#include "structures/avl_tree/avl-tree.h"
#include "structures/vector/vector.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct CustomWordSubProgramNode {
  const char command_name[64];
  Program subProgram;
} CustomWordSubProgramNode;

int command_comparator(const Key *left, const Key *right) {
  CustomWordSubProgramNode *leftNode = (CustomWordSubProgramNode *)left->data;
  CustomWordSubProgramNode *rightNode = (CustomWordSubProgramNode *)right->data;
  return strcmp(leftNode->command_name, rightNode->command_name);
}

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

  // ctx->program = program;
  size_t commands_amount = vector_size(program->commands);

  for (; ctx->current_command_index < commands_amount;) {
    Node node = {.data = NULL, .size = 0};
    bool ok = vector_get(program->commands, ctx->current_command_index, &node);
    if (!ok) {
      // terminate_compute_context();
    }
    Command *current_command = node.data;
    vm_execute_command(ctx, current_command);
  }
}

void vm_execute_command(ComputeContext *ctx, Command *command) {
  if (ctx->compile_mode == COMPILE) {
    Node wordNode;

    wordNode.size = sizeof(Command);
    wordNode.data = malloc(wordNode.size);
    memcpy(wordNode.data, command, wordNode.size);

    vector_append(ctx->commands_vector, wordNode);
    ctx->current_command_index++;
    return;
  } else if (ctx->compile_mode == EXPECT_CUSTOM_WORD) {
    AvlTree *tree = ctx->custom_commands_tree;

    Key customWord;
    customWord.size = sizeof(*command);
    customWord.data = command;

    bool ret = avl_tree_insert(tree, customWord);
    if (!ret) {
      // TODO: graceful fail
      abort();
    }
    ctx->compile_mode = COMPILE;
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
