#ifndef MATHS_TYPE_CONVERSIONS_H_
#define MATHS_TYPE_CONVERSIONS_H_

#include <type_traits>
#include <cmath>

namespace Maths {

template <class T> struct is_algebraic_field : public std::false_type {};
template <> struct is_algebraic_field<float> : public std::true_type {};
template <> struct is_algebraic_field<double> : public std::true_type {};
template <> struct is_algebraic_field<long double> : public std::true_type {};

template <class T> inline constexpr bool is_algebraic_field_v = is_algebraic_field<T>::value;
template <class T> using promote_to_algebraic_field_t = std::conditional_t<is_algebraic_field_v<T>, T, double>;

template <class To, class From, class = void>
struct rounding_function {
	From operator()(const From& from) { return from; }
};

template <class To, class From>
struct rounding_function<To, From,
	std::enable_if_t<std::is_integral_v<To> && std::is_floating_point_v<From>>>
{
	From operator()(const From& from) { return std::round(from); }
};

/* static_cast<int>(double) and similar doesn't give
 * the nearest integral number */
template <class To, class From>
From round_to(const From& from)
	{ return rounding_function<To, From>()(from); }

} // Maths namespace end

#endif // MATHS_TYPE_CONVERSIONS_H_