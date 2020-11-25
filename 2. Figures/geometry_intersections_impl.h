/*
 *  geometry_intersections_impl.h - implementation of template fuctions
 * for intersecting groups of geometric objects
 */

#ifndef GEOMETRY_INTERSECTIONS_IMPL_H_
#define GEOMETRY_INTERSECTIONS_IMPL_H_

#include "other.h"
#include <iterator>
#include <functional>
#include <vector>
#include <algorithm>

namespace Geometry {


/******* prototypes and declarations for local usage *******/


/*  Warning: don't call helper functions from client code,
 * because they don't check if figures are valid.
 * Use functions like nintersections(), ncrossintersections() */

/*  Helper-functions below use vectors of wrapped references on Figure in order
 * not to change the intial container (effective algorithms need to change
 * figures order) and to have random access iterator advantages,
 * even if user is trying to calculate intersections in std::map or smth like that */
template <class Figure>
using references_vector_iterator_t
	= typename std::vector<std::reference_wrapper<Figure>>::iterator;

template <class Figure>
struct FigureAndIndex {
	std::reference_wrapper<const Figure> figure;
	int idx;

	FigureAndIndex(const Figure& f, int i) : figure(f), idx(i) {}
	operator const Figure& () { return figure; }
};

template <class Figure>
using figure_and_index_vector_iterator_t
	= typename std::vector<FigureAndIndex<Figure>>::iterator;


/*----- nintersections -----*/

template <class Figure>
int nintersections_helper_generic(
	references_vector_iterator_t<Figure> figure_fst,
	references_vector_iterator_t<Figure> figure_last);

template <class Figure>
int nintersections_helper(
	references_vector_iterator_t<Figure> figure_fst,
	references_vector_iterator_t<Figure> figure_last)
{ return nintersections_helper_generic<Figure>(figure_fst, figure_last); }

template <>
int nintersections_helper<Triangle>(
	references_vector_iterator_t<Triangle> trgs_fst,
	references_vector_iterator_t<Triangle> trgs_last);


/*----- ncrossintersections -----*/

template <class Figure1, class Figure2>
int ncrossintersections_helper_generic(
	references_vector_iterator_t<Figure1> a_fst,
	references_vector_iterator_t<Figure1> a_last,
	references_vector_iterator_t<Figure2> b_fst,
	references_vector_iterator_t<Figure2> b_last
	);

template <class Figure1, class Figure2>
int ncrossintersections_helper(
	references_vector_iterator_t<Figure1> a_fst,
	references_vector_iterator_t<Figure1> a_last,
	references_vector_iterator_t<Figure2> b_fst,
	references_vector_iterator_t<Figure2> b_last)
{ return ncrossintersections_helper_generic<Figure1, Figure2>(a_fst, a_last, b_fst, b_last); }

template <>
int ncrossintersections_helper<Triangle, Triangle>(
	references_vector_iterator_t<Triangle> a_fst,
	references_vector_iterator_t<Triangle> a_last,
	references_vector_iterator_t<Triangle> b_fst,
	references_vector_iterator_t<Triangle> b_last);


/*----- build_intersections_table -----*/

template <class Figure>
void build_intersections_table_helper_generic(
	figure_and_index_vector_iterator_t<Figure> figure_fst,
	figure_and_index_vector_iterator_t<Figure> figure_last,
	IntersectionsTable& intrsctns_table);

template <class Figure>
void build_intersections_table_helper(
	figure_and_index_vector_iterator_t<Figure> figure_fst,
	figure_and_index_vector_iterator_t<Figure> figure_last,
	IntersectionsTable& intrsctns_table)
{ build_intersections_table_helper_generic<Figure>(figure_fst, figure_last, intrsctns_table); }

template <>
void build_intersections_table_helper<Triangle>(
	figure_and_index_vector_iterator_t<Triangle> trgs_fst,
	figure_and_index_vector_iterator_t<Triangle> trgs_last,
	IntersectionsTable& intrsctns_table);


/*----- build_crossintersections_table -----*/

template <class Figure1, class Figure2>
void build_crossintersections_table_helper_generic(
	figure_and_index_vector_iterator_t<Figure1> a_fst,
	figure_and_index_vector_iterator_t<Figure1> a_last,
	figure_and_index_vector_iterator_t<Figure2> b_fst,
	figure_and_index_vector_iterator_t<Figure2> b_last,
	IntersectionsTable& intrsctns_table);


/*----- other prototypes -----*/

template <class Figure>
void erase_not_valid_figures(std::vector<std::reference_wrapper<Figure>>& figures);

template <class Figure>
void erase_not_valid_figures(std::vector<FigureAndIndex<Figure>>& figures);

template <class Figure, class InputIt>
void init_figure_and_index_vec(
	std::vector<FigureAndIndex<Figure>>& figures,
	InputIt figure_fst,
	InputIt figure_last);




/******* realizations of "interface" functions *******/

template <class InputIt>
int nintersections(InputIt figure_fst, InputIt figure_last)
{
	using Figure = typename std::iterator_traits<InputIt>::value_type;

	std::vector<std::reference_wrapper<Figure>> figures(figure_fst, figure_last);
	erase_not_valid_figures(figures);

	return nintersections_helper<Figure>(figures.begin(), figures.end());
}

template <class InputIt>
int nintersections_benchmark(InputIt figure_fst, InputIt figure_last)
{
	using Figure = typename std::iterator_traits<InputIt>::value_type;

	std::vector<std::reference_wrapper<Figure>> figures(figure_fst, figure_last);
	erase_not_valid_figures(figures);

	return nintersections_helper_generic<Figure>(figures.begin(), figures.end());
}

template <class InputIt1, class InputIt2>
int ncrossintersections(InputIt1 a_fst, InputIt1 a_last,
	InputIt2 b_fst, InputIt2 b_last)
{
	using Figure1 = typename std::iterator_traits<InputIt1>::value_type;
	using Figure2 = typename std::iterator_traits<InputIt2>::value_type;

	std::vector<std::reference_wrapper<Figure1>> figures_a(a_fst, a_last);
	std::vector<std::reference_wrapper<Figure2>> figures_b(b_fst, b_last);
	erase_not_valid_figures(figures_a);
	erase_not_valid_figures(figures_b);

	return ncrossintersections_helper<Figure1, Figure2>(figures_a.begin(), figures_a.end(),
		figures_b.begin(), figures_b.end());
}

template <class InputIt>
IntersectionsTable
build_intersections_table(InputIt figure_fst, InputIt figure_last)
{
	using Figure = typename std::iterator_traits<InputIt>::value_type;
	
	std::vector<FigureAndIndex<Figure>> figures;
	init_figure_and_index_vec(figures, figure_fst, figure_last);
	erase_not_valid_figures(figures);

	IntersectionsTable intrsctns_table;
	build_intersections_table_helper<Figure>(
		figures.begin(), figures.end(), intrsctns_table);
	return intrsctns_table;
}

template <class InputIt>
IntersectionsTable
build_intersections_table_benchmark(InputIt figure_fst, InputIt figure_last)
{
	using Figure = typename std::iterator_traits<InputIt>::value_type;
	
	std::vector<FigureAndIndex<Figure>> figures;
	init_figure_and_index_vec(figures, figure_fst, figure_last);
	erase_not_valid_figures(figures);

	IntersectionsTable intrsctns_table;
	build_intersections_table_helper_generic<Figure>(
		figures.begin(), figures.end(), intrsctns_table);
	return intrsctns_table;
}




/******* realization of helper functions *******/

/*  Generic algorithm for number of intersections (see nintersections())
 *  Sometimes it is usefull to use exactly generic algorithm, so
 * it is separated from nintersections_helper()
 *  Complexity: O(n^2) */
template <class Figure>
int nintersections_helper_generic(
	references_vector_iterator_t<Figure> figure_fst,
	references_vector_iterator_t<Figure> figure_last)
{
	int counter = 0;
	for (auto it1 = figure_fst; it1 != figure_last; ++it1)
		for (auto it2 = std::next(it1); it2 != figure_last; ++it2)
			if (intersected(it1->get(), it2->get()))
				++counter;
	return counter;
}

/*  Generic algorithm for counting number of intersections between
 * groups of geometric objects (see ncrossintersections())
 *  Sometimes it is usefull to use exactly generic algorithm, so
 * it is separated from ncrossintersections_helper()
 *  Complexity: O(n*m) */
template <class Figure1, class Figure2>
int ncrossintersections_helper_generic(
	references_vector_iterator_t<Figure1> a_fst,
	references_vector_iterator_t<Figure1> a_last,
	references_vector_iterator_t<Figure2> b_fst,
	references_vector_iterator_t<Figure2> b_last)
{
	int counter = 0;
	for(auto it1 = a_fst; it1 != a_last; ++it1)
		for (auto it2 = b_fst; it2 != b_last; ++it2)
			if (intersected(it1->get(), it2->get()))
				++counter;
	return counter;	
}

/*  Generic algorithm for building intersections table
 * (see build_intersections_table())
 *  Sometimes it is usefull to use exactly generic algorithm, so
 * it is separated from build_intersections_table_helper()
 *  Complexity: O(n^2) */
template <class Figure>
void build_intersections_table_helper_generic(
	figure_and_index_vector_iterator_t<Figure> figure_fst,
	figure_and_index_vector_iterator_t<Figure> figure_last,
	IntersectionsTable& intrsctns_table)
{
	for (auto it1 = figure_fst; it1 != figure_last; ++it1)
		for (auto it2 = std::next(it1); it2 != figure_last; ++it2)
			if (intersected(it1->figure.get(), it2->figure.get()))
				intrsctns_table.push_back({it1->idx, it2->idx});
};

/*  Generic algorithm for building cross-intersections table
 * (see build_crossintersections_table())
 *  Sometimes it is usefull to use exactly generic algorithm, so
 * it is separated from build_crossintersections_table_helper()
 *  Complexity: O(n*m) */
template <class Figure1, class Figure2>
void build_crossintersections_table_helper_generic(
	figure_and_index_vector_iterator_t<Figure1> a_fst,
	figure_and_index_vector_iterator_t<Figure1> a_last,
	figure_and_index_vector_iterator_t<Figure2> b_fst,
	figure_and_index_vector_iterator_t<Figure2> b_last,
	IntersectionsTable& intrsctns_table)
{
	for(auto it1 = a_fst; it1 != a_last; ++it1)
		for (auto it2 = b_fst; it2 != b_last; ++it2)
			if (intersected(it1->figure.get(), it2->figure.get()))
				intrsctns_table.push_back({it1->idx, it2->idx});
}

/* Doesn't change underlying container */
template <class Figure>
void erase_not_valid_figures(std::vector<std::reference_wrapper<Figure>>& figures)
{
	auto is_not_valid = [](const Figure& f) { return !f.valid(); };
	figures.erase(
		std::remove_if(figures.begin(), figures.end(), is_not_valid),
		figures.end()
		);
}

template <class Figure>
void erase_not_valid_figures(std::vector<FigureAndIndex<Figure>>& figures)
{
	auto is_not_valid = [](const FigureAndIndex<Figure>& figure_and_index)
		{ return !figure_and_index.figure.get().valid(); };

	figures.erase(
		std::remove_if(figures.begin(), figures.end(), is_not_valid),
		figures.end()
		);
}

template <class Figure, class InputIt>
void init_figure_and_index_vec(
	std::vector<FigureAndIndex<Figure>>& figures,
	InputIt figure_fst,
	InputIt figure_last)
{
	int i = 0;
	for (auto it = figure_fst; it != figure_last; ++it)
		figures.push_back({*it, i++});
}

} // Geometry namespace end

#endif // GEOMETRY_INTERSECTIONS_IMPL_H_