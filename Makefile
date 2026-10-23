CC = gcc
CFLAGS = -Wall -Wextra -g -pedantic -fsanitize=address,undefined
LDFLAGS = -fsanitize=address,undefined

TARGET = forth

# Исходники проекта (реализации, НЕ тесты)
SRCS = src/main.c src/command.c src/compute_context.c src/stack.c src/avl-tree.c
OBJS = $(SRCS:.c=.o)

# Все файлы *_test.c в src/ (не рекурсивно, только в этой папке)
TEST_SRCS := $(wildcard src/*_test.c)
TEST_OBJS := $(TEST_SRCS:.c=.o)
TEST_BIN = tests

# Имена бинарей для отдельных тестов: foo_test.c -> foo
TEST_NAMES := $(TEST_SRCS:src/%.c=%)
TEST_NAMES := $(TEST_NAMES:_test.c=)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- Сборка всех тестов в один бинарь ---
# Линкуем: все тестовые .o, test_main.c, и все реализации (без main.c, чтобы не было двух main)
# Исключаем main.c из реализаций специально для тестов
SRCS_FOR_TESTS := $(filter-out src/main.c,$(SRCS))
OBJS_FOR_TESTS := $(SRCS_FOR_TESTS:.c=.o)

tests: $(TEST_OBJS) src/test_main.c $(OBJS_FOR_TESTS)
	$(CC) $(CFLAGS) $(TEST_OBJS) src/test_main.c $(OBJS_FOR_TESTS) -o $@ $(LDFLAGS)

check: tests
	./tests

# --- Отдельные тесты (по одному) ---
# Каждый отдельный тест: один *_test.o + test_main.c + все реализации (без main.c)
$(TEST_NAMES): %: src/%_test.c src/test_main.c $(OBJS_FOR_TESTS)
	$(CC) $(CFLAGS) src/$*_test.o src/test_main.c $(OBJS_FOR_TESTS) -o $@ $(LDFLAGS)

test-%: %
	./$*

.PHONY: all clean check $(TEST_NAMES)

# Флаги для покрытия: без санитайзеров, зато с отладочной информацией и профайлингом
COV_CFLAGS = -Wall -Wextra -g -O0 -fprofile-arcs -ftest-coverage
COV_LDFLAGS = -lgcov

# Бинарь для покрытия (отдельный от обычного tests)
COV_BIN = tests_cov

cov-objs := $(TEST_SRCS:.c=.cov.o) $(SRCS_FOR_TESTS:.c=.cov.o)

# Правило компиляции под покрытие
%.cov.o: %.c
	$(CC) $(COV_CFLAGS) -c $< -o $@

# Сборка бинаря с покрытием
$(COV_BIN): $(cov-objs) src/test_main.c
	$(CC) $(COV_CFLAGS) $(cov-objs) src/test_main.c $(COV_LDFLAGS) -o $@

# Запуск тестов и генерация отчёта
coverage: $(COV_BIN)
	./$(COV_BIN)
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory coverage_report

.PHONY: all clean check $(TEST_NAMES) coverage

DOXYGEN = doxygen
DOXYFILE = Doxyfile

.PHONY: docs clean

docs:
	$(DOXYGEN) $(DOXYFILE)

clean:
	rm -rf $(OBJS) $(TEST_OBJS) $(TARGET) $(TEST_BIN) $(TEST_NAMES) src/*.o \
	      coverage_report coverage.info src/*.gcda src/*.gcno \
	      docs/html
