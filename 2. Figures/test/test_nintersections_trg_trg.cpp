#include "../geometry.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <vector>
#include <set>

namespace {

std::istream&
operator >>(std::istream& is, Geometry::Point& pnt)
	{ return is >> pnt.x >> pnt.y >> pnt.z; }

}

void usage_error()
{
	fprintf(stderr, "Usage: test_intersections_trg_trg [-nib]\n");
	fprintf(stderr, "\t-n\t--\tprint number of intersections (will be first number in output)\n");
	fprintf(stderr, "\t-i\t--\tprint indices of intersected triangles (first triangle has index 0)\n");
	fprintf(stderr, "\t-b\t--\tuse benchmark methods (Complexity up to O(n^2))\n");
	fprintf(stderr, "\tinput format (from stdin): ntriangles trg1.pnt1.x trg1.pnt1.y"
		" trg1.pnt1.z trg1.pnt2.x ...\n");
	fprintf(stderr, "\toutput: values specified by [-ni] will be written to stdout\n");
	exit(EXIT_FAILURE);	
}

int main(int argc, char *argv[])
{
	std::ios::sync_with_stdio(true);

	int opt = 0;
	int opt_benchmark = 0;
	int opt_print_nintersections = 0;
	int opt_print_intersected_trgs_indices = 0;

	while ((opt = getopt(argc, argv, "bni?")) != -1) {
		switch (opt) {
		case 'b': opt_benchmark = 1; break;
		case 'n': opt_print_nintersections = 1; break;
		case 'i': opt_print_intersected_trgs_indices = 1; break;
		case '?': default: usage_error(); break;
		}
	}
	if (!opt_print_nintersections && !opt_print_intersected_trgs_indices)
		usage_error();

	int ntriangles = 0;
	std::cin >> ntriangles;

	std::vector<Geometry::Triangle> trgs;
	trgs.reserve(ntriangles);

	for (int i = 0; i < ntriangles; ++i) {
		Geometry::Point a, b, c;
		std::cin >> a >> b >> c;
		trgs.push_back({a, b, c});
	}

	if (opt_print_nintersections) {
		std::cout << ((opt_benchmark)
			? nintersections_benchmark(trgs.begin(), trgs.end())
			: nintersections(trgs.begin(), trgs.end())) << std::endl;
	}
		
	if (opt_print_intersected_trgs_indices) {
		auto intrstns_table = build_intersections_table(trgs.begin(), trgs.end());
		std::set<std::vector<Geometry::Triangle>::iterator> trg_iterators;
		for (auto& entry: intrstns_table) {
			trg_iterators.insert(entry[0]);
			trg_iterators.insert(entry[1]);
		}
		for (auto it : trg_iterators)
			std::cout << it - trgs.begin() << " ";
		std::cout << std::endl;
	}

	return 0;
}