#ifndef GEOMETRY_NINTERSECTIONS_H_
#define GEOMETRY_NINTERSECTIONS_H_

#include "other.h"

namespace Geometry {

/* Обобщенный алгоритм расчета числа пересечений (См. nintersections())
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

/* Возвращает 0, если which имеет общие точки с плоскостью base,
 * 1 - если все точки лежат в полуплоскости, в которую указывает
 * нормаль плоскости base,
 * -1 - */
inline int halfplane_detector(const Triangle& base, const Triangle& which)
{
	Plane plane(base);
	Vector n = plane.normal();
	Float a = Vector::inner_product(n, vdistance(which.a, plane));
	Float b = Vector::inner_product(n, vdistance(which.b, plane));
	Float c = Vector::inner_product(n, vdistance(which.c, plane));
	if (a > 0 && b > 0 && c > 0)
		return -1;
	if (a < 0 && b < 0 && c < 0)
		return 1;
	return 0;
}


/* Подсчет числа пересечений треугольников (См. nintersections())
 * Complexity: O(nlog(n)) в среднем, O(n^2) в худшем случае */
template <class InputIt,
	typename std::enable_if<
		std::is_same<
			typename std::iterator_traits<InputIt>::value_type,
			Triangle>::value,
		int>::type = 0
>
int nintersections_helper(InputIt trgs_fst, InputIt trgs_last, int)
{
	// std::cout << ">::: nintersections(trgs[]) :\n";
	// for(auto it = trgs_fst; it != trgs_last; ++it)
	// 	std::cout << ">::: -- " << *it << std::endl;
	// std::cout << std::endl;

	if (trgs_fst == trgs_last || std::next(trgs_fst) == trgs_last) // Нужно хотя бы 2 треугольника
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

	/*  Если не нашлось треугольников, лежащих целиком слева или справа,
	 * придется что-то делать самому. Уменьшим число треугольников на 1
	 * и попробуем снова. В худшем случае будет n^2 */
	if (borders[0] == trgs_last)
		return ncrossintersections(trgs_fst, fst_after_base, fst_after_base, trgs_last)
			+ nintersections(fst_after_base, trgs_last);

	return nintersections(borders[0], borders[1]) // среди левых
		+ nintersections(borders[1], trgs_last) // среди правых
		+ nintersections(trgs_fst, borders[0]) // среди средних
		+ ncrossintersections(trgs_fst, borders[0], borders[0], borders[1]) // средние - левые
		+ ncrossintersections(trgs_fst, borders[0], borders[1], trgs_last); // средние - правые
}



/* Обобщенный алгоритм расчета числа пересечений между группами
 * (См. ncrossintersections())
 * Complexity: O(n*m) */
template <class InputIt1, class InputIt2>
int ncrossintersections_helper(InputIt1 a_fst, InputIt1 a_last,
	InputIt2 b_fst, InputIt2 b_last, ...)
{
	// std::cout << ">::: ncrossintersections(trgs[], trgs[]) :\n";
	// for(auto it = a_fst; it != a_last; ++it)
	// 	std::cout << ">::: -- " << *it << std::endl;
	// printf(">::: --- and ---\n");
	// for(auto it = b_fst; it != b_last; ++it)
	// 	std::cout << ">::: -- " << *it << std::endl;

	int counter = 0;
	for(auto it1 = a_fst; it1 != a_last; ++it1)
		for (auto it2 = b_fst; it2 != b_last; ++it2)
			if (intersected(*it1, *it2))
				++counter;
	// std::cout << ">::: returning " << counter << std::endl;
	// std::cout << std::endl;
	return counter;
}


} // Geometry namespace end

#endif // GEOMETRY_NINTERSECTIONS_H_