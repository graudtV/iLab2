#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "geometry.h"
#include <cmath>

auto null_point = Geometry::Point::null_point;
auto null_vector = Geometry::Vector::null_vector;

TEST_CASE( "Point::valid()", "[Point]" ) {
	REQUIRE(Geometry::Point(0, 0, 0).valid());
	REQUIRE(!Geometry::Point(NAN, 0, 0).valid() );
	REQUIRE(!Geometry::Point(0, HUGE_VAL, 0).valid());
	REQUIRE(!Geometry::Point(0, 0, INFINITY).valid());
	REQUIRE(!Geometry::Point(FLT_MIN/2, 0, 0).valid());
}

TEST_CASE( "Point::operator ==()", "[Point]") {
	REQUIRE(Geometry::Point(0, 0, 0) == null_point);
	REQUIRE(null_point == null_point);
	REQUIRE(null_point
		== Geometry::Point(0, 0, Geometry::Float::float_tolerance - 1e-10));
	REQUIRE(!(null_point
		== Geometry::Point(0, 0, Geometry::Float::float_tolerance + 1e-9)));
	REQUIRE(Geometry::Point(10, 20, 30) == Geometry::Point(
		10 + Geometry::Float::float_tolerance/2,
		20 + Geometry::Float::float_tolerance/2,
		30 + Geometry::Float::float_tolerance/2));
}

TEST_CASE ( "Vector::inner_product", "[Vector]" ) {
	REQUIRE(Geometry::Vector::inner_product(
		null_vector, Geometry::Vector(10, 20, 30)) == 0);
	REQUIRE(Geometry::Vector::inner_product(
		Geometry::Vector(1, 2, 3), Geometry::Vector(-1, 2, -1)) == 0);
	REQUIRE(Geometry::Vector::inner_product(
		Geometry::Vector(1.2, 3.4, 5.6), Geometry::Vector(7.8, 9.0, 0.1)) == 40.52f);
}

TEST_CASE ( "Vector::outer_product", "[Vector]" ) {
	REQUIRE(Geometry::Vector::outer_product(
		null_vector, { 10, 20, 30 } ) == null_vector);
	REQUIRE(Geometry::Vector::outer_product(
		{ 1, 2, 3 }, { 10, 20, 30 } ) == null_vector);
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

TEST_CASE ( "vdistance(Point, Point)", "[distance]" ) {
	REQUIRE(Geometry::vdistance({0, 0, 0}, {10, 20, 30})
		== Geometry::Vector(10, 20, 30));
	REQUIRE(Geometry::vdistance({ 1.2, 3.4, 5.6 }, { 7.8, 9.0, 0.1 })
		== Geometry::Vector(6.6, 5.6, -5.5));
}

TEST_CASE( "distance(Point, Plane)", "[distance]" ) {
	REQUIRE(Geometry::distance(
		Geometry::Point(1, 1, 1),
		Geometry::Plane({0, 0, 0}, {1, -1, 0}, {1, 1, -2}) // x + y + z = 0
		) == 1.7320508075688772f);
}

TEST_CASE ( "intersection(Plane, Plane)", "[intersections]" ) {
	auto oxy = Geometry::Plane::oxy;
	auto oyz = Geometry::Plane::oyz;
	Geometry::Plane shifted_oxy({0, 0, 1}, {1, 0, 1}, {0, 1, 1});
	REQUIRE(std::holds_alternative<Geometry::Plane>(intersection(oxy, oxy)));
	REQUIRE(std::holds_alternative<Geometry::Line>(intersection(oxy, oyz)));
	REQUIRE(std::holds_alternative<Geometry::EmptySet>(intersection(oxy, shifted_oxy)));
}

// TODO: дописать тест ниже
TEST_CASE ( "intersected(Triangle, Triangle)", "[intersections]" ) {
	Geometry::Triangle t1({0, 0, 0}, {1, 0, 0}, {0, 1, 0});
	Geometry::Triangle t1_shifted({0, 0, 1}, {1, 0, 1}, {0, 1, 1});
	REQUIRE(!intersected(t1, t1_shifted));
}
