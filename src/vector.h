#pragma once
#include <stdbool.h>
#include <stdlib.h>

typedef struct Vector Vector;

Vector *create_vector();
void destroy_vector(Vector *vec);

typedef struct Node {
  void *data;
  size_t size;
} Node;

bool vector_append(Vector *vec, Node node);
bool vector_remove(Vector *vec, size_t index);
bool vector_get(const Vector *vec, size_t index, Node *out);
