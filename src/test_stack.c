#include <stdio.h>

// Define the test result strings
#define PASS "PASS"
#define FAIL "FAIL"

#include "stack.h"

static int count = 0;

int assertIdleStack() {
  Stack stack = {.topIndex = -1};

  int condition = stack.topIndex == -1;
  printf("#%d: %s\n", count, condition ? PASS : FAIL);
  if (!condition) {
    printf("#%d: had true, wanted false\n", count);
    count++;
    return 1;
  }

  count++;
  return 0;
}

int assertPushStack() {
  Stack stack = {.topIndex = -1};
  int pushingValue = 1;

  push(&stack, pushingValue);

  int condition = stack.topIndex == 0 && stack.data[0] == pushingValue;

  printf("#%d: %s\n", count, condition ? PASS : FAIL);
  if (!condition) {
    printf("#%d: had true, wanted false\n", count);
    count++;
    return 1;
  }

  count++;
  return 0;
}

int assertTopEmptyStack() {
  Stack stack = {.topIndex = -1};

  Ret ret = top(&stack);

  int condition = ret.error == -1;

  printf("#%d: %s\n", count, condition ? PASS : FAIL);
  if (!condition) {
    printf("#%d: had true, wanted false\n", count);
    count++;
    return 1;
  }

  count++;
  return 0;
}

int assertPopEmptyStack() {
  Stack stack = {.topIndex = -1};

  Ret ret = pop(&stack);

  int condition = ret.error == -1;

  printf("#%d: %s\n", count, condition ? PASS : FAIL);
  if (!condition) {
    printf("#%d: had true, wanted false\n", count);
    count++;
    return 1;
  }

  count++;
  return 0;
}

int assertTopStack() {
  Stack stack = {.topIndex = -1};
  int pushingValue = 1;

  Ret ret = push(&stack, pushingValue);
  ret = top(&stack);

  int condition = ret.value == pushingValue && ret.error == 0;

  printf("#%d: %s\n", count, condition ? PASS : FAIL);
  if (!condition) {
    printf("#%d: had true, wanted false\n", count);
    count++;
    return 1;
  }

  count++;
  return 0;
}

int main(void) {
  assertIdleStack();
  assertPushStack();
  assertTopEmptyStack();
  assertPopEmptyStack();
  assertTopStack();
}
