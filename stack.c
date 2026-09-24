
#define maxStackSize 1024

typedef struct Stack {
  int topIndex;
  int data[maxStackSize];
} Stack;

int push(Stack stack, int value) {
  if (stack.topIndex >= maxStackSize) {
    return -1;
  }

  stack.topIndex++;
  stack.data[stack.topIndex] = value;

  return 0;
}

typedef struct Ret {
  int value;
  int error;
} Ret;

Ret top(Stack stack) {
  if (stack.topIndex < 0) {
    return (Ret){.value = 0, .error = -1};
  }
  return (Ret){.value = stack.data[stack.topIndex], .error = 0};
}

Ret pop(Stack stack) {
  if (stack.topIndex < 0) {
    return (Ret){.value = stack.data[stack.topIndex], .error = 0};
  }
  return (Ret){.value = 0, .error = -1};
}
