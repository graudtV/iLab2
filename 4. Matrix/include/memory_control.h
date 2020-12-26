/* Details for controling Matrix memory */

#ifndef MATHS_MEMORY_CONTROL_H_
#define MATHS_MEMORY_CONTROL_H_

#include <cassert>

namespace Maths::details {

template <class T>
T *alloc_raw_linear_mem_noexcept(size_t nelems) noexcept
	{ return static_cast<T *>(::operator new (nelems * sizeof(T), std::nothrow)); }

template <class T>
void dealloc_raw_linear_mem(T *mem) noexcept
	{ ::operator delete (mem); }

template <class T>
void dealloc_raw_square_mem(T **data, size_t rows) noexcept
{
	for (T **p = data; p != data + rows; ++p)
		dealloc_raw_linear_mem(*p);
	dealloc_raw_linear_mem(data);
}

/* Allocates jagged array. Each row and pointers array are allocated
 * seperately (so they may be deleted separetely) */
template <class T>
T **alloc_raw_square_mem_noexcept(size_t rows, size_t columns) noexcept
{
	T **data;
	if (!(data = alloc_raw_linear_mem_noexcept<T *>(rows)))
		return nullptr;
	for (T **p = data; p != data + rows; ++p)
		if (!(*p = alloc_raw_linear_mem_noexcept<T>(columns))) {
			dealloc_raw_square_mem(data, p - data);
			return nullptr;
		}
	return data;
}

/* throws std::bad_alloc on error */
template <class T>
T **alloc_raw_square_mem(size_t rows, size_t columns)
{
	T **data = alloc_raw_square_mem_noexcept<T>(rows, columns);
	if (data == nullptr && rows != 0) // for empty array nullptr is ok
		throw std::bad_alloc();
	return data;
}

/* constructs object in already allocated memory */
template <class T>
void construct(T *raw, const T& value)
	{ new (raw) T (value); }

/* calles destructor for object without releasing dynamic memory */
template <class T>
void destruct(T *obj)
	{ obj->~T(); }


/* Matrix memory manager 
 * Data is stored as jagged array */
template <class T>
class MatrixBuf {
protected:
	T **m_data;
	size_t m_nrows;
	size_t m_ncolumns;
	size_t m_nfilled_elems;

	MatrixBuf() : m_data(0), m_nrows(0), m_ncolumns(0), m_nfilled_elems(0) {}
	MatrixBuf(size_t rows, size_t columns) :
		m_data(alloc_raw_square_mem<T>(rows, columns)),
		m_nrows(rows),
		m_ncolumns(columns),
		m_nfilled_elems(0) {}
	~MatrixBuf()
	{
		for (size_t i = 0; i < m_nrows; ++i)
			for (int j = 0; i < m_ncolumns && m_nfilled_elems > 0; ++j) {
				destruct(&m_data[i][j]);
				--m_nfilled_elems;
			}
		assert(m_nfilled_elems == 0);
		dealloc_raw_square_mem(m_data, m_nrows);
	}

	MatrixBuf(const MatrixBuf& other) = delete;
	MatrixBuf& operator =(const MatrixBuf& other) = delete;
	MatrixBuf& operator =(MatrixBuf&& other) = delete;
	MatrixBuf(MatrixBuf&& other) = delete;

	void swap(MatrixBuf& other) noexcept
	{
		std::swap(m_data, other.m_data);
		std::swap(m_nrows, other.m_nrows);
		std::swap(m_ncolumns, other.m_ncolumns);
		std::swap(m_nfilled_elems, other.m_nfilled_elems);
	}
};

} // Maths::details namespace end

#endif // MATHS_MEMORY_CONTROL_H_