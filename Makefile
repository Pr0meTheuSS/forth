CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -pedantic -fsanitize=address,undefined
LDFLAGS = -fsanitize=address,undefined

TARGET = forth

# Исходники проекта (реализации, НЕ тесты)
SRCS = src/main.c src/command.c src/compute_context.c src/stack.c src/vector.c src/avl-tree.c
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

help: # показать доступные цели
	@echo "Доступные цели:"
	@echo "  make            — собрать forth (debug, с санитайзерами)"
	@echo "  make tests      — собрать все тесты одним бинарем"
	@echo "  make check      — собрать и запустить тесты"
	@echo "  make test-<имя> — собрать и запустить отдельный тест (например: make test-forth_sum_test)"
	@echo "  make release    — релизная сборка с оптимизацией"
	@echo "  make forth_small— сборка с минимальным размером бинаря"
	@echo "  make strip      — вырезать символы из собранного бинаря"
	@echo "  make size       — показать размер секций бинаря"
	@echo "  make coverage   — тесты + отчёт lcov (coverage_report/)"
	@echo "  make docs       — документация doxygen (docs/html/)"
	@echo "  make clean      — удалить всё сгенерированное"

	.PHONY: help


# --- Релизная сборка: оптимизация и уменьшение размера ---
# -Os        — оптимизация под размер
# -flto      — link-time optimization (доп. сжатие и инлайнинг между .c)
# -ffunction-sections -fdata-sections + --gc-sections — выкинуть неиспользуемый код
# -s         — strip символьной информации на этапе линковки
# -D_FORTIFY_SOURCE — с -O2+: проверки рантайма почти без оверхеда
# -fno-asan  — санитайзеры в релиз не попадают (они утяжеляют бинарь в разы)
REL_CFLAGS  = -Wall -Wextra -Werror -pedantic -O2 -flto \
              -ffunction-sections -fdata-sections \
              -fno-plt -fomit-frame-pointer \
              -D_FORTIFY_SOURCE=2 -DNDEBUG
REL_LDFLAGS = -flto -s -Wl,--gc-sections -Wl,-O1 -Wl,--as-needed

# Тот же релизный профиль, но с максимальным сжатием (-Oz вместо -O2)
# и aggressive-инлайнингом; подходит, когда размер важнее скорости
REL_SMALL_CFLAGS  = $(subst -O2,-Oz,$(REL_CFLAGS)) -fno-unroll-loops
REL_SMALL_LDFLAGS = $(REL_LDFLAGS)

REL_OBJS = $(SRCS:.c=.rel.o)

%.rel.o: %.c
	$(CC) $(REL_CFLAGS) -c $< -o $@

# Релизный forth
release: CFLAGS = $(REL_CFLAGS)
release: clean-objs
release: $(TARGET)

# Минимальный по размеру бинарь (forth_small)
forth_small: $(REL_OBJS)
	$(CC) $(REL_SMALL_CFLAGS) -o $@ $(REL_OBJS) $(REL_SMALL_LDFLAGS)

strip: $(TARGET)
	strip --strip-all --remove-section=.comment --remove-section=.note \
        $(TARGET)

size: $(TARGET)
	@echo "=== Размер секций ==="
	@size -A $(TARGET)
	@echo "=== Итог ==="
	@ls -lh $(TARGET) | awk '{print "файл: " $$5}'

# Проверить, что release-бинарь жив: без ASan/UBSan, но без крашей
check-release: release
	./$(TARGET)

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

.PHONY: all clean check check-release release forth_small strip size \
	$(TEST_NAMES) coverage docs

clean-objs:
	rm -f $(OBJS)

clean:
	rm -rf $(OBJS) $(TEST_OBJS) $(TARGET) $(TEST_BIN) $(TEST_NAMES) \
          $(REL_OBJS) *.o src/*.o src/*.rel.o \
          coverage_report coverage.info src/*.gcda src/*.gcno \
          docs/html
docs:
	$(DOXYGEN) $(DOXYFILE)
