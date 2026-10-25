#pragma once
#define maxStackSize 1024

typedef struct Stack {
  int topIndex;
  int data[maxStackSize];
} Stack;

typedef struct Ret {
  int value;
  int error;
} Ret;

Stack* create_stack();
void destroy_stack(Stack*);

Ret push(Stack *stack, int value);
Ret top(Stack *stack);
Ret pop(Stack *stack);
