#include <stdlib.h>

#include "command.h"
#include "compute_context.h"

void test_forth_program() {
  ComputeContext vm;
  vm.stack.topIndex = -1;
  vm.skip_depth = 0;
  /*
  42 52 10
  SUM
  MULT
  DUP
  DROP
  DUP
  DIV  
  */
  Command program1[] = {
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
  /*
  1 IF 2 ELSE 3 THEN
  */
  Command program2[] = {
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

  // Execute program 1
  for (size_t i = 0; i < sizeof(program1) / sizeof(program1[0]); ++i) {
    vm_execute_command(&vm, &program1[i]);
    printf("stack size: %d\n", vm.stack.topIndex + 1);
    for (int stackTopIndex = vm.stack.topIndex; stackTopIndex >= 0;
         --stackTopIndex) {
      if (stackTopIndex >= 0) {
        printf("\t[%d]: %d\n", stackTopIndex + 1, vm.stack.data[stackTopIndex]);
      }
    }
  }
  vm.stack.topIndex = -1;
  // Execute program 2
  for (size_t i = 0; i < sizeof(program2) / sizeof(program2[0]); ++i) {
    vm_execute_command(&vm, &program2[i]);
    printf("stack size: %d\n", vm.stack.topIndex + 1);
    for (int stackTopIndex = vm.stack.topIndex; stackTopIndex >= 0;
         --stackTopIndex) {
      if (stackTopIndex >= 0) {
        printf("\t[%d]: %d\n", stackTopIndex + 1, vm.stack.data[stackTopIndex]);
      }
    }
  }
}
