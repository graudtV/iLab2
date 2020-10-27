#include "../geometry.h"
#include <iostream>

namespace {

std::istream&
operator >>(std::istream& is, Geometry::Point& pnt)
	{ return is >> pnt.x >> pnt.y >> pnt.z; }

std::istream&
operator >>(std::istream& is, Geometry::Triangle& trg)
	{ return is >> trg.a >> trg.b >> trg.c; }

}

int main()
{
	int ntriangles = 0;
	std::cin >> ntriangles;

	std::vector<Geometry::Triangle> trgs;
	trgs.reserve(ntriangles);

	for (int i = 0; i < ntriangles; ++i)
		std::cin >> trgs[i];

	for (auto& trg : trgs)
		std::cout << trg << std::endl;

	return 0;
}