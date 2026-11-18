CC = gcc

# Пути поиска заголовков: src + все подкаталоги до 2 уровней вложенности.
# После переезда файлов в src/structures/... инклуды #include "test.h"
# продолжают работать без правки исходников.
SRC_DIRS := src $(sort $(dir $(wildcard src/*/ src/*/*/)))
INCLUDES := $(addprefix -I,$(SRC_DIRS))

CFLAGS = -Wall -Wextra -Werror -g -pedantic $(INCLUDES) -fsanitize=address,undefined
LDFLAGS = -fsanitize=address,undefined
DEPFLAGS = -MMD -MP

TARGET = forth

# Все реализации: рекурсивный поиск, исключаем тесты и test_main.c.
# Порядок не важен, пути могут быть любой глубины.
SRCS := $(shell find src -name '*.c' ! -name '*_test.c' ! -name 'test_main.c')
OBJS := $(SRCS:.c=.o)

# Тесты теперь в двух местах: корень src и src/structures/*/
TEST_SRCS := $(wildcard src/*_test.c src/structures/*/*_test.c)
TEST_OBJS := $(TEST_SRCS:.c=.o)
TEST_BIN = tests

# Имя бинаря отдельного теста = имя файла без _test.c (из любого каталога):
#   src/commands_test.c                      -> commands
#   src/structures/avl_tree/avl-tree_test.c  -> avl-tree
TEST_NAMES := $(basename $(notdir $(TEST_SRCS)))

# Реализации без main.c — для линковки тестов
SRCS_FOR_TESTS := $(filter-out src/main.c,$(SRCS))
OBJS_FOR_TESTS := $(SRCS_FOR_TESTS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# Автозависимости от заголовков (в т.ч. из подкаталогов)
-include $(SRCS:.c=.d) $(TEST_SRCS:.c=.d)

# --- Релизная сборка: оптимизация и уменьшение размера ---
REL_CFLAGS  = -Wall -Wextra -Werror -pedantic $(INCLUDES) -O2 -flto \
	          -ffunction-sections -fdata-sections \
	          -fno-plt -fomit-frame-pointer \
	          -D_FORTIFY_SOURCE=2 -DNDEBUG
REL_LDFLAGS = -flto -s -Wl,--gc-sections -Wl,-O1 -Wl,--as-needed

REL_SMALL_CFLAGS  = $(subst -O2,-Oz,$(REL_CFLAGS)) -fno-unroll-loops
REL_SMALL_LDFLAGS = $(REL_LDFLAGS)

REL_OBJS := $(SRCS:.c=.rel.o)

%.rel.o: %.c
	$(CC) $(REL_CFLAGS) -c $< -o $@

release: clean-objs $(TARGET)
release: CFLAGS = $(REL_CFLAGS)
release: LDFLAGS = $(REL_LDFLAGS)

forth_small: $(REL_OBJS)
	$(CC) $(REL_SMALL_CFLAGS) -o $@ $(REL_OBJS) $(REL_SMALL_LDFLAGS)

strip: $(TARGET)
	strip --strip-all --remove-section=.comment --remove-section=.note $(TARGET)

size: $(TARGET)
	@echo "=== Размер секций ==="
	@size -A $(TARGET)
	@echo "=== Итог ==="
	@ls -lh $(TARGET) | awk '{print "файл: " $$5}'

check-release: release
	./$(TARGET)

# --- Тесты одним бинарем ---
tests: $(TEST_OBJS) src/test_main.c $(OBJS_FOR_TESTS)
	$(CC) $(CFLAGS) $(TEST_OBJS) src/test_main.c $(OBJS_FOR_TESTS) -o $@ $(LDFLAGS)

check: tests
	./tests

# --- Отдельные тесты: по одному бинарю на каждый *_test.c ---
# Путь файла неизвестен заранее, поэтому генерируем правила шаблоном.
define TEST_RULE
 $(1): $(2) src/test_main.c $(OBJS_FOR_TESTS)
	$$(CC) $$(CFLAGS) $(2) src/test_main.c $$(OBJS_FOR_TESTS) -o $$@ $$(LDFLAGS)
endef

 $(foreach src, $(TEST_SRCS),\
	$(eval $(call TEST_RULE,$(basename $(notdir $(src))),$(src:.c=.o))))

test-%: %
	./$*

# --- Покрытие ---
COV_CFLAGS = -Wall -Wextra -g -O0 $(INCLUDES) -fprofile-arcs -ftest-coverage
COV_LDFLAGS = -lgcov
COV_BIN = tests_cov

cov-objs := $(TEST_OBJS:.o=.cov.o) $(OBJS_FOR_TESTS:.o=.cov.o)

%.cov.o: %.c
	$(CC) $(COV_CFLAGS) -c $< -o $@

$(COV_BIN): $(cov-objs) src/test_main.c
	$(CC) $(COV_CFLAGS) $(cov-objs) src/test_main.c $(COV_LDFLAGS) -o $@

coverage: $(COV_BIN)
	./$(COV_BIN)
	lcov --capture --directory src --output-file coverage.info
	genhtml coverage.info --output-directory coverage_report

# --- Документация ---
DOXYGEN = doxygen
DOXYFILE = Doxyfile

docs:
	$(DOXYGEN) $(DOXYFILE)

# --- Справка ---
help:
	@echo "Доступные цели:"
	@echo "  make            — собрать forth (debug, с санитайзерами)"
	@echo "  make tests      — собрать все тесты одним бинарем"
	@echo "  make check      — собрать и запустить тесты"
	@echo "  make test-<имя> — отдельный тест; имена: $(TEST_NAMES)"
	@echo "                    (например: make test-avl-tree, make test-stack)"
	@echo "  make release    — релизная сборка с оптимизацией"
	@echo "  make forth_small— сборка с минимальным размером бинаря"
	@echo "  make strip      — вырезать символы из собранного бинаря"
	@echo "  make size       — показать размер секций бинаря"
	@echo "  make coverage   — тесты + отчёт lcov (coverage_report/)"
	@echo "  make docs       — документация doxygen (docs/html/)"
	@echo "  make cdb        — перегенерировать compile_commands.json (bear)"
	@echo "  make clean      — удалить всё сгенерированное"

.PHONY: all clean check check-release release forth_small strip size \
	$(TEST_NAMES) coverage docs cdb help

clean-objs:
	rm -f $(OBJS)

clean:
	rm -f $(TARGET) $(TEST_BIN) $(COV_BIN) forth_small $(TEST_NAMES)
	rm -rf coverage_report coverage.info docs/html
	find src \( -name '*.o' -o -name '*.d' \
	            -o -name '*.gcda' -o -name '*.gcno' \) -delete
