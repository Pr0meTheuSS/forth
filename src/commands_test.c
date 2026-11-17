#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "test.h"

#include "command.h"
#include "compute_context.h"
#include "vector.h"

bool forth_simple_calculation_test() {
  ComputeContext *compute_context = create_compute_context();

  /*
  42 52 10
  SUM
  MULT
  DUP
  DROP
  DUP
  DIV
  */
  Command commands[] = {
      {.type = COMMAND_INT,
       .data.int_command = {.value = 42},
       .execute = execute_int},
      {.type = COMMAND_INT,
       .data.int_command = {.value = 52},
       .execute = execute_int},
      {.type = COMMAND_INT,
       .data.int_command = {.value = 10},
       .execute = execute_int},
      {.type = COMMAND_SUM, .execute = execute_sum},

      {.type = COMMAND_MULT, .execute = execute_mult},
      {.type = COMMAND_DUP, .execute = execute_dup},
      {.type = COMMAND_DROP, .execute = execute_drop},
      {.type = COMMAND_DUP, .execute = execute_dup},
      {.type = COMMAND_DIV, .execute = execute_div},

  };

  const size_t commandsAmount = sizeof(commands) / sizeof(commands[0]);
  const int expectedStackTopForCommandIndex[] = {42,   52,   10,   62, 2604,
                                                 2604, 2604, 2604, 1};

  Program program =
      (Program){.commands = commands, .commandsAmount = commandsAmount};

  bool ret = true;
  for (size_t i = 0; i < program.commandsAmount; ++i) {
    vm_execute_command(compute_context, &program.commands[i]);
    Ret top = vm_top(compute_context);
    if (expectedStackTopForCommandIndex[i] != top.value) {
      ret = false;
      goto cleanup;
    }
  }
cleanup:
  destroy_compute_context(compute_context);
  return ret;
}

bool forth_if_branch_test() {
  ComputeContext *compute_context = create_compute_context();

  /*
  1 IF 2 ELSE 3 THEN
  */
  Command commands[] = {
      {.type = COMMAND_INT,
       .data.int_command = {.value = 1},
       .execute = execute_int},
      {.type = COMMAND_IF, .execute = execute_if},
      {.type = COMMAND_INT,
       .data.int_command = {.value = 2},
       .execute = execute_int},
      {.type = COMMAND_ELSE, .execute = execute_else},
      {.type = COMMAND_INT,
       .data.int_command = {.value = 3},
       .execute = execute_int},
      {.type = COMMAND_THEN, .execute = execute_then},
  };

  const size_t commandsAmount = sizeof(commands) / sizeof(commands[0]);
  const int expectedStackTopForCommandIndex[] = {1, 1, 2, 2, 2, 2};

  Program program =
      (Program){.commands = commands, .commandsAmount = commandsAmount};

  bool ret = true;
  for (size_t i = 0; i < program.commandsAmount; ++i) {
    vm_execute_command(compute_context, &program.commands[i]);
    Ret top = vm_top(compute_context);
    if (expectedStackTopForCommandIndex[i] != top.value) {
      ret = false;
      goto cleanup;
    }
  }
cleanup:
  destroy_compute_context(compute_context);
  return ret;
}

