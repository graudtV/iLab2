#include "../geometry.h"
#include <iostream>

namespace {

std::istream&
operator >>(std::istream& is, Geometry::Point& pnt)
	{ return is >> pnt.x >> pnt.y >> pnt.z; }

}

int main()
{
	int ntriangles = 0;
	std::cin >> ntriangles;

	std::vector<Geometry::Triangle> trgs;
	trgs.reserve(ntriangles);

	for (int i = 0; i < ntriangles; ++i) {
		Geometry::Point a, b, c;
		std::cin >> a >> b >> c;
		trgs.push_back({a, b, c});
	}

	std::cout << nintersections(trgs.begin(), trgs.end()) << std::endl;


	return 0;
}