#ifndef _TESTING_FACILITIES_H_
#define _TESTING_FACILITIES_H_

#include "graph.h"
#include <cstddef>
#include "timer.h"
#include <cassert>

namespace Cache {

struct TestResult {
	int nhits, nlookups;
	uint64_t usec; // Затраченное процессорное время

	TestResult(int hits, int lookups, uint64_t usecs) :
		nhits(hits), nlookups(lookups), usec(usecs) {}
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
	mytime::Timer timer;

	for (; queries_from != queries_to; ++queries_from)
		cache.get_temp_page(*queries_from);

	return TestResult(cache.nhits(), cache.nlookups(), timer.elapsed_us());
}

/*  Функция аналогична test_cache(),
 * но DummyCache не принимает размера в конструкторе,
 * поэтому отдельная функция */
template <class DataBase, class InputIt>
TestResult test_dummy_cache(const DataBase& db, size_t cache_sz,
	InputIt queries_from, InputIt queries_to)
{
	Cache::DummyCache<DataBase> cache(db);
	mytime::Timer timer;

	for (; queries_from != queries_to; ++queries_from)
		cache.get_temp_page(*queries_from);
	
	return TestResult(cache.nhits(), cache.nlookups(), timer.elapsed_us());
}

/*  Функция аналогична test_cache(),
 * но BeladyCache требует предсказание следующих запросов
 * при вызовах get_page(), get_temp_page(), поэтому отдельная функция */
template <class DataBase, class InputIt>
TestResult test_belady_cache(const DataBase& db, size_t cache_sz,
	InputIt queries_from, InputIt queries_to)
{
	Cache::BeladyCache<DataBase> cache(db, cache_sz);
	mytime::Timer timer;

	for (; queries_from != queries_to; ++queries_from)
		cache.get_temp_page(*queries_from, queries_from, queries_to);
	
	return TestResult(cache.nhits(), cache.nlookups(), timer.elapsed_us());
}

template <class T>
class GraphRandomWalkIt {
public:
	GraphRandomWalkIt(typename Graph<T>::Iterator it) :
		m_it(it) {}
	GraphRandomWalkIt& operator ++()
	{
		if (m_it.degree() != 0) {
			int bias = rand() % m_it.degree();	
			m_it = m_it.begin();
			for (int i = 0; i < bias; ++i)
				++m_it;
		}
		return *this;
	}
	T& operator *()
		{ return *m_it; }
private:
	typename Graph<T>::Iterator m_it;
};

/*! \brief Создает случайный неориентированный связный граф из nnodes узлов
 *
 *  Все узлы ращличны и имеют числовые значения от 0 до nnodes - 1
 *
 * \param links_per_node характеризует плотность графа, а именно
 *  максимальное число связей, которое может быть у каждого из узлов.
 *  links_per_node должно быть положительным числом */
Graph<int> generate_random_graph(int nnodes, int links_per_node)
{
	assert(links_per_node > 0);
	Graph<int> graph;
	std::vector<Graph<int>::Iterator> nodes;

	/* Создаем узлы */
	for (int node_val = 0; node_val < nnodes; ++node_val)
		nodes.push_back(graph.add_node(node_val));

	/* К каждому узлу пытаемся привязать links_per_node узлов */
	for (auto node1 = graph.begin(); node1 != graph.end(); ++node1)
		for (int i = 0; i < links_per_node; ++i) {
			auto node2 = nodes[rand() % nnodes];
			if (!node1.linked_with(node2))
				graph.add_link(node1, node2);
		}

	return graph;
}

/*! \brief Создает vector из nqueries случайных чисел
 *  от 0 до ndifferent_queries - 1 */
std::vector<int> generate_random_queries(int nqueries, int ndifferent_queries)
{
	std::vector<int> queries;
	for (int i = 0; i < nqueries; ++i)
		queries.push_back(rand() % ndifferent_queries);
	return queries;
}

/*! \brief Создает запросы наподобие хождения по графу
 *
 *  Создает vector из nqueries чисел от 0 до ndifferent_queries - 1,
 * числа эмулируют хождение пользователя по графу, например по сайтам в
 * интернете: каждый сайт содержит ссылки на другие сайты,
 * пользователь "нажимает" ссылки в случайном порядке. Ссылки являются
 * двусторонними (пользователь всегда может вернуться на предыдущий сайт)
 *  Граф и обход графа создаются случайно
 *
 * \param nqueries - число запросов, равно размеру возвращаемого массива
 * \param ndifferent_queres - число различных запросов, должно быть
 *  положительным числом
 * \param links_per_node - характеризует плотность графа, а именно
 *  максимальное число связей, которое может быть у каждого из узлов.
 *  Должно быть положительным числом.
 *
 *  Имеет смысл делать граф разреженным, т.е links_per_node намного
 * меньше ndifferent_queries, так при большом числе связей запросы
 * вырождаются в случайные (не связанные друг с другом)
 */
std::vector<int> generate_graph_queries(int nqueries,
	int ndifferent_queries, int links_per_node)
{
	assert(links_per_node > 0);
	assert(ndifferent_queries > 0);
	std::vector<int> queries;
	
	auto graph = generate_random_graph(ndifferent_queries, links_per_node);

	GraphRandomWalkIt<int> it(graph.begin());
	for (int i = 0; i < nqueries; ++i) {
		queries.push_back(*it);
		++it;
	}
	return queries;
}

} // Cache namespace end

#endif // _TESTING_FACILITIES_H_