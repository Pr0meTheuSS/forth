# forth

## build

```sh
make clean
make
```

## build & run tests

```sh
make tests
./tests
```

## coverage

```sh
make coverage
open coverage_report/index.html
```

## docs

```sh
make docs
```

## profiling

### 0. Сборка под профилирование (важно сделать сразу)

```cmake
# CMake — профиль "profiling"
set(CMAKE_C_FLAGS_PROF "-O2 -g -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer"
    CACHE STRING "" FORCE)
# для callgrind/gprof/uftrace иногда нужна ещё -pg
```

| Флаг                      | Зачем                                                               |
| ------------------------- | ------------------------------------------------------------------- |
| `-g`                      | имена функций и **строки** в отчётах                                |
| `-fno-omit-frame-pointer` | точные стеки в perf без dwarf-развёртки                             |
| `-O2` (не `-O0`!)         | профилируем то, что будет в релизе; строки останутся благодаря `-g` |

Установка инструментов:

```bash
sudo apt install valgrind kcachegrind massif-visualizer graphviz \
                 linux-tools-generic hyperfine uftrace
# FlameGraph (скрипты Брендана Грегга)
git clone https://github.com/brendangregg/FlameGraph ~/tools/FlameGraph
```

---

## 1. Бенчмарки на Си

### Вариант А — ручной харнесс (чистый C, ничего ставить не надо)

```c
// bench.c
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

static double now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);   // монотонные часы, не wall-clock!
    return ts.tv_sec * 1e3 + ts.tv_nsec / 1e6;
}

extern void my_sort(int *a, size_t n);   // тестируемый код
extern void gen_data(int *a, size_t n);

#define ITER 200
#define WARMUP 20

int main(void) {
    static int a[100000];
    double t[ITER];

    for (int i = 0; i < WARMUP; i++) {          // прогрев кэша/branch predictors
        gen_data(a, sizeof a / sizeof *a);
        my_sort(a, sizeof a / sizeof *a);
    }
    for (int i = 0; i < ITER; i++) {
        gen_data(a, sizeof a / sizeof *a);
        double t0 = now_ms();
        my_sort(a, sizeof a / sizeof *a);
        t[i] = now_ms() - t0;
    }
    // считаем медиану и хвосты (среднее врут из-за выбросов)
    int cmp(const void *x, const void *y);
    qsort(t, ITER, sizeof t[0], cmp);
    printf("median=%.4f ms  p90=%.4f  min=%.4f\n",
           t[ITER/2], t[ITER*9/10], t[0]);
}
```

**Гигиена бенчмаркинга** (без этого цифры — мусор):

```bash
sudo cpupower frequency-set -g performance   # фиксируем частоту CPU
taskset -c 2 ./bench                          # приколачиваем к одному ядру
# отключить turbo: echo 1 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo
```

- «Чёрный ящик»: чтобы компилятор не выкинул вычисление, передавайте вход через `volatile` или отдельную единицу трансляции.
- Бенчмарк в отдельной .c-файле от генерации данных — иначе инлайнинг исказит картину.

### Вариант Б — Google Benchmark (индустриальный стандарт, слой на C++)

Код бенчмарков пишется в одном `bench.cc`, тестируемый код остаётся на чистом C:

```cpp
// bench.cc  (линкуется с вашим libmylib.a)
#include <benchmark/benchmark.h>
extern "C" {
void my_sort(int*, size_t);
void gen_data(int*, size_t);
}

static void BM_Sort(benchmark::State& st) {
    std::vector<int> v(st.range(0));
    for (auto _ : st) {
        st.PauseTiming();                    // подготовка не считаем
        gen_data(v.data(), v.size());
        st.ResumeTiming();
        my_sort(v.data(), v.size());         // измеряемая часть
    }
    st.SetItemsProcessed(st.iterations() * v.size());
}
BENCHMARK(BM_Sort)->Range(1000, 1000000)->Unit(benchmark::kMicrosecond);
BENCHMARK_MAIN();
```

```bash
g++ -O2 -g bench.cc -L. -lmylib -lbenchmark -lpthread -o bench
./bench --benchmark_format=json > results.json   # JSON для графиков
python tools/compare.py baseline.json new.json    # регрессии между версиями
```

Это даёт автоматические итерации/статистику (медиана, CV, outliers), сравнение релизов и JSON, из которого matplotlib-ом рисуются графики.

### Вариант В — hyperfine (бенчмарк целого бинарника)

```bash
hyperfine --warmup 3 --export-markdown cmp.md \
    './bench_old --n=100000' './bench_new --n=100000'
```

Даёт таблицу сравнения и boxplot-статистику. Идеален для «до/после оптимизации».

---

## 2. Граф вызовов и горячие пути (функция + строка)

### 2.1 perf — сэмплинг с минимальным оверхедом

```bash
sudo sysctl kernel.perf_event_paranoid=1 kptr_restrict=0   # разрешить профилирование

perf record -F 999 -g --call-graph fp -- ./bench   # сэмплинг 999 Гц
perf report                                        # TUI: сколько % в каждой функции
perf report --stdio --no-children                  # self-время, без "детей"
perf annotate --stdio2                             # ХОТ: построчная разметка
```

`perf annotate` показывает **конкретные строки и ассемблерные инструкции**, где горит CPU — ровно то, что вы просите.

**Пламевидный график (flamegraph) — самый наглядный вид:**

```bash
perf script > perf.stacks
~/tools/FlameGraph/stackcollapse-perf.pl perf.stacks \
  | ~/tools/FlameGraph/flamegraph.pl > flamegraph.svg
```

