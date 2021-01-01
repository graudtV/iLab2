#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <iomanip>
#include "../geometry.h"

namespace {

/* from 0 to 1 */
float rnd_float()
	{ return static_cast<float>(rand()) / static_cast<float>(RAND_MAX); }

/* from min to max */
float rnd_float(float min, float max)
	{ return rnd_float() * (max - min) + min; }

Geometry::Point rnd_point(float mincoord, float maxcoord)
{
	return Geometry::Point(
		rnd_float(mincoord, maxcoord),
		rnd_float(mincoord, maxcoord),
		rnd_float(mincoord, maxcoord)
		);
}

Geometry::Triangle rnd_triangle(float mincoord, float maxcoord)
{
	return Geometry::Triangle(
		rnd_point(mincoord, maxcoord),
		rnd_point(mincoord, maxcoord),
		rnd_point(mincoord, maxcoord)
		);
}

void usage_error()
{
	fprintf(stderr, "trggen usage: ./trggen [-s] ntriangles\n");
	fprintf(stderr, "\t-s\t--\tgenerate sparse triangles\n");
	exit(EXIT_FAILURE);	
}

template <class Function>
void generate(int ntriangles, Function random_triangle_generator)
{
	for (int i = 0; i < ntriangles; ++i) {
		while (1) {
			Geometry::Triangle trg = random_triangle_generator();
			if (!trg.valid())
				continue;
			std::cout
				<< trg.a.x << ' ' << trg.a.y << ' ' << trg.a.z << ' '
				<< trg.b.x << ' ' << trg.b.y << ' ' << trg.b.z << ' '
				<< trg.c.x << ' ' << trg.c.y << ' ' << trg.c.z << std::endl;
			break;
		}
	}
}

/* Triangles coords will variate from -maxcoord to maxcoord (maxcoords > 0) */
const float maxcoord = 1e6;

} // anonymous namespace end

int main(int argc, char *argv[])
{
	std::ios::sync_with_stdio(true);

	int opt = 0;
	int opt_sparse = 0;
	int ntriangles = 0;

	while ((opt = getopt(argc, argv, "s")) != -1) {
		switch (opt) {
		case 's': opt_sparse = 1; break;
		default: usage_error(); break;			
		}
	}

	argv += optind;
	argc -= optind;

	if (argc != 1 || sscanf(*argv, "%d", &ntriangles) != 1)
		usage_error();

	srand(time(0));

	auto sparse_trg_generator = [=]() {
		/* Placing triangles in small random cells in space
		 * Number of cells = ntriangles */
		static float cell_sz = maxcoord / pow(ntriangles, 1.0/3.0);
		float cell_start = rnd_float(-maxcoord, maxcoord);
		return rnd_triangle(cell_start, cell_start + cell_sz);
	};

	//std::cout << std::setprecision(5);
	printf("%d\n", ntriangles);
	if (opt_sparse) {
		std::cout << std::setprecision(5);
		generate(ntriangles, sparse_trg_generator);
	}
	else
		generate(ntriangles, [=](){ return rnd_triangle(-maxcoord, maxcoord); });

}