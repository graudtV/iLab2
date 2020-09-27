#ifndef _TESTING_FACILITIES_H_
#define _TESTING_FACILITIES_H_

#include "graph.h"

struct TestResult {
	int nhits, nlookups;

	TestResult(int hits, int lookups) :
		nhits(hits), nlookups(lookups) {}
};

/*  Тестирует Cache на запросах(ключах) из [queries_from, queries_to)
 *  Запросы должны иметь тип, который может быть неявно преобразован
 * к Cache::key_t
 *  База данных db, к которой обращается кэш, должна содержать страницы
 * для всех запрашиваемых ключей. Лучше всего для этого использовать
 * QuickEndlessDB. */
template <class Cache, class InputIt>
TestResult test_cache(const typename Cache::database_t& db, size_t cache_sz,
	InputIt queries_from, InputIt queries_to)
{
	Cache cache(db, cache_sz);

	for (; queries_from != queries_to; ++queries_from)
		cache.get_temp_page(*queries_from);
	
	return TestResult(cache.nhits(), cache.nlookups());
}

/*  Функция аналогична test_cache(),
 * но DummyCache не принимает размера в конструкторе,
 * поэтому отдельная функция */
template <class DataBase, class InputIt>
TestResult test_dummy_cache(const DataBase& db, size_t cache_sz,
	InputIt queries_from, InputIt queries_to)
{
	DummyCache<DataBase> cache(db);

	for (; queries_from != queries_to; ++queries_from)
		cache.get_temp_page(*queries_from);
	
	return TestResult(cache.nhits(), cache.nlookups());
}

/*  Функция аналогична test_cache(),
 * но BeladyCache требует предсказание следующих запросов
 * при вызовах get_page(), get_temp_page(), поэтому отдельная функция */
template <class DataBase, class InputIt>
TestResult test_belady_cache(const DataBase& db, size_t cache_sz,
	InputIt queries_from, InputIt queries_to)
{
	BeladyCache<DataBase> cache(db, cache_sz);

	for (; queries_from != queries_to; ++queries_from)
		cache.get_temp_page(*queries_from, queries_from, queries_to);
	
	return TestResult(cache.nhits(), cache.nlookups());
}

/* Создает vector из nqueries случайных чисел от 0 до ndifferent_queries - 1 */
std::vector<int> generate_random_queries(int nqueries, int ndifferent_queries)
{
	std::vector<int> queries;
	for (int i = 0; i < nqueries; ++i)
		queries.push_back(rand() % ndifferent_queries);
	return queries;
}

/*  Создает vector из nqueries чисел от 0 до ndifferent_queries - 1,
 * числа эмулируют хождение пользователя "по дереву", например по папкам и файлам
 * в файловой системе
 *  Дерево и обход дерева создаются случайно */
std::vector<int> generate_tree_queries(int nqueries, int ndifferent_queries)
{
	std::vector<int> queries;
	return queries;
}

/*  Создает vector из nqueries чисел от 0 до ndifferent_queries - 1,
 * числа эмулируют хождение пользователя по графу, например по сайтам в
 * интернете: каждый сайт содержит ссылки на другие сайты,
 * пользователь "нажимает" ссылки в случайном порядке.
 *  Ссылки двусторонние, т.е. пользователь всегда может вернуться
 * на предыдущий сайт.
 *  Граф и обход графа создаются случайно */
std::vector<int> generate_graph_queries(int nqueries, int ndifferent_queries)
{
	std::vector<int> queries;
	return queries;
}

#endif // _TESTING_FACILITIES_H_