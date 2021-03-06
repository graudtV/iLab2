#include "../geometry.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <vector>
#include <set>
#include <iomanip>

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
	using vec_it_t = std::vector<Geometry::Triangle>::iterator;
	const auto nintersections_benchmark = Geometry::nintersections_benchmark<vec_it_t>;
	const auto get_intersected_figures_indices_benchmark
		= Geometry::get_intersected_figures_indices_benchmark<vec_it_t>;

	std::ios::sync_with_stdio(true);
	srand(time(0));
	//std::cin >> std::setprecision(5);

	int opt = 0;
	int opt_benchmark = 0;
	int opt_print_nintersections = 0;
	int opt_print_intersected_trgs_indices = 0;

	while ((opt = getopt(argc, argv, "bni")) != -1) {
		switch (opt) {
		case 'b': opt_benchmark = 1; break;
		case 'n': opt_print_nintersections = 1; break;
		case 'i': opt_print_intersected_trgs_indices = 1; break;
		default: usage_error(); break;
		}
	}
	if (!opt_print_nintersections && !opt_print_intersected_trgs_indices)
		opt_print_intersected_trgs_indices = 1;

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
		auto intersected_trgs = (opt_benchmark)
			? get_intersected_figures_indices_benchmark(trgs.begin(), trgs.end())
			: get_intersected_figures_indices(trgs.begin(), trgs.end());

		for (int idx : intersected_trgs)
			std::cout << idx << " ";
		std::cout << std::endl;
	}

	return 0;
}