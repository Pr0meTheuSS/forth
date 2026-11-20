#include <stdbool.h>
#include <stdio.h>

#include "logger.h"
// Сюда нужно добавить объявления всех test_* функций из *_test.c
// Либо подключить общий заголовок, например test_runner.h
extern bool avl_tree_run_all_tests(void);
extern bool stack_run_all_tests(void);
extern bool forth_commands_run_all_tests(void);
extern bool vector_run_all_tests(void);

int main(void) {
  const char log_file[] = "log.log";
  FILE *flog = fopen(log_file, "a+");

  logger_set_stream(flog);
  logger_set_level(LOG_LVL_OFF);
  bool ok = true;

  ok = ok && avl_tree_run_all_tests();
  ok = ok && stack_run_all_tests();
  ok = ok && forth_commands_run_all_tests();
  ok = ok && vector_run_all_tests();
  if (ok) {
    printf("All tests passed.\n");
    fclose(flog);
    return 0;
  } else {
    printf("Some tests failed.\n");
    fclose(flog);
    return 1;
  }
}
