#include "stack.h"

Ret push(Stack *stack, int value) {
  if (stack->topIndex >= maxStackSize) {
    return (Ret){.value = 0, .error = -1};
  }

  stack->topIndex++;
  stack->data[stack->topIndex] = value;

  return (Ret){.value = value, .error = 0};
}

Ret top(Stack *stack) {
  if (stack->topIndex < 0) {
    return (Ret){.value = 0, .error = -1};
  }
  return (Ret){.value = stack->data[stack->topIndex], .error = 0};
}

Ret pop(Stack *stack) {
  if (stack->topIndex < 0) {
    return (Ret){.value = 0, .error = -1};
  }
  return (Ret){.value = stack->data[stack->topIndex--], .error = 0};
}
