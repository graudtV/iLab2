//#define SIMPLE_DB_VERBOSE
//#define CACHE_VERBOSE

#include "../include/database.h"
#include "../include/cache.h"

using std::cout;
using std::endl;

struct TestCacheResult {
	int nhits, nlookups;

	TestCacheResult(int hits, int lookups) :
		nhits(hits), nlookups(lookups) {}

	friend std::ostream& operator <<
		(std::ostream& os, const TestCacheResult& res);
};

std::ostream&
operator <<(std::ostream& os, const TestCacheResult& res)
{
	os << "hits = " << res.nhits
		<< "\tlookups = " << res.nlookups
		<< "\thit ratio = "
		<< ( static_cast<double>(res.nhits) / res.nlookups);
	return os;
}


template <class Cache>
TestCacheResult test_cache
	(size_t cache_sz, int npages, int niterations)
{
	typename Cache::database_t db;
	Cache cache(db, cache_sz);

	for (int i = 0; i < niterations; ++i)
		cache.get_temp_page(rand() % npages);

	return TestCacheResult(cache.nhits(), cache.nlookups());
}


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

	cout << "RandomCache<EndlessDB>:\t"
		<< test_cache<RandomCache<EndlessDB>>(10, 100, 100000) << endl;
	cout << "LRUCache<EndlessDB>:\t"
		<< test_cache<LRUCache<EndlessDB>>(10, 100, 100000) << endl;


	return 0;
}
