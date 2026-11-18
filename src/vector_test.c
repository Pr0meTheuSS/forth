#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "test.h"
#include "vector.h"

bool assertIdleVector() {
  bool ret = true;

  Vector *vec = create_vector();
  ret = (vec != NULL);
  destroy_vector(vec);

  return ret;
}

bool assertEmptyVectorHasZeroSize() {
  bool ret = true;

  Vector *vec = create_vector();
  ret = (vec != NULL);

  ret = (vector_size(vec) == 0);
  destroy_vector(vec);
  return ret;
}

bool assertPushVector() {
  Vector *vec = create_vector();
  int pushingValue = 1;

  Node node;
  node.size = sizeof(pushingValue);
  node.data = malloc(node.size);
  memcpy(node.data, &pushingValue, node.size);

  bool ret = vector_append(vec, node);

  destroy_vector(vec);
  return ret;
}

bool assertAppendAndGetNodeFromVector() {
  bool ret = true;
  Vector *vec = create_vector();

  Node node;
  size_t i = 0;
  for (; i < 2048; i++) {
    node.size = sizeof(i);
    node.data = malloc(sizeof(i));
    memcpy(node.data, &i, node.size);

    bool ok = vector_append(vec, node);
    if (!ok) {
      ret = false;
      goto cleanup;
    }
  }

  i = 0;
  for (; i < 2048; i++) {
    bool ok = vector_get(vec, i, &node);
    if (!ok || *(size_t *)node.data != i) {
      ret = false;
      goto cleanup;
    }
  }

cleanup:
  destroy_vector(vec);
  return ret;
}

static Test tests[] = {{.name = "assertIdleVector", .test = assertIdleVector},
                       {.name = "assertPushVector", .test = assertPushVector},
                       {.name = "assertAppendAndGetNodeFromVector",
                        .test = assertAppendAndGetNodeFromVector},
                       {.name = "assertEmptyVectorHasZeroSize",
                        .test = assertEmptyVectorHasZeroSize}};

bool vector_run_all_tests(void) {
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
