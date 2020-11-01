#ifndef GEOMETRY_NINTERSECTIONS_H_
#define GEOMETRY_NINTERSECTIONS_H_

#include "other.h"

namespace Geometry {

/* Generic algorithm for number of intersections (see nintersections())
 * Complexity: O(n^2) */
template <class InputIt>
int nintersections_helper(InputIt figure_fst, InputIt figure_last, ...)
{
	int counter = 0;
	for (auto it1 = figure_fst; it1 != figure_last; ++it1)
		for (auto it2 = std::next(it1); it2 != figure_last; ++it2)
			if (intersected(*it1, *it2))
				++counter;
	return counter;
}

/*  returns 0 - if trg has common points with plane base
 *  returns 1 - if all of trg vertices are in a halfplane, which
 * is pointed by base normal
 *  returns -1 - if all of trg vertices are in a halfplane,
 * which is opposit to the one pointed by base plane normal */
inline int halfplane_detector(const Triangle& base, const Triangle& trg)
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


/*  Algorithm for counting number of intersections between
 * triangles. More effective than generic algorithm (see nintersections())
 *  Complexity: O(nlog(n)) в среднем, O(n^2) в худшем случае */
template <class InputIt,
	typename std::enable_if<
		std::is_same<
			typename std::iterator_traits<InputIt>::value_type,
			Triangle>::value,
		int>::type = 0
>
int nintersections_helper(InputIt trgs_fst, InputIt trgs_last, int)
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
	/* trgs_fst   -- borders[0]	-	both halfplanes
	 * borders[0] -- borders[1]	-	left halfplane
	 * borders[1] -- trgs_last	-	right halfplane */

	/*  If there is no triangles, which are completely left or completely right,
	 * we need to do smth, or there will be infinite recursion. So, we decrease
	 * number of triangles by one and try again. In the worst case complexity is n^2 */
	if (borders[0] == trgs_last)
		return ncrossintersections(trgs_fst, fst_after_base, fst_after_base, trgs_last)
			+ nintersections(fst_after_base, trgs_last);

	return nintersections(borders[0], borders[1]) // among left
		+ nintersections(borders[1], trgs_last) // among right
		+ nintersections(trgs_fst, borders[0]) // among middle
		+ ncrossintersections(trgs_fst, borders[0], borders[0], borders[1]) // middle - left
		+ ncrossintersections(trgs_fst, borders[0], borders[1], trgs_last); // middle - right
}



/*  Generic algorithm for counting number of intersections between
 * groups of geometric objects (see ncrossintersections())
 *  Complexity: O(n*m) */
template <class InputIt1, class InputIt2>
int ncrossintersections_helper(InputIt1 a_fst, InputIt1 a_last,
	InputIt2 b_fst, InputIt2 b_last, ...)
{
	int counter = 0;
	for(auto it1 = a_fst; it1 != a_last; ++it1)
		for (auto it2 = b_fst; it2 != b_last; ++it2)
			if (intersected(*it1, *it2))
				++counter;
	return counter;
}


} // Geometry namespace end

#endif // GEOMETRY_NINTERSECTIONS_H_