bool forth_else_branch_test() {
  ComputeContext *compute_context = create_compute_context();

  /*
  0 IF 2 ELSE 3 THEN
  */
  Command commands[] = {
      {.type = COMMAND_INT,
       .data.int_command = {.value = 0},
       .execute = execute_int},
      {.type = COMMAND_IF, .execute = execute_if},
      {.type = COMMAND_INT,
       .data.int_command = {.value = 2},
       .execute = execute_int},
      {.type = COMMAND_ELSE, .execute = execute_else},
      {.type = COMMAND_INT,
       .data.int_command = {.value = 3},
       .execute = execute_int},
      {.type = COMMAND_THEN, .execute = execute_then},
  };

  const size_t commandsAmount = sizeof(commands) / sizeof(commands[0]);
  const int expectedStackTopForCommandIndex[] = {0, 0, 0, 0, 3, 3};

  Program program =
      (Program){.commands = commands, .commandsAmount = commandsAmount};

  bool ret = true;
  for (size_t i = 0; i < program.commandsAmount; ++i) {
    vm_execute_command(compute_context, &program.commands[i]);
    Ret top = vm_top(compute_context);
    if (expectedStackTopForCommandIndex[i] != top.value) {
      ret = false;
      goto cleanup;
    }
  }
cleanup:
  destroy_compute_context(compute_context);
  return ret;
}
// TODO: необходимо переработать процесс прогона тестов, добавив служебные
// команды-точки останова для сравнения вершины стека с ожидаемым результатом
// (команды {, ->, }) bool forth_do_loop_simple_test() {
bool forth_do_loop_simple_test() {
  ComputeContext *compute_context = create_compute_context();

  /*
  0 0 10 DO 1 + LOOP
  */
  Command commands[] = {
      {.type = COMMAND_INT,
       .data.int_command = {.value = 0},
       .execute = execute_int},
      {.type = COMMAND_INT,
       .data.int_command = {.value = 0},
       .execute = execute_int},
      {.type = COMMAND_INT,
       .data.int_command = {.value = 10},
       .execute = execute_int},
      {.type = COMMAND_DO, .execute = execute_do},
      {.type = COMMAND_INT,
       .data.int_command = {.value = 1},
       .execute = execute_int},
      {.type = COMMAND_SUM, .execute = execute_sum},
      {.type = COMMAND_LOOP, .execute = execute_loop},
  };

  const size_t commandsAmount = sizeof(commands) / sizeof(commands[0]);

  Program program =
      (Program){.commands = commands, .commandsAmount = commandsAmount};

  bool ret = true;
  vm_execute_program(compute_context, &program);
  Ret top = vm_top(compute_context);
  if (top.error != 0 || 10 != top.value) {
    ret = false;
    goto cleanup;
  }
cleanup:
  destroy_compute_context(compute_context);
  return ret;
}

bool forth_compile_custom_word() {
  ComputeContext *compute_context = create_compute_context();

  /*
  : square DUP * ;
  */
  Command commands[] = {
      {.type = COMMAND_COMPILE,
       .command_name = ":\0",
       .execute = execute_compile},
      {.type = UNKNOWN, .command_name = "square\0"},
      {.type = COMMAND_DUP, .command_name = "DUP\0", .execute = execute_dup},
      {.type = COMMAND_MULT, .command_name = "*\0", .execute = execute_mult},
      {.type = COMMAND_END_COMPILE,
       .command_name = ";\0",
       .execute = execute_end_compile},
  };

  const size_t commandsAmount = sizeof(commands) / sizeof(commands[0]);

  Program program =
      (Program){.commands = commands, .commandsAmount = commandsAmount};

  bool ret = true;
  vm_execute_program(compute_context, &program);

  Vector *commandsVector = compute_context->commands_vector;
  for (size_t i = 0; i < vector_size(commandsVector); ++i) {
    printf("access vector[%ld]\n", i);
    Node node;
    bool ok = vector_get(commandsVector, i, &node);
    if (!ok) {
      printf("Fail to access vector[%ld]\n", i);
    }
    Command *com = (Command *)node.data;
    printf("%s ", com->command_name);
  }
cleanup:
  destroy_compute_context(compute_context);
  return ret;
}

static Test tests[] = {
    {.name = "forth_simple_calculation_test",
     .test = forth_simple_calculation_test},
    {.name = "forth_if_branch_test", .test = forth_if_branch_test},
    {.name = "forth_else_branch_test", .test = forth_else_branch_test},
    {.name = "forth_do_loop_simple_test", .test = forth_do_loop_simple_test},
    {.name = "forth_compile_custom_word", .test = forth_compile_custom_word}};

bool forth_commands_run_all_tests(void) {
  for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i) {
    if (!tests[i].test()) {
      printf("FAIL: %s\n", tests[i].name);
      return false;
    } else {
      printf("PASS: %s\n", tests[i].name);
    }
  }
  return true;
}
