#include <iostream>
#include <cstdlib>
#include "../geometry.h"

namespace {

// от 0 до 1000
float rnd_float()
	{ return 1000 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX); }

Geometry::Point gen_point()
	{ return Geometry::Point(rnd_float(), rnd_float(), rnd_float()); }

}

int main(int argc, const char *argv[])
{
	int ntriangles = 0;
	if (argc != 2
		|| sscanf(argv[1], "%d", &ntriangles) != 1) {
		fprintf(stderr, "datagen usage: ./datagen ntriangles\n");
		exit(EXIT_FAILURE);
	}

	srand(time(0));

	printf("%d\n", ntriangles);
	for (int i = 0; i < ntriangles; ++i) {
		while (1) {
			Geometry::Triangle trg(gen_point(), gen_point(), gen_point());
			if (trg.valid()) {
				printf("%lg %lg %lg %lg %lg %lg %lg %lg %lg\n",
					float(trg.a.x), float(trg.a.y), float(trg.a.z),
					float(trg.b.x), float(trg.b.y), float(trg.b.z),
					float(trg.c.x), float(trg.c.y), float(trg.c.z)
					);
				break;
			}
		}
	}
}