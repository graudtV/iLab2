// #define SIMPLE_DB_VERBOSE
// #define CACHE_VERBOSE

#include "../include/database.h"
#include "../include/cache.h"
#include "../test/testing_facilities.h"

using std::cout;
using std::endl;

std::ostream& operator <<
	(std::ostream& os, const TestResult& res)
{
	os << "hits = " << res.nhits
		<< "\tlookups = " << res.nlookups
		<< "\thit ratio = "
		<< ( static_cast<double>(res.nhits) / res.nlookups);
	return os;
}


/* debug print */
#define DPRINT(var) cout << #var ":\n\t" << (var) << endl

int main()
{
	srand(time(NULL));
	using Simple = SimpleDB<int, int>;

	EndlessDB endless;
	FileSystemDB filesystem;
	Simple simple;

	DummyCache<EndlessDB> dummy_endless(endless);
	DummyCache<FileSystemDB> dummy_filesystem(filesystem);
	DummyCache<Simple> dummy_simple(simple);

	RandomCache<EndlessDB> random_endless(endless, 5);
	RandomCache<FileSystemDB> random_filesystem(filesystem, 5);
	RandomCache<Simple> random_simple(simple, 5);

	LRUCache<EndlessDB> lru_endless(endless, 5);
	LRUCache<FileSystemDB> lru_filesystem(filesystem, 5);
	LRUCache<Simple> lru_simple(simple, 5);

	BeladyCache<EndlessDB> belady_endless(endless, 5);
	BeladyCache<FileSystemDB> belady_filesystem(filesystem, 5);
	BeladyCache<Simple> belady_simple(simple, 5); 

	std::vector<int> queries;
	for (int i = 0; i < 1000000; ++i)
		queries.push_back(rand() % 11);

	DPRINT(test_dummy_cache						(endless, 10, queries.begin(), queries.end()));
	DPRINT(test_cache<RandomCache<EndlessDB>>	(endless, 10, queries.begin(), queries.end()));
	DPRINT(test_cache<LRUCache<EndlessDB>>		(endless, 10, queries.begin(), queries.end()));
	DPRINT(test_belady_cache					(endless, 10, queries.begin(), queries.end()));

	//test_cache<RandomCache<EndlessDB>>(endless, 10, queries.begin(), queries.end());
	return 0;
}
