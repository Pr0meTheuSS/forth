#include "vector.h"
#include <assert.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t cDefaultVectorCapacity = 1024;

Vector *create_vector() {
  Vector *ret = (Vector *)malloc(sizeof(Vector));
  if (!ret) {
    return NULL;
  }
  ret->capacity = cDefaultVectorCapacity;
  ret->size = 0;

  ret->nodes = (Node *)calloc(ret->capacity, sizeof(ret->nodes[0]));
  if (!ret->nodes) {
    free(ret);
    return NULL;
  }

  return ret;
}

void destroy_vector(Vector *vec) {
  for (size_t i = 0; i < vec->size; ++i) {
    free(vec->nodes[i].data);
  }
  free(vec->nodes);
  free(vec);
}

bool vector_append(Vector *vec, const void *data, size_t size) {
  assert(vec);
  Node node;

  node.size = size;
  node.data = malloc(node.size);
  memcpy(node.data, data, node.size);

  if (vec->size == vec->capacity) {
    vec->capacity *= 2;
    vec->nodes = reallocarray(vec->nodes, vec->capacity, sizeof(vec->nodes[0]));
    if (!vec->nodes) {
      return false;
    }
  }
  if (!memcpy(&vec->nodes[vec->size], &node, sizeof(vec->nodes[0]))) {
    return false;
  }
  vec->size++;
  return true;
}

bool vector_remove(Vector *vec, size_t index) {
  assert(vec);
  if (index >= vec->size) {
    return false;
  }
  vec->size--;
  // TODO: not implemented yet
  return false;
}

bool vector_get(const Vector *vec, size_t index, void *out, size_t size) {
  assert(vec);
  assert(out);

  if (index >= vec->size) {
    return false;
  }
  Node node = vec->nodes[index];
  return memcpy(out, node.data, size);
}

size_t vector_size(const Vector *vec) {
  assert(vec);
  return vec->size;
}
