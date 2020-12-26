/* Implementation of Matrix methods */

#ifndef MATHS_MATRIX_IMPL_H_
#define MATHS_MATRIX_IMPL_H_

#include <cmath>
#include <iostream>

namespace Maths {

template <class T>
Matrix<T>::Matrix(size_t rows, size_t columns, const T& value /* = T{} */) :
	Matrix(rows, columns, [&](size_t, size_t) { return value; } ) {}

template <class T>
Matrix<T>::Matrix(const Matrix& other) :
	Matrix(other.m_nrows, other.m_ncolumns,
		[&](size_t i, size_t j) { return other.m_data[i][j]; } ) {}

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

template <class T>
template <class U>
Matrix<U> Matrix<T>::convert_to() const
{
	return Matrix<U>(m_nrows, m_ncolumns,
		[&](size_t i, size_t j) { return static_cast<U>(m_data[i][j]); } );
}

template <class T>
Matrix<T> Matrix<T>::get_transposed() const
{
	return Matrix<T>(m_ncolumns, m_nrows,
		[this](size_t i, size_t j) { return m_data[j][i]; } );
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
template <class A /* = promote_to_algebraic_field_t<T> */>
LUPDecomposition<A>
Matrix<T>::LUP_decomposition() const
{
	Matrix<promoted_value_type> C;
	Permutation P;
	int n = LUP_decomposition_impl(C, P);
	if (n == 0) // decomposition failed
		return LUPDecomposition<A>();
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
				C[i][j] /= pivot_value;
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
	assert(m_valid);
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
	assert(m_valid);
	const size_t n = m_C.nrows();

	Matrix<A> res(n, n);
	for (size_t i = 0; i < n; ++i)
		for (size_t j = i; j < n; ++j)
			res[i][j] = m_C[i][j];
	return res;
}

} // Maths namespace end

#endif // MATHS_MATRIX_IMPL_H_