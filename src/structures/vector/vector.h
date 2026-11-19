#pragma once
#include <stdbool.h>
#include <stdlib.h>

typedef struct Node {
  void *data;
  size_t size;
} Node;

typedef struct Vector {
  Node *nodes;
  size_t size;
  size_t capacity;
} Vector;

Vector *create_vector();
void destroy_vector(Vector *vec);

bool vector_append(Vector *vec, const void *data, size_t size);
bool vector_remove(Vector *vec, size_t index);

bool vector_get(const Vector *vec, size_t index, void *data, size_t size);
size_t vector_size(const Vector *vec);
