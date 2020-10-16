#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "geometry.h"
#include <cmath>

TEST_CASE( "Point::valid()", "[Point]" ) {
	REQUIRE(Geometry::Point(0, 0, 0).valid());
	REQUIRE(!Geometry::Point(NAN, 0, 0).valid() );
	REQUIRE(!Geometry::Point(0, HUGE_VAL, 0).valid());
	REQUIRE(!Geometry::Point(0, 0, INFINITY).valid());
	REQUIRE(!Geometry::Point(FLT_MIN/2, 0, 0).valid());
}

TEST_CASE( "Point::operator ==()", "[Point]") {
	REQUIRE(Geometry::Point(0, 0, 0) == Geometry::null_point);
	REQUIRE(Geometry::null_point == Geometry::null_point);
	REQUIRE(Geometry::null_point
		== Geometry::Point(0, 0, Geometry::Float::float_tolerance - 1e-10));
	REQUIRE(!(Geometry::null_point
		== Geometry::Point(0, 0, Geometry::Float::float_tolerance + 1e-9)));
	REQUIRE(Geometry::Point(10, 20, 30) == Geometry::Point(
		10 + Geometry::Float::float_tolerance/2,
		20 + Geometry::Float::float_tolerance/2,
		30 + Geometry::Float::float_tolerance/2));
}

TEST_CASE ( "Vector::inner_product", "[Vector]" ) {
	REQUIRE(Geometry::Vector::inner_product(
		Geometry::null_vector, Geometry::Vector(10, 20, 30)) == 0);
	REQUIRE(Geometry::Vector::inner_product(
		Geometry::Vector(1, 2, 3), Geometry::Vector(-1, 2, -1)) == 0);
	REQUIRE(Geometry::Vector::inner_product(
		Geometry::Vector(1.2, 3.4, 5.6), Geometry::Vector(7.8, 9.0, 0.1)) == 40.52f);
}

TEST_CASE ( "Vector::outer_product", "[Vector]" ) {
	REQUIRE(Geometry::Vector::outer_product(
		Geometry::null_vector, { 10, 20, 30 } ) == Geometry::null_vector);
	REQUIRE(Geometry::Vector::outer_product(
		{ 1, 2, 3 }, { 10, 20, 30 } ) == Geometry::null_vector);
	REQUIRE(Geometry::Vector::outer_product(
		{ 1.2, 3.4, 5.6 }, { 7.8, 9.0, 0.1 })
		== Geometry::Vector(-50.06f, 43.56f, -15.72f));
}

TEST_CASE ( "Vector::mixed_product", "[Vector]" ) {
	REQUIRE(Geometry::Vector::mixed_product(
		{ 1, 0, 0 }, { 0, 1, 0 }, { 1.2, 3.4, 0 } ) == 0);
	REQUIRE(Geometry::Vector::mixed_product(
		{ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } ) == 1);
	REQUIRE(Geometry::Vector::mixed_product(
		{ 1.23, 2.31, 3.12 }, { 4.56, 5.64, 6.45 }, { 7.89, -8.97, 9.78 } )
		== -112.908978 );
}