/* Intersections of groups of geometric objects (realization of non-templates) */

#include "geometry.h"

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

/*  Algorithm for counting number of intersections between
 * triangles. More effective than generic algorithm (see nintersections())
 *  Complexity: O(nlog(n)) on average, O(n^2) in the worst case */
template <>
int nintersections_helper<Triangle>(
	references_vector_iterator_t<Triangle> trgs_fst,
	references_vector_iterator_t<Triangle> trgs_last)
{
	if (trgs_fst == trgs_last || std::next(trgs_fst) == trgs_last) // 2 triangles minimum needed
		return 0;

	// TODO: choose_base_trg(trgs_fst, trgs_last);
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

} // Geometry namespace end