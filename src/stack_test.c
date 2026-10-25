#include <stdbool.h>
#include <stdio.h>

#include "stack.h"
#include "test.h"

bool assertIdleStack() {
  bool ret = true;

  Stack *stack = create_stack();
  destroy_stack(stack);

  return ret;
}

bool assertPushStack() {
  Stack *stack = create_stack();
  int pushingValue = 1;

  push(stack, pushingValue);

  bool ret = stack->topIndex == 0 && stack->data[0] == pushingValue;
  destroy_stack(stack);
  return ret;
}

bool assertTopEmptyStack() {
  Stack *stack = create_stack();
  Ret ret = top(stack);
  destroy_stack(stack);
  return ret.error == -1;
}

bool assertPopEmptyStack() {
  Stack *stack = create_stack();
  Ret ret = pop(stack);
  destroy_stack(stack);
  return ret.error == -1;
}

bool assertTopStack() {
  Stack *stack = create_stack();
  int pushingValue = 1;

  Ret ret = push(stack, pushingValue);
  ret = top(stack);

  destroy_stack(stack);
  return ret.value == pushingValue && ret.error == 0;
}

static Test tests[] = {
    {.name = "assertIdleStack", .test = assertIdleStack},
    {.name = "assertPushStack", .test = assertPushStack},
    {.name = "assertTopEmptyStack", .test = assertTopEmptyStack},
    {.name = "assertPopEmptyStack", .test = assertPopEmptyStack},
    {.name = "assertTopStack", .test = assertTopStack}};

bool stack_run_all_tests(void) {
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
