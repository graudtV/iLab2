#ifndef GEOMETRY_NINTERSECTIONS_H_
#define GEOMETRY_NINTERSECTIONS_H_

#include "other.h"
#include <iterator>
#include <functional>
#include <vector>
#include <algorithm>

namespace Geometry {

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

/* Generic algorithm for number of intersections (see nintersections())
 * Complexity: O(n^2) */
template <class Figure>
int nintersections_helper(
	references_vector_iterator_t<Figure> figure_fst,
	references_vector_iterator_t<Figure> figure_last)
{ return nintersections_helper_benchmark(figure_fst, figure_last); }

template <class Figure>
int nintersections_helper_benchmark(
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

/* Quick algorithm for triangles */
template <>
int nintersections_helper<Triangle>(
	references_vector_iterator_t<Triangle> trgs_fst,
	references_vector_iterator_t<Triangle> trgs_last
	);

/*  Generic algorithm for counting number of intersections between
 * groups of geometric objects (see ncrossintersections())
 *  Complexity: O(n*m) */
template <class Figure1, class Figure2>
int ncrossintersections_helper(
	references_vector_iterator_t<Figure1> a_fst,
	references_vector_iterator_t<Figure1> a_last,
	references_vector_iterator_t<Figure2> b_fst,
	references_vector_iterator_t<Figure2> b_last
	)
{
	int counter = 0;
	for(auto it1 = a_fst; it1 != a_last; ++it1)
		for (auto it2 = b_fst; it2 != b_last; ++it2)
			if (intersected(it1->get(), it2->get()))
				++counter;
	return counter;	
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

	return nintersections_helper_benchmark<Figure>(figures.begin(), figures.end());
}

template <class InputIt1, class InputIt2>
int ncrossintersections(InputIt1 a_fst, InputIt1 a_last,
	InputIt2 b_fst, InputIt2 b_last)
{
	using Figure1 = typename std::iterator_traits<InputIt1>::value_type;
	using Figure2 = typename std::iterator_traits<InputIt1>::value_type;

	std::vector<std::reference_wrapper<Figure1>> figures_a(a_fst, a_last);
	std::vector<std::reference_wrapper<Figure2>> figures_b(b_fst, b_last);
	erase_not_valid_figures(figures_a);
	erase_not_valid_figures(figures_b);

	return ncrossintersections_helper(figures_a.begin(), figures_a.end(),
		figures_b.begin(), figures_b.end());
}


} // Geometry namespace end

#endif // GEOMETRY_NINTERSECTIONS_H_