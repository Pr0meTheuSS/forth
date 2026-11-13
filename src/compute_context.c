#include "compute_context.h"
#include "command.h"
#include <assert.h>

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
