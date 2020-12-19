#ifndef MATHS_MATRIX_H_
#define MATHS_MATRIX_H_

#include <vector>
#include "type_conversions.h"

namespace Maths {

class Permutation;
template <class T> class LUPDecomposition;


template <class T>
class Matrix final {
public:
	using value_type = T;
	using promoted_value_type = promote_to_algebraic_field_t<T>;

	struct ProxyRow;
	using ConstProxyRow = const ProxyRow;

	Matrix() : m_data(nullptr), m_nrows(0), m_ncolumns(0) {}
	Matrix(size_t rows, size_t columns); // filled with default values
	Matrix(size_t rows, size_t columns, const T& value) :
		Matrix(rows, columns) { fill_with(value); }
	~Matrix();
	Matrix(const Matrix& other);
	Matrix(Matrix&& other) : Matrix() { *this = other; }
	Matrix& operator =(const Matrix& other);
	Matrix& operator =(Matrix&& other) noexcept;

	template <class U>
	explicit Matrix(const Matrix<U>& other) :
		Matrix(other.template convert_to<T>()) {}

	template <class U>
	Matrix<T>& operator =(const Matrix<U>& other)
		{ return *this = other.template convert_to<T>(); }

	/* cast matrix to another type */
	template <class U> Matrix<U> convert_to() const;

	Matrix<T> get_transposed() const;

	ProxyRow operator [] (size_t i) { return {m_data[i], m_ncolumns}; }
	ConstProxyRow operator [] (size_t i) const { return {m_data[i], m_ncolumns}; }

	void fill_with(const T& value);

	size_t nrows() const { return m_nrows; }
	size_t ncolumns() const { return m_ncolumns; }
	bool empty() const { return m_nrows == 0 || m_ncolumns == 0; }
	bool is_square() const { return m_nrows == m_ncolumns; }

	T determinant() const;
	void transpose() { *this = get_transposed(); }
	T main_diagonal_elements_product() const;
	void swap_rows(size_t i, size_t j) { std::swap(m_data[i], m_data[j]); }
	void swap_rows(size_t i, Matrix& other, size_t j)
		{ std::swap(m_data[i], other.m_data[j]); }
	void permute_rows(const Permutation& p);

	template <class A = promote_to_algebraic_field_t<T>>
	LUPDecomposition<A> LUP_decomposition() const;

private:
	T **m_data;
	size_t m_nrows;
	size_t m_ncolumns;

	template <class A> int LUP_decomposition_impl(Matrix<A>& C, Permutation& P) const;
};


template <class T>
class Matrix<T>::ProxyRow final {
public:
	T& operator[] (size_t j) const { return m_row[j]; }
	size_t size() const { return m_size; }

private:
	T *m_row; // not owning access
	size_t m_size;
	ProxyRow(const T *row, size_t size) :
		m_row(const_cast<T *>(row)),
		m_size(size) {}

	friend class Matrix<T>;
};


class Permutation final {
public:
	using ConstIterator = typename std::vector<size_t>::const_iterator;
	using Iterator = ConstIterator;

	Permutation() {}
	explicit Permutation(size_t nvalues) { reset(nvalues); }

	void reset(size_t nvalues);
	size_t nvalues() const { return m_numbers.size(); }
	void swap(int i, int j) { std::swap(m_numbers[i], m_numbers[j]); }

	ConstIterator begin() const { return m_numbers.cbegin(); }
	ConstIterator end() const { return m_numbers.cend(); }
private:
	std::vector<size_t> m_numbers;
};


/*  Decomposition of matrix A in form PA=LU
 *  All not singular square matrices and only they have such decomposition.
 *  If decomposition exists, valid() returns true and decomposition
 * components can be obtained via L(), U(), P(), ...
 *  Otherwise, valid() returns false, and calling L(), ...
 * is undefined behaviour
 *  C = L + U - E
 */
template <class A>
class LUPDecomposition final {
public:
	Matrix<A> L() const;
	Matrix<A> U() const;
	Matrix<A> P() const;
	Matrix<A> C() const { assert(valid()); return m_C; }
	Permutation P_vec() const { assert(m_valid); return m_P_vec; }
	bool valid() const { return m_valid; }

	LUPDecomposition() : m_valid(false) {}
	LUPDecomposition(const Matrix<A>& C, const Permutation& P) :
		m_C(C), m_P_vec(P), m_valid(true) { assert(C.is_square()); }

private:
	Matrix<A> m_C;
	Permutation m_P_vec;
	bool m_valid;
};

template <class T>
Matrix<T> identity_matrix(size_t size);

} // Maths namespace end

#include "matrix_impl.h"

#endif // MATHS_MATRIX_H_