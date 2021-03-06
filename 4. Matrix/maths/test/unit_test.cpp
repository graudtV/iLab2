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
	Maths::Matrix<TestType> m = Maths::identity_matrix<TestType>(10);
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
	CHECK(*m1 == m2);

	/* check copy assign */
	Maths::Matrix<TestType> m3(10, 20, 30);
	m3 = m2;
	CHECK(m3 == m2);
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
	CHECK(m2.get_transposed() == m1);
}

TEST_CASE("main_diagonal_elements_product", "[Matrix]")
{
	Maths::Matrix<float> m(3, 3);
	m[0][0] = 0.1;
	m[1][1] = 0.5;
	m[2][2] = 0.35;
	CHECK(m.main_diagonal_elements_product() == 0.0175f);
}

TEST_CASE("operator ==, operator !=", "[Matrix]")
{
	CHECK(Maths::identity_matrix<int>(0) == Maths::identity_matrix<int>(0));
	CHECK(Maths::identity_matrix<int>(1) == Maths::identity_matrix<int>(1));
	CHECK(Maths::identity_matrix<int>(5) == Maths::identity_matrix<int>(5));
	CHECK(Maths::identity_matrix<int>(5) != Maths::identity_matrix<int>(6));
	auto m = Maths::identity_matrix<int>(5);
	m[2][3] = 2;
	CHECK(m != Maths::identity_matrix<int>(5));
}

TEST_CASE("cut", "[Matrix]")
{
	Maths::Matrix<int> m { 4, 4, {
		1, 2, 3, 4,
		5, 6, 7, 8,
		9, 10, 11, 12,
		13, 14, 15, 16 }};
	Maths::Matrix<int> expected { 2, 3, {
		6, 7, 8,
		10, 11, 12
	}};
	CHECK(expected == m.get_cut(1, 2, 1, 3));
}

TEMPLATE_TEST_CASE("operator +=, +", "[Matrix]", int, float, double)
{
	Maths::Matrix<int> m { 3, 3, {
		1, 2, 3,
		4, 5, 6,
		7, 8, 9 }};
	Maths::Matrix<int> expected { 3, 3, {
		2, 4, 6,
		8, 10, 12,
		14, 16, 18
	}};
	CHECK(m + m == expected);
	CHECK((m += m) == expected);
}

TEMPLATE_TEST_CASE("operator -=, -", "[Matrix]", int, float, double)
{
	Maths::Matrix<int> m { 3, 3, {
		1, 2, 3,
		4, 5, 6,
		7, 8, 9 }};
	Maths::Matrix<int> expected(3, 3);
	CHECK(m - m == expected);
	CHECK((m -= m) == expected);
}

TEMPLATE_TEST_CASE("operator *=, *", "[Matrix]", int, float, double)
{
	Maths::Matrix<TestType> m { 3, 3, {
		1, 2, 3,
		4, 5, 6,
		7, 8, 9 }};
	Maths::Matrix<TestType> expected { 3, 3, {
		30, 36, 42,
		66, 81, 96,
		102, 126, 150
	}};
	CHECK(m * m == expected);
	CHECK((m *= m) == expected);
}

TEMPLATE_TEST_CASE("invert(), get_inverted()", "[Matrix]", int, float, double)
{
	SECTION ("not invertible") {
		Maths::Matrix<TestType> m(10, 20, rand_val<TestType>);
		CHECK_THROWS_AS(m.invert(), Maths::InvertionError);		
	}
	SECTION ("invertible") {
		Maths::Matrix<TestType> m(100, 100, rand_val<TestType>); // it is probably invertible
		auto product1 = m.get_inverted() * m.template convert_to<typename Maths::Matrix<TestType>::promoted_value_type>();
		auto product2 = m.template convert_to<typename Maths::Matrix<TestType>::promoted_value_type>() * m.get_inverted();
		CHECK(product1.template convert_to<int>() == Maths::identity_matrix<int>(100));
		CHECK(product2.template convert_to<int>() == Maths::identity_matrix<int>(100));
	}
}

TEMPLATE_TEST_CASE("initializer_list", "[Matrix]", int, float, double)
{
	Maths::Matrix<TestType> m(2, 2);
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
		CHECK_THROWS_AS(m.LUP_decomposition(), Maths::DecompositionError);
		CHECK(m.determinant() == 0);
	}
}
