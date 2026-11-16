#include "vector.h"
#include <assert.h>

#include <stdlib.h>
#include <string.h>
const size_t cDefaultVectorCapacity = 1;

typedef struct Vector {
  Node *nodes;
  size_t size;
  size_t capacity;
} Vector;

Vector *create_vector() {
  Vector *ret = (Vector *)malloc(sizeof(Vector));
  if (!ret) {
    return NULL;
  }
  ret->capacity = cDefaultVectorCapacity;
  ret->size = 0;

  ret->nodes = (Node *)calloc(ret->capacity, sizeof(*ret->nodes));
  if (!ret->nodes) {
    free(ret);
    return NULL;
  }

  return ret;
}

void destroy_vector(Vector *vec) {
  free(vec->nodes);
  free(vec);
}

bool vector_append(Vector *vec, Node node) {
  assert(vec);

  if (vec->size == vec->capacity) {
    vec->nodes =
        reallocarray(vec->nodes, vec->capacity * 2, sizeof(vec->nodes[0]));
    if (!vec->nodes) {
      return false;
    }
    vec->capacity *= 2;
  }
  if (!memcpy(&node, &vec->nodes[vec->size], sizeof(vec->nodes[0]))) {
    return false;
  }
  vec->size++;
  return true;
}

bool vector_remove(Vector *vec, size_t index) {
  assert(vec);
  assert(index <= vec->size);
  // TODO: not implemented yet
  return false;
}

bool vector_get(const Vector *vec, size_t index, Node *out) {
  assert(vec);
  assert(out);

  if (index >= vec->size) {
    return false;
  }
  return memcpy(&vec->nodes[index], out, sizeof(*out));
}
