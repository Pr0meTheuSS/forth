#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "compute_context.h"
#include "structures/stack/stack.h"
#include "structures/vector/vector.h"
#include "test.h"

Program *program_from_commands(const Command *commands, size_t commandsAmount) {
  Program *program = malloc(sizeof(Program));
  program->commands = create_vector();

  for (size_t i = 0; i < commandsAmount; ++i) {
    vector_append(program->commands, &commands[i], sizeof(commands[i]));
  }
  return program;
}

static void destroy_program(Program *program) {
  destroy_vector(program->commands);
  free(program);
}

/* Прогон по одной команде со сравнением вершины стека после каждой. */
static bool run_program_stepwise(ComputeContext *compute_context,
                                 Program *program,
                                 const int expectedStackTopForCommandIndex[]) {
  for (size_t i = 0; i < vector_size(program->commands); ++i) {
    Command command;
    if (!vector_get(program->commands, i, &command, sizeof(command))) {
      return false;
    }
    vm_execute_command(compute_context, &command);
    Ret top = vm_top(compute_context);
    if (top.error != 0 || expectedStackTopForCommandIndex[i] != top.value) {
      printf("Expected top [%d], has [%d]\n",
             expectedStackTopForCommandIndex[i], top.value);
      return false;
    }
  }
  return true;
}

bool forth_stack_push_order_test() {
  ComputeContext *compute_context = create_compute_context();

  /*
  42 52 10
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
  };

  const size_t commandsAmount = sizeof(commands) / sizeof(commands[0]);
  const int expectedStackTopForCommandIndex[] = {42, 52, 10};

  Program *program = program_from_commands(commands, commandsAmount);

  bool ret = run_program_stepwise(compute_context, program,
                                  expectedStackTopForCommandIndex);
  destroy_program(program);
  destroy_compute_context(compute_context);
  return ret;
}

bool forth_sum_test() {
  ComputeContext *compute_context = create_compute_context();

  /*
  42 52 10 SUM
  */
  Command commands[] = {
      {.type = COMMAND_INT,
       .data.int_command = {.value = 42},
       .execute = execute_int},
      {.type = COMMAND_INT,
       .data.int_command = {.value = 10},
       .execute = execute_int},
      {.type = COMMAND_SUM, .execute = execute_sum},
  };

  const size_t commandsAmount = sizeof(commands) / sizeof(commands[0]);
  const int expectedStackTopForCommandIndex[] = {42, 10, 52};

  Program *program = program_from_commands(commands, commandsAmount);

  bool ret = run_program_stepwise(compute_context, program,
                                  expectedStackTopForCommandIndex);
  destroy_program(program);
  destroy_compute_context(compute_context);
  return ret;
}

bool forth_mult_test() {
  ComputeContext *compute_context = create_compute_context();

  /*
  62 52 MULT
  */
  Command commands[] = {
      {.type = COMMAND_INT,
       .data.int_command = {.value = 62},
       .execute = execute_int},
      {.type = COMMAND_INT,
       .data.int_command = {.value = 52},
       .execute = execute_int},
      {.type = COMMAND_MULT, .execute = execute_mult},
  };

  const size_t commandsAmount = sizeof(commands) / sizeof(commands[0]);
  const int expectedStackTopForCommandIndex[] = {62, 52, 3224};

  Program *program = program_from_commands(commands, commandsAmount);

  bool ret = run_program_stepwise(compute_context, program,
                                  expectedStackTopForCommandIndex);
  destroy_program(program);
  destroy_compute_context(compute_context);
  return ret;
}

bool forth_dup_test() {
  ComputeContext *compute_context = create_compute_context();

  /*
  3224 DUP
  */
  Command commands[] = {
      {.type = COMMAND_INT,
       .data.int_command = {.value = 3224},
       .execute = execute_int},
      {.type = COMMAND_DUP, .execute = execute_dup},
  };

  const size_t commandsAmount = sizeof(commands) / sizeof(commands[0]);
  const int expectedStackTopForCommandIndex[] = {3224, 3224};

  Program *program = program_from_commands(commands, commandsAmount);

  bool ret = run_program_stepwise(compute_context, program,
                                  expectedStackTopForCommandIndex);
  destroy_program(program);
  destroy_compute_context(compute_context);
  return ret;
}

bool forth_drop_test() {
  ComputeContext *compute_context = create_compute_context();

  /*
  3224 DUP DROP
  */
  Command commands[] = {
      {.type = COMMAND_INT,
       .data.int_command = {.value = 3224},
       .execute = execute_int},
      {.type = COMMAND_DUP, .execute = execute_dup},
      {.type = COMMAND_DROP, .execute = execute_drop},
  };

  const size_t commandsAmount = sizeof(commands) / sizeof(commands[0]);
  const int expectedStackTopForCommandIndex[] = {3224, 3224, 3224};

  Program *program = program_from_commands(commands, commandsAmount);

  bool ret = run_program_stepwise(compute_context, program,
                                  expectedStackTopForCommandIndex);
  destroy_program(program);
  destroy_compute_context(compute_context);
  return ret;
}

