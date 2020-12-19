#include "matrix.h"
#include <numeric>

namespace Maths {

void Permutation::reset(size_t nvalues)
{
	m_numbers.resize(nvalues);
	std::iota(m_numbers.begin(), m_numbers.end(), 0);
}

} // Maths namespace end