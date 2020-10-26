#ifndef OTHER_H_
#define OTHER_H_

namespace other {

/*  Переставляет элементы в контейнере так, что "слева" (т.е. от
 * итератора fst до итератора, возвращаемого функцией)
 * оказываются элементы, удовлетворяющие предикату,
 * а "справа" (т.е. от возвращаемого итератора до итератора last) - 
 * не удовлетворяющие */
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

/*  Работает аналогично split(BidirIt, BidirIt, Predicate),
 * но разбивает контейнер на 3 части: все элементы, удовлетворяющие
 * in_fst_part, попадут в первую часть. Элементы, не удовлетворяющие
 * in_fst_part, но удовлетворяющие in_snd_part, попадут во вторую
 * часть. Все остальный элементы будут в конце. (т.е. in_fst_part
 * и in_snd_part не обязаны быть взаимоисключающими)
 *  Первый элемент возвращаемоего массива - конец первой части (начало второй),
 * второй элемент - конец второй части (начало третьей)
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