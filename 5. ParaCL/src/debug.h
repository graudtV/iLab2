#pragma once

#include <type_traits>

template <class T, class P>
T save_ptr_cast(P *p)
{
	auto res = dynamic_cast<T>(p);
	if (!res)
		throw std::runtime_error("bad_cast: " + std::string(__PRETTY_FUNCTION__));
	return res;
}

#ifdef NDEBUG
# define cast static_cast
#else
# define cast save_ptr_cast
#endif