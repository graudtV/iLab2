namespace Geometry {

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

template <class InputIt,
	typename std::enable_if<
		std::is_same<
			typename std::iterator_traits<InputIt>::value_type,
			Triangle>::value,
		int>::type
>
int nintersections_helper(InputIt trgs_fst, InputIt trgs_last, int)
{
	throw std::runtime_error("not implemented");
	return 0;
}


} // Geometry namespace end