#define CATCH_CONFIG_MAIN
#include "matrix.h"
#include "../../../catch.hpp"

template class Maths::Matrix<int>;
template class Maths::Matrix<float>;
template class Maths::Matrix<double>;

/* from 0 to 1 */
float rnd_float()
	{ return static_cast<float>(rand()) / static_cast<float>(RAND_MAX); }

/* from min to max */
float rnd_float(float min, float max)
	{ return rnd_float() * (max - min) + min; }

template <class T>
T rand_val(size_t, size_t)
	{ return rnd_float(-100, 100); }

#define FOR_EACH(i, j, mrx) \
	for (size_t (i) = 0; (i) < (mrx).nrows(); ++(i)) \
		for (size_t (j) = 0; (j) < (mrx).ncolumns(); ++(j))

TEMPLATE_TEST_CASE("identity_matrix", "[Matrix]", int, float, double)
{
	Maths::Matrix<TestType> m = Maths::identity_matrix<TestType>(100);
		FOR_EACH(i, j, m) {
			INFO("i=" << i << "; j=" << j)
			if (i == j)
				CHECK(m[i][j] == 1);
			else
				CHECK(m[i][j] == 0);
		}
}

TEMPLATE_TEST_CASE("copy constructor and copy assign", "[Matrix]", int, float, double)
{
	Maths::Matrix<TestType> mrx_wide(200, 20, rand_val<TestType>);
	Maths::Matrix<TestType> mrx_narrow(20, 200, rand_val<TestType>);
	Maths::Matrix<TestType> mrx_empty(0, 0, rand_val<TestType>);
	Maths::Matrix<TestType> *m1 = nullptr;
	SECTION("wide") { m1 = &mrx_wide; }
	SECTION("narrow") { m1 = &mrx_narrow; }
	SECTION("empty") { m1 = &mrx_empty; }

	/* check copy ctr */
	Maths::Matrix<TestType> m2(*m1);
	FOR_EACH(i, j, *m1) {
		INFO("i=" << i << "; j=" << j);
		CHECK((*m1)[i][j] == m2[i][j]);
	}

	/* check copy assign */
	Maths::Matrix<TestType> m3(10, 20, 30);
	m3 = m2;
	FOR_EACH(i, j, m3) {
		INFO("i=" << i << "; j=" << j);
		CHECK(m3[i][j] == m2[i][j]);
	}
}

TEMPLATE_TEST_CASE("get_transposed()", "[Matrix]", int, float, double)
{
	Maths::Matrix<TestType> m1(200, 20, rand_val<TestType>);
	auto m2 = m1.get_transposed();

	CHECK(m2.nrows() == m1.ncolumns());
	CHECK(m2.ncolumns() == m1.nrows());
	FOR_EACH(i, j, m2) {
		INFO("i=" << i << "; j=" << j);
		CHECK(m2[i][j] == m1[j][i]);
	}
}

TEST_CASE("main_diagonal_elements_product", "[Matrix]")
{
	Maths::Matrix<float> m(3, 3);
	m[0][0] = 0.1;
	m[1][1] = 0.5;
	m[2][2] = 0.35;
	CHECK(m.main_diagonal_elements_product() == 0.0175f);
}

TEMPLATE_TEST_CASE("initializer_list", "[Matrix]", int, float, double)
{
	Maths::Matrix<int> m(2, 2);
	SECTION("exact amount") { m = {1, 2, 3, 4}; }
	SECTION("extra values") { m = {1, 2, 3, 4, 5, 6, 7}; }

	FOR_EACH(i, j, m) {
		INFO("i=" << i << "; j=" << j);
		CHECK(m[i][j] == i * m.ncolumns() + j + 1);
	}
}

TEMPLATE_TEST_CASE("empty initializer_list", "[Matrix]", int, float, double)
{
	Maths::Matrix<int> m{2, 2, {}};
	FOR_EACH(i, j, m) {
		INFO("i=" << i << "; j=" << j);
		CHECK(m[i][j] == 0);
	}
}

template <class T>
std::ostream&
operator <<(std::ostream& os, const Maths::Matrix<T>& m)
{
	for (int i = 0; i < m.nrows(); ++i) {
		for (int j = 0; j < m.ncolumns(); ++j)
			os << std::setw(10) << m[i][j] << " ";
		os << std::endl;
	}
	return os;
}

TEST_CASE("LUP_decomposition() and determinant()", "[Matrix]")
{
	SECTION ("not singular matrix") {
		Maths::Matrix<int> m { 3, 3, {
			12,  -2,  1,
			36,  0,   7,
			180, 12,  38 }};
		Maths::Matrix<int>::default_decomposition_type decomposition;
		CHECK_NOTHROW(decomposition = m.LUP_decomposition());
		CHECK(m.determinant() == -360);
	}
	SECTION ("singular matrix") {
		Maths::Matrix<int> m(3, 3);
		CHECK_THROWS_AS(m.LUP_decomposition(), Maths::Matrix<int>::DecompositionError);
		CHECK(m.determinant() == 0);
	}
}
