/* Проверяет LRUCache на соответствие стратегии LRU */

#include <iostream>
#include <cstdlib>
#include "../include/database.h"
#include "../include/cache.h"

#define ARR_SZ(arr) (sizeof (arr) / sizeof (*arr))

int main()
{
	const int cache_sz = 3;

	SimpleDB db;
	LRUCache<SimpleDB> cache(db, cache_sz);

	int queries[] = { 1, 2, 3, 4, 1, 2, 5, 1, 2, 4, 3, 4 };
	int expected[][cache_sz] = {
		{},
		{},
		{ 1, 2, 3 },
		{ 2, 3, 4 },
		{ 3, 4, 1 },
		{ 4, 1, 2 },
		{ 1, 2, 5 },
		{ 2, 5, 1 },
		{ 5, 1, 2 },
		{ 1, 2, 4 },
		{ 2, 4, 3 },
		{ 2, 3, 4 }
	};

	for (int i = 0; i < cache_sz - 1; ++i)
		cache.get_temp_page(queries[i]);

	for (int i = cache_sz - 1; i < ARR_SZ(queries); ++i) {
		cache.get_temp_page(queries[i]);
		for (int j = 0; j < cache_sz; ++j)
			if (!cache.is_cached(expected[i][j])) {
				fprintf(stderr, "LRU test failed: LRUCache doesn't follow LRU strategy\n");
				fprintf(stderr, "Queries to cache (from first to last): ( ");
				for (int k = 0; k <= i; ++k)
					fprintf(stderr, "%d ", queries[k]);
				fprintf(stderr, ")\n");
				fprintf(stderr, "Cache expected to contain ( %d %d %d )",
					expected[i][0], expected[i][1], expected[i][2]);
				fprintf(stderr, ", but cache doesn't contain page %d\n",
					expected[i][j]);

				exit(1);
			}
	}


	return 0;
}