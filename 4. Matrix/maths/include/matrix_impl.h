/* Implementation of Matrix methods */

#ifndef MATHS_MATRIX_IMPL_H_
#define MATHS_MATRIX_IMPL_H_

#include <cmath>
#include <iomanip>

namespace Maths {

template <class T>
Matrix<T>::Matrix(size_t rows, size_t columns, const T& value /* = T{} */) :
	Matrix(rows, columns, [&](size_t, size_t) { return value; } ) {}

/*  value_generator takes row and column indices (in such order)
 * and generate some value, which will be put into matrix
 *  value_generator must have defined behaviour for each row from 0 to rows-1
 * and each column from 0 to columns-1. */
template <class T> inline
Matrix<T>::Matrix(size_t rows, size_t columns, std::function<T(size_t, size_t)> value_generator) :
	MatrixBuf<T>(rows, columns)
{
	for (size_t i = 0; i < m_nrows; ++i)
		for (size_t j = 0; j < m_ncolumns; ++j) {
			details::construct(&m_data[i][j], value_generator(i, j));
			++m_nfilled_elems;
		}
}

/* If std::distance(fst, last) > rows * columns, extra values are ignored 
 * If std::distance(fst, last) < rows * columns, default values T{} are appended */
template <class T>
template <class InputIt>
Matrix<T>::Matrix(size_t rows, size_t columns, InputIt fst, InputIt last) :
	Matrix(rows, columns,
		[&](size_t, size_t) { return (fst == last) ? T{} : *fst++; } ) {}

template <class T>
Matrix<T>::Matrix(const Matrix& other) :
	Matrix(other.m_nrows, other.m_ncolumns,
		[&](size_t i, size_t j) { return other.m_data[i][j]; } ) {}

template <class T>
Matrix<T>& Matrix<T>::operator =(const Matrix& other)
{
	if (m_nrows == other.m_nrows && m_ncolumns == other.m_ncolumns)
		fill_with_values(
			[&](size_t i, size_t j) noexcept(noexcept(T(other.m_data[0][0])))
				{ return other.m_data[i][j]; }
			);
	return *this = Matrix(other);
}

template <class T>
template <class U>
Matrix<U> Matrix<T>::convert_to() const
{
	return Matrix<U>(m_nrows, m_ncolumns,
		[&](size_t i, size_t j) { return round_to<U>(m_data[i][j]); } );
}

template <class T>
Matrix<T> Matrix<T>::get_transposed() const
{
	return Matrix<T>(m_ncolumns, m_nrows,
		[this](size_t i, size_t j) { return m_data[j][i]; } );
}

template <class T>
template <class A /* = promoted_value_type */>
Matrix<A> Matrix<T>::get_inverted() const
{
	Matrix<A> C;
	Permutation P;

	if (LUP_decomposition_impl(C, P) == 0)
		throw InvertionError();

	Matrix<A> res(m_nrows, m_nrows); // m_nrows == m_ncolumns

    for (size_t j = 0; j < m_nrows; ++j) {
        for (size_t i = 0; i < m_nrows; ++i) {
            res[i][j] = (P[i] == j) ? 1 : 0;
            for (size_t k = 0; k < i; ++k)
                res[i][j] -= C[i][k] * res[k][j];
        }
        for (size_t i = m_nrows - 1;; --i) {
            for (size_t k = i + 1; k < m_nrows; ++k)
                res[i][j] -= C[i][k] * res[k][j];
            res[i][j] /= C[i][i];
            if (i == 0)
            	break;
        }
    }
	return res;
}

/* For not square matrices returns zero */
template <class T>
T Matrix<T>::determinant() const
{
	Matrix<promoted_value_type> C;
	Permutation P;
	int n = LUP_decomposition_impl(C, P);
	return round_to<T>(n * C.main_diagonal_elements_product());
}

/* For empty matrices returns zero */
template <class T>
T Matrix<T>::main_diagonal_elements_product() const
{
	if (empty())
		return T();
	T res = m_data[0][0];
	for (int i = 1; i < m_nrows && i < m_ncolumns; ++i)
		res *= m_data[i][i];
	return res;
}

template <class T>
void Matrix<T>::fill_with(const T& value)
{
	fill_with_values(
		[&](size_t i, size_t j) noexcept(noexcept(T(value)))
			{ return value; }
		);
}

template <class T>
template <class Func>
void Matrix<T>::fill_with_values(Func value_generator)
{
	/*  If values can be generated and assigned without exceptions,
	 * where is no need to make extra copies. But, if not, the only safe
	 * way is to make a new Matrix and then swap data */
	if constexpr (noexcept(m_data[0][0] = value_generator(0, 0))) {
		for (size_t i = 0; i < m_nrows; ++i)
			for (size_t j = 0; j < m_ncolumns; ++j)
				m_data[i][j] = value_generator(i, j);
	} else {
		*this = Matrix(m_nrows, m_ncolumns, value_generator);		
	}
}

template <class T>
void Matrix<T>::swap_rows(size_t i, Matrix& other, size_t j)
{
	if (m_ncolumns != other.m_ncolumns)
		throw std::invalid_argument("Matrix::swap_rows(): different number of columns");
	std::swap(m_data[i], other.m_data[j]);
}

/* Note. row_max, column_max will (!) be included in the result
 * 0 <= row_min <= row_max <= nrows() - 1
 * 0 <= column_min <= column_max <= ncolumns() - 1 */
template <class T>
Matrix<T> Matrix<T>::get_cut(size_t row_min, size_t row_max,
	size_t column_min, size_t column_max) const
{
	if (row_min > row_max || row_max >= m_nrows
		|| column_min > column_max || column_max >= m_ncolumns)
		throw std::invalid_argument("Matrix::cut(): range to be cut is invalid");
	return Matrix(row_max - row_min + 1, column_max - column_min + 1,
		[=](size_t i, size_t j) { return m_data[row_min + i][column_min + j]; });
}

template <class T>
bool operator ==(const Matrix<T>& fst, const Matrix<T>& snd)
{
	if (fst.nrows() != snd.nrows() || fst.ncolumns() != snd.ncolumns())
		return false;
	for (size_t i = 0; i < fst.nrows(); ++i)
		for (size_t j = 0; j < fst.ncolumns(); ++j)
			if (fst[i][j] != snd[i][j])
				return false;
	return true;
}

template <class T>
void Matrix<T>::dump(std::ostream& os /* = std::cout */, int field_width /* = 10 */) const
{
	os << std::left;
	for (size_t i = 0; i < m_nrows; ++i) {
		for (size_t j = 0; j < m_ncolumns - 1; ++j)
			os << std::setw(field_width) << m_data[i][j] << " ";
		if (m_ncolumns > 0)
			os << m_data[i][m_ncolumns - 1];
		os << std::endl;
	}
}

template <class T>
Matrix<T>& Matrix<T>::operator +=(const Matrix<T>& other) &
{
	fill_with_values(
		[&](size_t i, size_t j) noexcept(noexcept(T(m_data[0][0] + other.m_data[0][0])))
			{ return m_data[i][j] + other.m_data[i][j]; } );
	return *this;
}

template <class T>
Matrix<T>& Matrix<T>::operator -=(const Matrix<T>& other) &
{
	fill_with_values(
		[&](size_t i, size_t j) noexcept(noexcept(T(m_data[0][0] - other.m_data[0][0])))
	 		{ return m_data[i][j] - other.m_data[i][j]; } );
	return *this;
}

/* may change matrix size */
template <class T>
Matrix<T>& Matrix<T>::operator *=(const Matrix<T>& other) &
	{ return *this = *this * other; }

template <class T>
Matrix<T> operator +(const Matrix<T>& fst, const Matrix<T>& snd)
{
	if (fst.nrows() != snd.nrows() || fst.ncolumns() != snd.ncolumns())
		throw IncompatibleSizeError();
	return Matrix<T>(fst.nrows(), fst.ncolumns(), [&](size_t i, size_t j) { return fst[i][j] + snd[i][j]; } );
}

template <class T>
Matrix<T> operator -(const Matrix<T>& fst, const Matrix<T>& snd)
{
	if (fst.nrows() != snd.nrows() || fst.ncolumns() != snd.ncolumns())
		throw IncompatibleSizeError();
	return Matrix<T>(fst.nrows(), fst.ncolumns(), [&](size_t i, size_t j) { return fst[i][j] - snd[i][j]; } );
}

template <class T>
Matrix<T> operator *(const Matrix<T>& fst, const Matrix<T>& snd)
{
	if (fst.ncolumns() != snd.nrows())
		throw IncompatibleSizeError();
	Matrix<T> res(fst.nrows(), snd.ncolumns());
	for (size_t i = 0; i < res.nrows(); ++i)
		for (size_t j = 0; j < res.ncolumns(); ++j)
			for (size_t k = 0; k < fst.ncolumns(); ++k)
				res[i][j] += fst[i][k] * snd[k][j];
	return res;
}

/* throws DecompostionError if decomposition doesn't exist */
template <class T>
template <class A /* = promoted_value_type */>
LUPDecomposition<A>
Matrix<T>::LUP_decomposition() const
{
	Matrix<A> C;
	Permutation P;
	int n = LUP_decomposition_impl(C, P);
	if (n == 0) // decomposition failed
		throw DecompositionError();
	return LUPDecomposition(C, P);
}

template <class T>
inline T absval(const T& value)
	{ return (value >= 0) ? value : -value; }

/* C = L+U-E, there L, U are from decomposition: PA = LU
 * If C is known, it is easy to extract L and U 
 * Returns 0 if matrix is not singular. Otherwise,
 * returns 1 if number of permutations in P is even, -1 if odd
 * (it is useful for couting determinant) */
template <class T>
template <class A>
int Matrix<T>::LUP_decomposition_impl(Matrix<A>& C, Permutation& P) const
{
	if (m_nrows != m_ncolumns)
		return 0;

	int npermutations = 0;
	C = *this;
	P.reset(m_nrows);

	for (size_t j = 0; j < m_nrows; ++j) {
		/* search pivot element */
		A pivot_value{};
		ssize_t pivot_idx = -1;
		for (size_t i = j; i < m_nrows; ++i ) {
			if (absval(C[i][j]) > pivot_value ) {
				pivot_value = absval(C[i][j]);
				pivot_idx = i;
			}
		}
		if (pivot_idx != -1) {
			if (pivot_idx != j) { // swapinng j-s line with pivot line
				++npermutations;
				P.swap(pivot_idx, j);
				C.swap_rows(pivot_idx, j);
			}
			for (size_t i = j + 1; i < m_nrows; ++i) {
				C[i][j] /= C[j][j]; // pivot_value = abs(C[j][j])
				for(size_t k = j + 1; k < m_nrows; ++k) 
					C[i][k] -= C[i][j] * C[j][k];
			}
		} else
			return 0;
	}
	return (npermutations % 2 == 0) ? 1 : -1;
}

template <class A>
Matrix<A>
LUPDecomposition<A>::L() const
{
	const size_t n = m_C.nrows();

	Matrix<A> res = identity_matrix<A>(n);
	for (size_t i = 1; i < n; ++i)
		for (size_t j = 0; j < i; ++j)
			res[i][j] = m_C[i][j];
	return res;
}

template <class A>
Matrix<A>
LUPDecomposition<A>::U() const
{
	const size_t n = m_C.nrows();
	return Matrix<A>(n, n, [&](size_t i, size_t j) { return (i > j) ? m_C[i][j] : 0; } );
}

} // Maths namespace end

#endif // MATHS_MATRIX_IMPL_H_