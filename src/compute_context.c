#include "compute_context.h"
#include "avl-tree.h"
#include "command.h"
#include "vector.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

ComputeContext *create_compute_context() {
  ComputeContext *ret = (ComputeContext *)malloc(sizeof(ComputeContext));
  if (!ret) {
    return NULL;
  }
  ret->custom_commands_tree = create_avl_tree();
  if (!ret->custom_commands_tree) {
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
  ret->compile_mode = NO_COMPILE;
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

  ctx->program = program;
  for (; ctx->current_command_index < program->commandsAmount;) {
    Command *current_command = &program->commands[ctx->current_command_index];

    vm_execute_command(ctx, current_command);
  }
}
typedef struct CustomWordSubProgramNode {
  Command command;
  Program subProgram;
} CustomWordSubProgramNode;

int command_comparator(const Key *left, const Key *right) {
  CustomWordSubProgramNode *leftNode = (CustomWordSubProgramNode *)left->data;
  CustomWordSubProgramNode *rightNode = (CustomWordSubProgramNode *)right->data;
  return strcmp(leftNode->command.command_name,
                rightNode->command.command_name);
}

void vm_execute_command(ComputeContext *ctx, Command *command) {

  if (ctx->compile_mode == COMPILE) {
    Node wordNode;

    wordNode.size = sizeof(Command);
    wordNode.data = command;

    vector_append(ctx->commands_vector, wordNode);
    ctx->current_command_index++;
    return;
  } else if (ctx->compile_mode == EXPECT_CUSTOM_WORD) {
    AvlTree *tree = ctx->custom_commands_tree;

    Key customWord;
    customWord.size = sizeof(*command);
    customWord.data = command;
    customWord.comp = command_comparator;

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
