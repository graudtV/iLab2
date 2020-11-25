#ifndef GEOMETRY_IMPL_H_
#define GEOMETRY_IMPL_H_

namespace Geometry {

using std::experimental::is_detected_v;

bool intersected_impl(const Triangle& fst, const Triangle& snd);

template <class Figure1, class Figure2>
using intersected_impl_t = decltype(intersected_impl(
	std::declval<Figure1>(), std::declval<Figure2>()));

template <class Figure1, class Figure2>
using intersection_t = decltype(intersection(
	std::declval<Figure1>(), std::declval<Figure2>()));

template <class Figure1, class Figure2>
bool intersected(const Figure1& fst, const Figure2& snd)
{
	static_assert( is_detected_v<intersected_impl_t, Figure1, Figure2>
		|| is_detected_v<intersection_t, Figure1, Figure2>,
		"intersected() is not defined for such figures."
		" Provide function intersected_impl(figure1, figure2)"
		" or function intersection(figure1, figure2)."
		" Note. Don't redefine intersected() itself, it works"
		" badly with inheritance");	
	if constexpr (is_detected_v<intersected_impl_t, Figure1, Figure2>)
		return intersected_impl(fst, snd);
	else
		return !std::holds_alternative<EmptySet>(intersection(fst, snd));
	
}


} // Geometry namespace end

#endif // GEOMETRY_IMPL_H_