#define SIMPLE_DB_VERBOSE
#define CACHE_VERBOSE
#define ENDLESS_DB_TIMEOUT_US 1500000

#include "../include/database.h"
#include "../include/cache.h"

using std::cout;
using std::endl;


int main()
{
	int cache_sz = 3;

	DB::EndlessDB endless;
	Cache::LRUCache<DB::EndlessDB> cache(endless, cache_sz);
	std::vector<int> queries = {1, 2, 3, 4, 2, 1};

	for (int query : queries) {
		auto page = cache.get_temp_page(query);
		std::cout << "-----> page[" << query << "]: '" << page << "'\n\n";
	}

	return 0;
}
