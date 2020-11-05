#ifndef OTHER_H_
#define OTHER_H_

#include <array>

namespace other {

/*  Swaps elements in container in such way that on the "left side"
 * of the container (i.e. in range from iterator fst to the iterator,
 * which is returned from the function) appear elements, which satisfy
 * predicate, and on the right (i.e. in range from returned iterator
 * to iterator last) - which don't satisfy it
*/
template <class BidirIt, class Predicate>
BidirIt split(BidirIt fst, BidirIt last, Predicate pred)
{ 
	if (fst == last)
		return last;
	--last;
	while (fst != last) {
		if (pred(*fst))
			++fst;
		else {
			std::swap(*fst, *last);
			--last;
		}
	}
	return (pred(*fst)) ? ++fst : fst; // fst и last сейчас равны
}

/*  Analogous to split(BidirIt, BidirIt, Predicate),
 * but splits the container into 3 parts: All elements, which
 * satisfy predicate in_fst_part, will get into the first part.
 * All elements, which don't satisfy in_fst_part, but which satisfy
 * in_snd_part, will get into second part. All other elemets will
 * be in the third part.
 *  The first iterator in the returned array is the end of the first
 * part (i.e. begin of the second part). The second iterator in the
 * returned array is the end of the second part (i.e. begin of the third part)
 *  Note. in_fst_part and in_snd_part aren't necessary mutual exclusive
*/
template <class BidirIt, class Predicate1, class Predicate2>
std::array<BidirIt, 2>
split(BidirIt fst, BidirIt last, Predicate1 in_fst_part, Predicate2 in_snd_part)
{
	auto snd_part_begin = split(fst, last, in_fst_part);
	auto thd_part_begin = split(snd_part_begin, last, in_snd_part);
	return { snd_part_begin, thd_part_begin };
}

} // other namespace end

#endif // OTHER_H_