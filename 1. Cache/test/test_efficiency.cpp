#include <iostream>
#include <iomanip>
#include <ctime>
#include "../include/cache.h"
#include "testing_facilities.h"
#include <cstdlib>

namespace {

std::ostream& operator <<(std::ostream& os, const TestResult& res)
{
	os	<< std::setw(10) << res.nhits
		<< std::setw(12) << res.nlookups
		<< (static_cast<double>(res.nhits) / res.nlookups);
	return os;
}

/*  Тестирует эффективность всех реализованных кэшей на переданном
 * наборе запросов (ключей) и выводит результаты в консоль
 *  В качестве базы данных используется EndlessDB, поэтому ключом
 * может быть любое число типа int
 */
void run_all_tests(
	const std::string& test_title,
	int cache_sz,
	typename std::vector<int>::const_iterator queries_from,
	typename std::vector<int>::const_iterator queries_to)
{
	using DB = QuickEndlessDB;
	DB db;

	using std::left;

	int shift_sz = 20;
	auto shift = std::setw(shift_sz);

	TestResult dummy	= test_dummy_cache				(db, cache_sz, queries_from, queries_to);
	TestResult random	= test_cache<RandomCache<DB>>	(db, cache_sz, queries_from, queries_to);
	TestResult lru		= test_cache<LRUCache<DB>>		(db, cache_sz, queries_from, queries_to);
	TestResult belady	= test_belady_cache				(db, cache_sz, queries_from, queries_to);

	std::cout
		<< std::right
		<< shift << "*******  " << test_title << "  *******" << std::endl
		<< shift << "" << " HITS      LOOKUPS     HIT RATIO\n";
	std::cout
		<< shift << left << "DummyCache"	<< ' '	<<  dummy	<< std::endl
		<< shift << left << "RandomCache" << ' '	<<  random	<< std::endl
		<< shift << left << "LRUCache" << ' '	<<  lru		<< std::endl
		<< shift << left << "BeladyCach" << ' '	<<  belady	<< "\n\n";	
}

void run_all_tests(const std::string& test_title, int cache_sz,
	const typename std::vector<int>& queries)
{
	run_all_tests(test_title, cache_sz, queries.begin(), queries.end());
}

} // anonymous namespace end


void usage_error(const char *progname, const char *err_info)
{
	fprintf(stderr, "%s: %s\n", progname,
		(err_info) ? err_info : "incorrect usage");
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "\t%s <nlookups> <different_queries> <cache_sz>\n", progname);
	exit(EXIT_FAILURE);
}

/* ./test <nlookups> <different_queries> <cache_sz> */
int main(int argc, char *argv[])
{
	const char * const progname = argv[0];
	if (argc < 4)
		usage_error(progname, "not enough args");
	if (argc > 4)
		usage_error(progname, "too many args");

	int nlookups = 0;
	int ndifferent_queries = 0;
	int cache_sz = 0;

	if (sscanf(argv[1], "%d", &nlookups) != 1
		|| sscanf(argv[2], "%d", &ndifferent_queries) != 1
		|| sscanf(argv[3], "%d", &cache_sz) != 1
		|| nlookups < 0
		|| ndifferent_queries < 0
		|| cache_sz < 1)
		usage_error(progname, "last 3 arguments must be positive numbers");

	srand(time(0));
	auto random_queries = generate_random_queries(nlookups, ndifferent_queries);
	auto tree_queries = generate_tree_queries(nlookups, ndifferent_queries);
	auto graph_queries = generate_graph_queries(nlookups, ndifferent_queries);
	
	run_all_tests("RANDOM QUERIES", cache_sz, random_queries);
	run_all_tests("TREE-LIKE QUERIES", cache_sz, tree_queries);
	run_all_tests("GRAPH-LIKE QUERIES", cache_sz, graph_queries);

	return 0;
}