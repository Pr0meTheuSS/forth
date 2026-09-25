#include <stdlib.h>

#include "command.h"

void test_forth_program() {
  VM vm;
  vm.stack.topIndex = -1;

  Command commands[] = {{.type = COMMAND_INT,
                         .data.int_command = {.value = 42},
                         .execute = execute_int},
                        {.type = COMMAND_INT,
                         .data.int_command = {.value = 52},
                         .execute = execute_int},
                        {.type = COMMAND_INT,
                         .data.int_command = {.value = 10},
                         .execute = execute_int},
                        {.type = COMMAND_SUM, .execute = execute_sum},

                        {.type = COMMAND_MULT, .execute = execute_mult}};
  for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); ++i) {
    commands[i].execute(&vm, &commands[i].data);
    printf("stack size: %d\n", vm.stack.topIndex + 1);
    for (int stackTopIndex = vm.stack.topIndex; stackTopIndex >= 0;
         --stackTopIndex) {
      if (stackTopIndex >= 0) {
        printf("\t[%d]: %d\n", stackTopIndex + 1, vm.stack.data[stackTopIndex]);
      }
    }
  }
}
