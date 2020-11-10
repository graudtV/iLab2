/* Intersections of groups of geometric objects (realization of non-templates) */

#include "geometry.h"
#include <random>
#include <cassert>

namespace Geometry {
namespace {

/*  returns 0 - if trg has common points with plane base
 *  returns 1 - if all of trg vertices are in a halfplane, which
 * is pointed by base normal
 *  returns -1 - if all of trg vertices are in a halfplane,
 * which is opposit to the one pointed by base plane normal */
int halfplane_detector(const Triangle& base, const Triangle& trg)
{
	Plane plane(base);
	Vector n = plane.normal();
	Float a = Vector::inner_product(n, vdistance(trg.a, plane));
	Float b = Vector::inner_product(n, vdistance(trg.b, plane));
	Float c = Vector::inner_product(n, vdistance(trg.c, plane));
	if (a > 0 && b > 0 && c > 0)
		return -1;
	if (a < 0 && b < 0 && c < 0)
		return 1;
	return 0;
}

} // anonymous namespace end

template <class RandomIt>
void random_base_choise(RandomIt fst, RandomIt last)
	{ std::swap(fst[0], fst[rand() % (last - fst)]); }

/*  Algorithm for counting number of intersections between
 * triangles. More effective than generic algorithm (see nintersections())
 *  Complexity: O(nlog(n)^2) on average, O(n^2) in the worst case */
template <>
int nintersections_helper<Triangle>(
	references_vector_iterator_t<Triangle> trgs_fst,
	references_vector_iterator_t<Triangle> trgs_last)
{
	if (trgs_fst == trgs_last || std::next(trgs_fst) == trgs_last) // 2 triangles minimum needed
		return 0;

	random_base_choise(trgs_fst, trgs_last);
	const Triangle& base = *trgs_fst; // Опорный элемент
	auto fst_after_base = std::next(trgs_fst);

	auto borders = other::split(fst_after_base, trgs_last,
		[base](const Triangle& trg) { return halfplane_detector(base, trg) == 0; }, // both halfplanes
		[base](const Triangle& trg) { return halfplane_detector(base, trg) < 0; } // left halfplane
		);
	/* trgs_fst   -- borders[0]	-	both halfplanes - "middle"
	 * borders[0] -- borders[1]	-	left halfplane
	 * borders[1] -- trgs_last	-	right halfplane */

	/*  If there is no triangles, which are completely left or completely right,
	 * we need to do smth, or there will be infinite recursion. So, we decrease
	 * number of triangles by one and try again. In the worst case complexity is n^2 */
	if (borders[0] == trgs_last)
		return ncrossintersections_helper<Triangle, Triangle>(
				trgs_fst, fst_after_base, fst_after_base, trgs_last)
			+ nintersections_helper<Triangle>(fst_after_base, trgs_last);

	return nintersections_helper<Triangle>	(borders[0], borders[1]) // among left
		+ nintersections_helper<Triangle>	(borders[1], trgs_last) // among right
		+ nintersections_helper<Triangle>	(trgs_fst, borders[0]) // among middle
		+ ncrossintersections_helper<Triangle, Triangle> (trgs_fst, borders[0], borders[0], borders[1]) // middle - left
		+ ncrossintersections_helper<Triangle, Triangle> (trgs_fst, borders[0], borders[1], trgs_last); // middle - right
}

template <>
int ncrossintersections_helper<Triangle, Triangle>(
	references_vector_iterator_t<Triangle> a_fst,
	references_vector_iterator_t<Triangle> a_last,
	references_vector_iterator_t<Triangle> b_fst,
	references_vector_iterator_t<Triangle> b_last
	)
{
	if (a_last - a_fst <= 1 || b_last - b_fst <= 1)
		return ncrossintersections_helper_generic<Triangle, Triangle> (a_fst, a_last, b_fst, b_last);
	
	const Triangle& base = *a_fst;
	auto fst_after_base = std::next(a_fst);

	auto middle_halfplane_detector = [base](const Triangle& trg)
		{ return halfplane_detector(base, trg) == 0; };
	auto left_halfplane_detector = [base](const Triangle& trg)
		{ return halfplane_detector(base, trg) < 0; };

	auto a_borders = other::split(fst_after_base, a_last,
		middle_halfplane_detector, left_halfplane_detector);
	auto b_borders = other::split(b_fst, b_last,
		middle_halfplane_detector, left_halfplane_detector);

	/* a_fst 	 	-- a_borders[0]	-	a_middle
	 * a_borders[0]	-- a_borders[1]	-	a_left
	 * a_borders[1]	-- a_last		-	a_right */

	/* b_fst  		-- b_borders[0]	-	b_middle
	 * b_borders[0]	-- b_borders[1]	-	b_left
	 * b_borders[1]	-- b_last		-	b_right */

	if ((b_borders[0] == b_borders[1] || a_borders[1] == a_last)
		&& (b_borders[1] == b_last || a_borders[0] == a_borders[1])) {
		/* Reducing a_group by one triangle */
		return ncrossintersections_helper<Triangle, Triangle> (a_fst, fst_after_base, b_fst, b_last)
			+ ncrossintersections_helper<Triangle, Triangle> (fst_after_base, a_last, b_fst, b_last);
	}

	/* We don't need to intersect a_left and b_right groups.
	 * Also we don't intersect a_right and b_left */
	/* Order is important!!! */
	int n1 = ncrossintersections_helper<Triangle, Triangle> (b_borders[1], b_last, a_borders[1], a_last); // b_right and a_right
	int n2 = ncrossintersections_helper<Triangle, Triangle> (b_borders[1], b_last, a_fst, a_borders[0]); // b_right and a_middle
	int n3 = ncrossintersections_helper<Triangle, Triangle> (b_borders[0], b_borders[1], a_fst, a_borders[1]); // b_left and (a_middle + a_left)
	int n4 = ncrossintersections_helper<Triangle, Triangle> (b_fst, b_borders[0], a_fst, a_last); // b_middle and full a

	return n1 + n2 + n3 + n4;
}

} // Geometry namespace end