# Cache
Реализация и сравнительный анализ алгоритмов кэширования

## Build
Компиляция всех исходников:
```
make
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
