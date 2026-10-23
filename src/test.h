#pragma once

typedef struct Test {
  const char *name;
  bool (*test)();
} Test;
