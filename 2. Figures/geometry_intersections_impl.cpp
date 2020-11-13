/*
 *  geometry_intersections_impl.cpp - implementation of non-template
 * functions for intersecting groups of geometric objects
 */

#include "geometry.h"
#include <random>
#include <cassert>

namespace Geometry {
namespace {

/*  Similar to halfplane_detector(Plane, Triangle), but
 * takes plane by a point on it and normal. It could be more
 * efficient, if normal is already known */
int halfplane_detector(const Point& plane_origin,
	const Vector& plane_normal, const Triangle& trg)
{
	Float a = Vector::inner_product(Vector(trg.a, plane_origin), plane_normal);
	Float b = Vector::inner_product(Vector(trg.b, plane_origin), plane_normal);
	Float c = Vector::inner_product(Vector(trg.c, plane_origin), plane_normal);

	if (a > 0 && b > 0 && c > 0)
		return -1;
	if (a < 0 && b < 0 && c < 0)
		return 1;
	return 0;	
}

/*  returns 0 - if trg has common points with plane base
 *  returns 1 - if all of trg vertices are in a halfplane, which
 * is pointed by base normal
 *  returns -1 - if all of trg vertices are in a halfplane,
 * which is opposit to the one pointed by base plane normal */
int halfplane_detector(const Plane& base, const Triangle& trg)
	{ return halfplane_detector(base.a, base.normal(), trg); }

} // anonymous namespace end

template <class RandomIt>
void random_base_choise(RandomIt fst, RandomIt last)
	{ std::swap(fst[0], fst[rand() % (last - fst)]); }

template <class RandomIt>
inline auto split_triangles(const Triangle& base,
	RandomIt trgs_fst,
	RandomIt trgs_last)
{
	const Point origin = base.a; // base plane origin
	const Vector normal = Plane(base).normal(); // base plane normal

	return other::split(trgs_fst, trgs_last,
		[origin, normal](const Triangle& trg) { return halfplane_detector(origin, normal, trg) == 0; }, // both halfplanes
		[origin, normal](const Triangle& trg) { return halfplane_detector(origin, normal, trg) < 0; } // left halfplane
		);
}

/*  Algorithm for counting number of intersections between
 * triangles. More effective than generic algorithm (see nintersections())
 *  Complexity: O(nlog(n)^2 (probably, I am not s)) on average, O(n^2) in the worst case */
template <>
int nintersections_helper<Triangle>(
	references_vector_iterator_t<Triangle> trgs_fst,
	references_vector_iterator_t<Triangle> trgs_last)
{
	if (trgs_fst == trgs_last || std::next(trgs_fst) == trgs_last) // 2 triangles minimum needed
		return 0;

	random_base_choise(trgs_fst, trgs_last);
	auto fst_after_base = std::next(trgs_fst);
	auto borders = split_triangles(*trgs_fst, fst_after_base, trgs_last);

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

	return nintersections_helper<Triangle>  (borders[0], borders[1]) // among left
		+ nintersections_helper<Triangle>	(borders[1], trgs_last) // among right
		+ nintersections_helper<Triangle>	(trgs_fst, borders[0]) // among middle
		+ ncrossintersections_helper<Triangle, Triangle>(borders[0], borders[1], trgs_fst, borders[0]) // left - middle
		+ ncrossintersections_helper<Triangle, Triangle> (borders[1], trgs_last,  trgs_fst, borders[0]); // right - middle
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
		return ncrossintersections_helper_generic<Triangle, Triangle>(
			a_fst, a_last, b_fst, b_last);
	
	const Triangle& base = *a_fst;
	auto fst_after_base = std::next(a_fst);
	auto a_borders = split_triangles(base, fst_after_base, a_last);
	auto b_borders = split_triangles(base, b_fst, b_last);

	/* a_fst 	 	-- a_borders[0]	-	a_middle
	 * a_borders[0]	-- a_borders[1]	-	a_left
	 * a_borders[1]	-- a_last		-	a_right */

	/* b_fst  		-- b_borders[0]	-	b_middle
	 * b_borders[0]	-- b_borders[1]	-	b_left
	 * b_borders[1]	-- b_last		-	b_right */

	if ((b_borders[0] == b_borders[1] || a_borders[1] == a_last)
		&& (b_borders[1] == b_last || a_borders[0] == a_borders[1])) {
		/* Reducing a_group by one triangle */
		return ncrossintersections_helper<Triangle, Triangle>(
				a_fst, fst_after_base, b_fst, b_last)
			+ ncrossintersections_helper<Triangle, Triangle>(
				fst_after_base, a_last, b_fst, b_last);
	}

	/* We don't need to intersect a_left and b_right groups.
	 * Also we don't intersect a_right and b_left */
	/* Order is important!!! */
	int n = ncrossintersections_helper<Triangle, Triangle>(
		b_borders[1], b_last, a_borders[1], a_last); // b_right and a_right
	n += ncrossintersections_helper<Triangle, Triangle>(
		b_borders[1], b_last, a_fst, a_borders[0]); // b_right and a_middle
	n += ncrossintersections_helper<Triangle, Triangle>(
		b_borders[0], b_borders[1], a_fst, a_borders[1]); // b_left and (a_middle + a_left)
	n += ncrossintersections_helper<Triangle, Triangle>(
		b_fst, b_borders[0], a_fst, a_last); // b_middle and full a

	return n;
}

void build_crossintersections_table_helper(
	figure_and_index_vector_iterator_t<Triangle> a_fst,
	figure_and_index_vector_iterator_t<Triangle> a_last,
	figure_and_index_vector_iterator_t<Triangle> b_fst,
	figure_and_index_vector_iterator_t<Triangle> b_last,
	IntersectionsTable& intrsctns_table);

template <>
void build_intersections_table_helper<Triangle>(
	figure_and_index_vector_iterator_t<Triangle> trgs_fst,
	figure_and_index_vector_iterator_t<Triangle> trgs_last,
	IntersectionsTable& intrsctns_table)
{
	/* Algorithm is mostly copied from nintersections_helper<Triangle>() */

	if (trgs_fst == trgs_last || std::next(trgs_fst) == trgs_last) // 2 triangles minimum needed
		return;

	random_base_choise(trgs_fst, trgs_last);
	auto fst_after_base = std::next(trgs_fst);
	auto borders = split_triangles(*trgs_fst, fst_after_base, trgs_last);

	/* trgs_fst   -- borders[0]	-	both halfplanes - "middle"
	 * borders[0] -- borders[1]	-	left halfplane
	 * borders[1] -- trgs_last	-	right halfplane */

	/*  If there is no triangles, which are completely left or completely right,
	 * we need to do smth, or there will be infinite recursion. So, we decrease
	 * number of triangles by one and try again. In the worst case complexity is n^2 */
	if (borders[0] == trgs_last) {
		build_crossintersections_table_helper(
			trgs_fst, fst_after_base, fst_after_base, trgs_last, intrsctns_table);
		build_intersections_table_helper<Triangle>(
			fst_after_base, trgs_last, intrsctns_table);
		return;
	}

	build_intersections_table_helper<Triangle>(
		borders[0], borders[1], intrsctns_table); // among left
	build_intersections_table_helper<Triangle>(
		borders[1], trgs_last, intrsctns_table); // among right
	build_intersections_table_helper<Triangle>(
		trgs_fst, borders[0], intrsctns_table); // among middle
	build_crossintersections_table_helper(
		borders[0], borders[1], trgs_fst, borders[0], intrsctns_table); // left - middle
	build_crossintersections_table_helper(
		borders[1], trgs_last,  trgs_fst, borders[0], intrsctns_table); // right - middle
}

/* Helper, because it doesn't check if figures are valid */
void build_crossintersections_table_helper(
	figure_and_index_vector_iterator_t<Triangle> a_fst,
	figure_and_index_vector_iterator_t<Triangle> a_last,
	figure_and_index_vector_iterator_t<Triangle> b_fst,
	figure_and_index_vector_iterator_t<Triangle> b_last,
	IntersectionsTable& intrsctns_table
	)
{
	/*  Algorithm is mostly copied from
	 * ncrossintersections_helper<Triangle, Triangle>() */

	if (a_last - a_fst <= 1 || b_last - b_fst <= 1)
		return build_crossintersections_table_helper_generic<Triangle, Triangle>(
			a_fst, a_last, b_fst, b_last, intrsctns_table);
	
	const Triangle& base = *a_fst;
	auto fst_after_base = std::next(a_fst);
	auto a_borders = split_triangles(base, fst_after_base, a_last);
	auto b_borders = split_triangles(base, b_fst, b_last);

	/* a_fst 	 	-- a_borders[0]	-	a_middle
	 * a_borders[0]	-- a_borders[1]	-	a_left
	 * a_borders[1]	-- a_last		-	a_right */

	/* b_fst  		-- b_borders[0]	-	b_middle
	 * b_borders[0]	-- b_borders[1]	-	b_left
	 * b_borders[1]	-- b_last		-	b_right */

	if ((b_borders[0] == b_borders[1] || a_borders[1] == a_last)
		&& (b_borders[1] == b_last || a_borders[0] == a_borders[1])) {
		/* Reducing a_group by one triangle */
		build_crossintersections_table_helper(
			a_fst, fst_after_base, b_fst, b_last, intrsctns_table);
		build_crossintersections_table_helper(
			fst_after_base, a_last, b_fst, b_last, intrsctns_table);
		return;
	}

	/* We don't need to intersect a_left and b_right groups.
	 * Also we don't intersect a_right and b_left */
	/* Order is important!!! */
	build_crossintersections_table_helper(
		b_borders[1], b_last, a_borders[1], a_last, intrsctns_table); // b_right and a_right
	build_crossintersections_table_helper(
		b_borders[1], b_last, a_fst, a_borders[0], intrsctns_table); // b_right and a_middle
	build_crossintersections_table_helper(
		b_borders[0], b_borders[1], a_fst, a_borders[1], intrsctns_table); // b_left and (a_middle + a_left)
	build_crossintersections_table_helper(
		b_fst, b_borders[0], a_fst, a_last, intrsctns_table); // b_middle and full a
	return;
}

} // Geometry namespace end