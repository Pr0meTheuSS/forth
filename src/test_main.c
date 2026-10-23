#include <stdbool.h>
#include <stdio.h>

// Сюда нужно добавить объявления всех test_* функций из *_test.c
// Либо подключить общий заголовок, например test_runner.h
extern bool avl_tree_run_all_tests(void);
extern bool stack_run_all_tests(void);

int main(void) {
  bool ok = true;

  ok = ok && avl_tree_run_all_tests();
  ok = ok && stack_run_all_tests();
  if (ok) {
    printf("All tests passed.\n");
    return 0;
  } else {
    printf("Some tests failed.\n");
    return 1;
  }
}