bool forth_div_test() {
  ComputeContext *compute_context = create_compute_context();

  /*
  3224 3224 DIV
  */
  Command commands[] = {
      {.type = COMMAND_INT,
       .data.int_command = {.value = 3224},
       .execute = execute_int},
      {.type = COMMAND_INT,
       .data.int_command = {.value = 3224},
       .execute = execute_int},
      {.type = COMMAND_DIV, .execute = execute_div},
  };

  const size_t commandsAmount = sizeof(commands) / sizeof(commands[0]);
  const int expectedStackTopForCommandIndex[] = {3224, 3224, 1};

  Program *program = program_from_commands(commands, commandsAmount);

  bool ret = run_program_stepwise(compute_context, program,
                                  expectedStackTopForCommandIndex);
  destroy_program(program);
  destroy_compute_context(compute_context);
  return ret;
}

bool forth_if_true_branch_test() {
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

  Program *program = program_from_commands(commands, commandsAmount);

  bool ret = run_program_stepwise(compute_context, program,
                                  expectedStackTopForCommandIndex);
  destroy_program(program);
  destroy_compute_context(compute_context);
  return ret;
}

bool forth_if_false_branch_test() {
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

  Program *program = program_from_commands(commands, commandsAmount);

  bool ret = run_program_stepwise(compute_context, program,
                                  expectedStackTopForCommandIndex);
  destroy_program(program);
  destroy_compute_context(compute_context);
  return ret;
}

/* ---------- Циклы ---------- */

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

  Program *program = program_from_commands(commands, commandsAmount);

  bool ret = true;
  vm_execute_program(compute_context, program);
  Ret top = vm_top(compute_context);
  if (top.error != 0 || 10 != top.value) {
    ret = false;
    goto cleanup;
  }
cleanup:
  destroy_program(program);
  destroy_compute_context(compute_context);
  return ret;
}

/* ---------- Компиляция (разбита на компиляцию и проверку тела) ---------- */

bool forth_compile_custom_word() {
  ComputeContext *compute_context = create_compute_context();

  /*
  : square DUP * ;
  */
  Command commands[] = {
      {.type = COMMAND_COMPILE,
       .command_name = ":\0",
       .execute = execute_compile},
      {.type = COMMAND_REGISTER_CUSTOM_WORD,
       .command_name = "square\0",
       .execute = execute_register_custom_word},
      {.type = COMMAND_DUP, .command_name = "DUP\0", .execute = execute_dup},
      {.type = COMMAND_MULT, .command_name = "*\0", .execute = execute_mult},
      {.type = COMMAND_END_COMPILE,
       .command_name = ";\0",
       .execute = execute_end_compile},
  };

  const size_t commandsAmount = sizeof(commands) / sizeof(commands[0]);

  Program *program = program_from_commands(commands, commandsAmount);

  bool ret = true;
  vm_execute_program(compute_context, program);

  destroy_program(program);
  destroy_compute_context(compute_context);
  return ret;
}

bool forth_compile_custom_word_body_test() {
  ComputeContext *compute_context = create_compute_context();

  /*
  : square DUP * ;
  Тело скомпилированного слова должно содержать DUP и *
  */
  Command commands[] = {
      {.type = COMMAND_COMPILE,
       .command_name = ":\0",
       .execute = execute_compile},
      {.type = COMMAND_REGISTER_CUSTOM_WORD,
       .command_name = "square\0",
       .execute = execute_register_custom_word},
      {.type = COMMAND_DUP, .command_name = "DUP\0", .execute = execute_dup},
      {.type = COMMAND_MULT, .command_name = "*\0", .execute = execute_mult},
      {.type = COMMAND_END_COMPILE,
       .command_name = ";\0",
       .execute = execute_end_compile},
  };

  const size_t commandsAmount = sizeof(commands) / sizeof(commands[0]);
  const char *expectedBodyNames[] = {"DUP\0", "*\0"};
  const size_t expectedBodyAmount =
      sizeof(expectedBodyNames) / sizeof(expectedBodyNames[0]);

  Program *program = program_from_commands(commands, commandsAmount);

  bool ret = true;
  vm_execute_program(compute_context, program);

  Vector *commandsVector = compute_context->commands_vector;
  size_t found = 0;
  for (size_t i = 0; i < vector_size(commandsVector); ++i) {
    Command command;
    if (!vector_get(commandsVector, i, &command, sizeof(command))) {
      printf("Fail to access vector[%ld]\n", i);
      ret = false;
      goto cleanup;
    }
    for (size_t j = 0; j < expectedBodyAmount; ++j) {
      if (strcmp(command.command_name, expectedBodyNames[j]) == 0) {
        ++found;
      }
    }
  }
  if (found != expectedBodyAmount) {
    ret = false;
  }
cleanup:
  destroy_program(program);
  destroy_compute_context(compute_context);
  return ret;
}

