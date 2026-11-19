#include "vector.h"
#include <assert.h>

#include "logger.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t cDefaultVectorCapacity = 1024;

Vector *create_vector() {
  LOG_DEBUG("vector", "create_vector");

  Vector *ret = (Vector *)malloc(sizeof(Vector));
  if (!ret) {
    LOG_ERROR("vector",
              "Vector *ret = (Vector *)malloc(sizeof(Vector)) return NULL");
    return NULL;
  }
  ret->capacity = cDefaultVectorCapacity;
  ret->size = 0;

  ret->nodes = (Node *)calloc(ret->capacity, sizeof(ret->nodes[0]));
  if (!ret->nodes) {
    LOG_ERROR(
        "vector",
        "ret->nodes = (Node *)calloc(ret->capacity, sizeof(ret->nodes[0]))");
    free(ret);
    return NULL;
  }

  return ret;
}

void destroy_vector(Vector *vec) {
  LOG_DEBUG("vector", "destroy_vector");

  for (size_t i = 0; i < vec->size; ++i) {
    LOG_DEBUG("vector", "free node [%ld]", i);
    free(vec->nodes[i].data);
  }
  free(vec->nodes);
  free(vec);
}

bool vector_append(Vector *vec, const void *data, size_t size) {
  LOG_DEBUG("vector", "vector_append");
  assert(vec);
  Node node;

  node.size = size;
  node.data = malloc(node.size);
  memcpy(node.data, data, node.size);

  if (vec->size == vec->capacity) {
    LOG_DEBUG("vector", "reallocarray");

    vec->capacity *= 2;
    vec->nodes = reallocarray(vec->nodes, vec->capacity, sizeof(vec->nodes[0]));
    if (!vec->nodes) {
      LOG_ERROR("vector", "vec->nodes = reallocarray(vec->nodes, "
                          "vec->capacity, sizeof(vec->nodes[0])) return NULL");
      return false;
    }
  }
  if (!memcpy(&vec->nodes[vec->size], &node, sizeof(vec->nodes[0]))) {
    LOG_ERROR("vector", "memcpy(&vec->nodes[vec->size], &node, "
                        "sizeof(vec->nodes[0])) return NULL");

    return false;
  }
  vec->size++;
  LOG_DEBUG("vector", "vec.size = %ld", vec->size);
  return true;
}

bool vector_remove(Vector *vec, size_t index) {
  LOG_DEBUG("vector", "vector_remove");
  assert(vec);
  if (index >= vec->size) {
    LOG_ERROR("vector", "index >= vec->size");
    return false;
  }
  vec->size--;
  // TODO: not implemented yet
  return false;
}

bool vector_get(const Vector *vec, size_t index, void *out, size_t size) {
  LOG_DEBUG("vector", "vector_get");
  assert(vec);
  assert(out);

  if (index >= vec->size) {
    LOG_ERROR("vector", "index >= vec->size");
    return false;
  }
  Node node = vec->nodes[index];
  return memcpy(out, node.data, size);
}

size_t vector_size(const Vector *vec) {
  LOG_DEBUG("vector", "vector_size");
  assert(vec);
  return vec->size;
}