Открываете в браузере: ширина плашки = доля времени, вертикаль = стек вызовов. Сразу видно горячие пути.

**GUI-альтернатива — [Hotspot](https://github.com/KDAB/hotspot) (KDAB):** скачиваете AppImage, открываете `perf.data` — получаете timeline, дерево вызовов, flamegraph и дизассемблер с исходниками в одном окне. Самый красивый способ смотреть perf-данные.

### 2.2 Callgrind (valgrind) — детерминированный точный граф вызовов

Оверхед 20–50×, зато **никакого сэмплинга** — точные счётчики инструкций по каждой функции и строке. Отлично подходит для тестов и коротких бенчей:

```bash
valgrind --tool=callgrind --cache-sim=yes --branch-sim=yes \
         --callgrind-out-file=callgrind.out ./bench

callgrind_annotate --auto=yes callgrind.out      # текст: функции + строки исходника
kcachegrind callgrind.out                        # GUI: интерактивный граф вызовов
gprof2dot -f callgrind callgrind.out | dot -Tsvg -o callgraph.svg   # SVG-граф
```

- `kcachegrind` — интерактивная карта: кликаете по функции → видите процент инструкций, кэш-промахи I1/D1/LL, входящие/исходящие вызовы и **точные строки**.
- `--toggle-collect=bench_func` — профилировать только нужную функцию (удобно, если тестов много).
- Симуляция кэша покажет, где вы вламываетесь в память — частая причина «горячих» путей.

### 2.3 uftrace — трассировка вызовов с таймингами

Собираете с `-pg` (или без пересборки: `uftrace record -P . --force ./bench`):

```bash
uftrace record --srcline ./bench          # с привязкой к строкам
uftrace report -s self --srcline          # топ функций по self-времени + строки
uftrace graph my_sort                     # ASCII-граф вызовов под функцию
uftrace dump --flame-graph > fg.txt && \
  ~/tools/FlameGraph/flamegraph.pl fg.txt > flame.svg
uftrace tui                               # интерактивный TUI
uftrace record --chrome ./bench           # Chrome trace -> perfetto.dev UI
```

uftrace показывает **реальную последовательность вызовов с длительностями** — это граф вызовов в чистом виде, чего нет у perf.

_(gprof с `-pg`/`gmon.out` тоже работает, но устарел — не рекомендую как основной инструмент.)_

### 2.4 Tracy — интерактивный профайлер в приложении

Если хотите живой GUI с зонами (frame markers, зоны, замеры в коде): [Tracy](https://github.com/wolfpld/tracy) имеет **C API** (`TracyC.h`) — вставляете `ZoneScopedC`/`TracyCZoneN` в код, GUI рисует таймлайн, flame-графы и статистику в реальном времени. Тяжеловат для CI, но фантастичен для разработки.

---

## 3. Massif — профилирование памяти

### Запуск (собирайте с `-g`):

```bash
valgrind --tool=massif \
    --stacks=yes \                # учитывать и стек (по умолч. только heap)
    --time-unit=B \               # время = выделенные байты (наглядно для коротких прогонов)
    --detailed-freq=10 \          # каждый 10-й снапшот подробный (со стеком аллокаций)
    --max-snapshots=200 \
    --massif-out-file=massif.out.%p \
    ./tests
```

### Просмотр отчётов:

```bash
ms_print massif.out.12345 | less          # ASCII-график потребления + пик + стеки
massif-visualizer massif.out.12345        # GUI: красивый график, дерево аллокаций
```

В отчёте: график потребления по времени, **snapshot пика** с деревом вызовов, которые его устроили (файл:строка каждого `malloc`), финальное потребление. Т.е. вы сразу видите «кто надул память».

Полезные опции: `--threshold=0.5` (ловить даже мелкие аллокации), `--peak-only` (для CI — только пик для регрессионного теста).

### Современный бонус — DHAT

```bash
valgrind --tool=dhat ./tests        # -> dhat.out.*.json
```

JSON открывается в [dh_view.html](https://nnethercote.github.io/dh_view/dh_view.html) — показывает total bytes/blocks, **время жизни блоков** и «горячие» места аллокаций (двойной клик по строке — стек). Часто находит то, что massif не показывает.

А утечки — классика: `valgrind --leak-check=full --show-leak-kinds=all ./tests`.

---

## 4. Готовый конвейер (Makefile)

```make
FLAME := ~/tools/FlameGraph
PROF_FLAGS := -O2 -g -fno-omit-frame-pointer

bench:            ## сборка+запуск бенчей (без оверхеда)
	cmake -B build -DCMAKE_BUILD_TYPE=Release
	cmake --build build -j
	taskset -c 2 ./build/bench

flame:            ## flamegraph из perf
	perf record -F 999 -g --call-graph fp -- ./build/bench
	perf script | $(FLAME)/stackcollapse-perf.pl | $(FLAME)/flamegraph.pl > flame.svg
	xdg-open flame.svg

hotspot:          ## GUI поверх perf.data
	hotspot ./perf.data

callgrind:        ## точный граф вызовов + SVG
	valgrind --tool=callgrind --cache-sim=yes --callgrind-out-file=callgrind.out ./build/bench
	gprof2dot -f callgrind callgrind.out | dot -Tsvg -o callgraph.svg
	kcachegrind callgrind.out &

uftrace:
	uftrace record --srcline ./build/bench
	uftrace report -s self --srcline | head -40

massif:           ## память
	valgrind --tool=massif --stacks=yes --time-unit=B ./build/tests
	massif-visualizer massif.out.* &
```
---