bool forth_compile_custom_word_execution() {
  ComputeContext *compute_context = create_compute_context();

  /*
  : square DUP * ;
  5 square
  */
  Command commands[] = {
      {.type = COMMAND_COMPILE,
       .command_name = ":\0",
       .execute = execute_compile},
      {.type = COMMAND_REGISTER_CUSTOM_WORD,
       .command_name = "square\0",
       .execute = execute_register_custom_word},
      {.type = COMMAND_DUP, .command_name = "DUP\0", .execute = execute_dup},
      {.type = COMMAND_MULT, .command_name = "*\0", .execute = execute_mult},
      {.type = COMMAND_END_COMPILE,
       .command_name = ";\0",
       .execute = execute_end_compile},
      {.type = COMMAND_INT,
       .data.int_command = {.value = 5},
       .execute = execute_int},
      {.type = COMMAND_CUSTOM,
       .command_name = "square\0",
       .execute = execute_custom_command},
  };

  const size_t commandsAmount = sizeof(commands) / sizeof(commands[0]);

  Program *program = program_from_commands(commands, commandsAmount);

  bool ret = true;
  vm_execute_program(compute_context, program);

  Ret top = vm_top(compute_context);
  if (top.error != 0 || top.value != 5 * 5) {
    printf("top.error: [%d], top.value: [%d]\n", top.error, top.value);
    ret = false;
    goto cleanup;
  }

cleanup:
  destroy_program(program);
  destroy_compute_context(compute_context);
  return ret;
}

bool forth_compile_custom_word_execution_twice() {
  ComputeContext *compute_context = create_compute_context();

  /*
  : square DUP * ;
  2 square square
  */
  Command commands[] = {
      {.type = COMMAND_COMPILE,
       .command_name = ":\0",
       .execute = execute_compile},
      {.type = COMMAND_REGISTER_CUSTOM_WORD,
       .command_name = "square\0",
       .execute = execute_register_custom_word},
      {.type = COMMAND_DUP, .command_name = "DUP\0", .execute = execute_dup},
      {.type = COMMAND_MULT, .command_name = "*\0", .execute = execute_mult},
      {.type = COMMAND_END_COMPILE,
       .command_name = ";\0",
       .execute = execute_end_compile},
      {.type = COMMAND_INT,
       .data.int_command = {.value = 2},
       .execute = execute_int},
      {.type = COMMAND_CUSTOM,
       .command_name = "square\0",
       .execute = execute_custom_command},
      {.type = COMMAND_CUSTOM,
       .command_name = "square\0",
       .execute = execute_custom_command},
  };

  const size_t commandsAmount = sizeof(commands) / sizeof(commands[0]);

  Program *program = program_from_commands(commands, commandsAmount);

  bool ret = true;
  vm_execute_program(compute_context, program);

  Ret top = vm_top(compute_context);
  if (top.error != 0 || top.value != 16) {
    printf("top.error: [%d], top.value: [%d]\n", top.error, top.value);
    ret = false;
    goto cleanup;
  }

cleanup:
  destroy_program(program);
  destroy_compute_context(compute_context);
  return ret;
}

/* ---------- Реестр тестов ---------- */

static Test tests[] = {
    {.name = "forth_stack_push_order_test",
     .test = forth_stack_push_order_test},
    {.name = "forth_sum_test", .test = forth_sum_test},
    {.name = "forth_mult_test", .test = forth_mult_test},
    {.name = "forth_dup_test", .test = forth_dup_test},
    {.name = "forth_drop_test", .test = forth_drop_test},
    {.name = "forth_div_test", .test = forth_div_test},
    {.name = "forth_if_true_branch_test", .test = forth_if_true_branch_test},
    {.name = "forth_if_false_branch_test", .test = forth_if_false_branch_test},
    {.name = "forth_do_loop_simple_test", .test = forth_do_loop_simple_test},
    {.name = "forth_compile_custom_word", .test = forth_compile_custom_word},
    {.name = "forth_compile_custom_word_body_test",
     .test = forth_compile_custom_word_body_test},
    {.name = "forth_compile_custom_word_execution",
     .test = forth_compile_custom_word_execution},
    {.name = "forth_compile_custom_word_execution_twice",
     .test = forth_compile_custom_word_execution_twice}};

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
