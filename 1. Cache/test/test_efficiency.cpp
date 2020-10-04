/* ./test_efficiency [OPTIONS] <nlookups> <ndifferent_queries> <cache_sz>
 * OPTIONS: -r, -t, -g (random, tree, graph queries) */
#define NDEBUG

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <unistd.h>
#include "../include/cache.h"
#include "testing_facilities.h"

namespace {

std::ostream& operator <<(std::ostream& os, const TestResult& res)
{
	os << std::fixed;
	os	<< std::setw(10) << res.nhits
		<< std::setw(12) << res.nlookups
		<< std::setw(13) << std::defaultfloat << (static_cast<double>(res.nhits) / res.nlookups)
		<< std::setw(13) << std::setprecision(2) << res.usec / 1e6 // total time, sec
		<< std::setprecision(3) << static_cast<double>(res.usec) / res.nlookups;
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

	int shift_sz = 15;
	auto shift = std::setw(shift_sz);

	std::cout
		<< std::right
		<< std::setw(shift_sz * 2) << "*******  " << test_title << "  *******" << std::endl
		<< shift << "" << " HITS      LOOKUPS     HIT RATIO    TIME(sec)    SPEED(usec/query)\n";
	std::cout
		<< shift << left << "DummyCache"	<< ' '
		<<  test_dummy_cache			(db, cache_sz, queries_from, queries_to)	<< std::endl
		<< shift << left << "RandomCache" << ' '
		<<  test_cache<RandomCache<DB>>	(db, cache_sz, queries_from, queries_to)	<< std::endl
		<< shift << left << "LRUCache" << ' '
		<<  test_cache<LRUCache<DB>>	(db, cache_sz, queries_from, queries_to)	<< std::endl
		<< shift << left << "BeladyCache" << ' '
		<<  test_belady_cache			(db, cache_sz, queries_from, queries_to)	<< "\n\n";	
}

void run_all_tests(const std::string& test_title, int cache_sz,
	const typename std::vector<int>& queries)
{
	run_all_tests(test_title, cache_sz, queries.begin(), queries.end());
}


void usage_error(const char *progname, const char *err_info)
{
	fprintf(stderr, "%s: %s\n", progname,
		(err_info) ? err_info : "incorrect usage");
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "\t%s [OPTIONS] <nlookups> <ndifferent_queries> <cache_sz>\n", progname);
	fprintf(stderr, "\tOPTIONS:\t-r\t--\trandom queries test\n");
	fprintf(stderr, "\t        \t-g\t--\tgraph-like queries test\n");
	exit(EXIT_FAILURE);
}

} // anonymous namespace end

int main(int argc, char *argv[])
{
	const char * const progname = argv[0];
	int opt_random_queries = 0;
	int opt_graph_queries = 0;
	int opt = 0;

	while ((opt = getopt(argc, argv, "rg")) != -1) {
		switch (opt) {
		case 'r': opt_random_queries = 1; break;
		case 'g': opt_graph_queries = 1; break;
		default: exit(EXIT_FAILURE);
		}
	}
	argc -= optind;
	argv += optind;

	if (argc < 3)
		usage_error(progname, "not enough args");
	if (argc > 3)
		usage_error(progname, "too many args");
	if (!opt_random_queries && !opt_graph_queries)
		usage_error(progname, "no test specified, see OPTIONS");

	int nlookups = 0;
	int ndifferent_queries = 0;
	int cache_sz = 0;

	if (sscanf(argv[0], "%d", &nlookups) != 1
		|| sscanf(argv[1], "%d", &ndifferent_queries) != 1
		|| sscanf(argv[2], "%d", &cache_sz) != 1
		|| nlookups <= 0
		|| ndifferent_queries <= 0
		|| cache_sz <= 0)
		usage_error(progname, "last 3 arguments must be positive numbers");

	srand(time(0));

	if (opt_random_queries) {
		auto random_queries = generate_random_queries(nlookups, ndifferent_queries);
		run_all_tests("RANDOM QUERIES", cache_sz, random_queries);
	}
	if (opt_graph_queries) {
		auto graph_queries = generate_graph_queries(nlookups, ndifferent_queries, 1);	
		run_all_tests("GRAPH-LIKE QUERIES [1 link per node]", cache_sz, graph_queries);
		graph_queries = generate_graph_queries(nlookups, ndifferent_queries, 2);	
		run_all_tests("GRAPH-LIKE QUERIES [2 links per node]", cache_sz, graph_queries);
		graph_queries = generate_graph_queries(nlookups, ndifferent_queries, 3);	
		run_all_tests("GRAPH-LIKE QUERIES [3 links per node]", cache_sz, graph_queries);
	}

	return 0;
}