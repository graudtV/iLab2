#include "../geometry.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <vector>

namespace {

std::istream&
operator >>(std::istream& is, Geometry::Point& pnt)
	{ return is >> pnt.x >> pnt.y >> pnt.z; }

}

int main(int argc, char *argv[])
{
	int opt = 0;
	int opt_benchmark = 0;

	while ((opt = getopt(argc, argv, "b?")) != -1) {
		switch (opt) {
		case 'b': opt_benchmark = 1; break;
		case '?': default:
			fprintf(stderr, "Usage: test_nintersections_trg_trg [-d]\n");
			fprintf(stderr, "\t-b\t--\tuse benchmark nintersections() (Complexity O(n^2))\n");
			fprintf(stderr, "\tinput format (from stdin): ntriangles trg1.pnt1.x trg1.pnt1.y"
				" trg1.pnt1.z trg1.pnt2.x ...\n");
			exit(EXIT_FAILURE);
		}
	}

	int ntriangles = 0;
	std::cin >> ntriangles;

	std::vector<Geometry::Triangle> trgs;
	trgs.reserve(ntriangles);

	for (int i = 0; i < ntriangles; ++i) {
		Geometry::Point a, b, c;
		std::cin >> a >> b >> c;
		trgs.push_back({a, b, c});
	}

	std::cout << ((opt_benchmark)
			? nintersections_helper(trgs.begin(), trgs.end(), 0, 0)
			: nintersections(trgs.begin(), trgs.end()))
		<< std::endl;


	return 0;
}