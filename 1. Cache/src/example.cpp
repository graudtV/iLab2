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

	SimpleDB db;
	FileSystemDB fsdb;

	DummyCache<SimpleDB> cached_db(db);
	DummyCache<FileSystemDB> cached_fsdb(fsdb);
	RandomCache<SimpleDB> random_cache(db, 5);


	LRUCache<SimpleDB> lru_cache(db, 5);

	cout << "RandomCache<SimpleDB>:\t"
		<< test_cache<RandomCache<SimpleDB>>(10, 100, 100000) << endl;
	cout << "LRUCache<SimpleDB>:\t"
		<< test_cache<LRUCache<SimpleDB>>(10, 100, 100000) << endl;

	BeladyCache<SimpleDB> belady(db, 10);

	// cout << db.get_page(228) << endl;
	// cout << cached_db.get_page(228) << endl;

	// const std::string& s1 = cached_fsdb.get_page("in");
	// const std::string& s2 = cached_fsdb.get_temp_page("in2");
	// cout << "'" << s1 << "'"<< endl;
	// cout << "'" << s2 << "'"<< endl;

	return 0;
}
