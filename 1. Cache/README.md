# Cache
Реализация и сравнительный анализ алгоритмов кэширования

## Build
Компиляция всех исходников

Через clang:
```
make
```
Через gcc:
```
make CC=g++ CFLAGS=-std=c++11
```
## Efficiency tests
Запуск тестов эффективности с параметрами по умолчанию:
```
make run-test
```
Выбор параметров вручную:
```
bin/test/test_efficiency [OPTIONS] <nlookups> <ndifferent_queries> <cache_sz>
```
**OPTIONS**:
- **-r**	--	random queries test
- **-g**	--	graph-like queries test
