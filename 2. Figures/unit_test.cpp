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

TEST_CASE ( "Segment::contains()", "[Segment]" ) {
	Geometry::Segment seg(null_point, {1, 0, 0});
	REQUIRE(seg.contains({0.5, 0, 0}));
	REQUIRE(!seg.contains({0.5, 0.5, 0}));
	REQUIRE(!seg.contains({2, 0, 0}));
}

TEST_CASE ( "Line::contains()", "[Line]" ) {
	Geometry::Line line(null_point, {1, 0, 0});
	REQUIRE(line.contains({0.5, 0, 0}));
	REQUIRE(!line.contains({0.5, 0.5, 0}));
	REQUIRE(line.contains({2, 0, 0}));
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

TEST_CASE ( "intersection(Line, Triangle)", "[intersections]" ) {
	Geometry::Triangle triangle(null_point, {1, 0, 0}, {0, 1, 0});
	
	SECTION ( "same plane, not intersected" ) {
		Geometry::Line line({2, 0, 0}, {0, 2, 0});
		REQUIRE(std::holds_alternative<Geometry::EmptySet>(intersection(line, triangle)));
	}
	SECTION ( "same plane, intersected" ) {
		Geometry::Line line({0.5, 0, 0}, {0, 0.5, 0});
		auto intersection = Geometry::intersection(line, triangle);
		REQUIRE(std::holds_alternative<Geometry::FinitePointSet>(intersection));
		REQUIRE(std::get<Geometry::FinitePointSet>(intersection).size() == 2);
	}
	SECTION ( "parallel planes (not intersected)" ) {
		Geometry::Line line({1, 0, 1}, {0, 1, 1});
		REQUIRE(std::holds_alternative<Geometry::EmptySet>(intersection(line, triangle)));
	}
	SECTION ( "line intersects plane, but not triangle" ) {
		Geometry::Line line({0, 0, 1}, {0, 1, 2});
		REQUIRE(std::holds_alternative<Geometry::EmptySet>(intersection(line, triangle)));
	}
	SECTION ( "line intersects plane and triangle side" ) {
		Geometry::Line line({0.5, 0, -1}, {0.5, 0, 1});
		auto intersection = Geometry::intersection(line, triangle);
		REQUIRE(std::holds_alternative<Geometry::FinitePointSet>(intersection));
		REQUIRE(std::get<Geometry::FinitePointSet>(intersection).size() == 1);		
	}
}

TEST_CASE ( "intersection(Segment, Segment)", "[intersections]" ) {
	Geometry::Segment seg1(null_point, {1, 0, 0});
	SECTION ( "on one line, not intersected" ) {
		Geometry::Segment seg2({2, 0, 0},  {3, 0, 0});
		REQUIRE(std::holds_alternative<Geometry::EmptySet>(intersection(seg1, seg2)));
	}
	SECTION( "on one line, intersected" ) {
		Geometry::Segment seg2({0.5, 0, 0},  {1.5, 0, 0});
		auto intersection = Geometry::intersection(seg1, seg2);
		REQUIRE(std::holds_alternative<Geometry::Segment>(intersection));
	}
	SECTION ( "one point intersection" ) {
		Geometry::Segment seg2({0.5, 1, 0}, {0.5, -1, 0});
		REQUIRE(std::holds_alternative<Geometry::Point>(intersection(seg1, seg2)));
	}
}

// TODO: дописать тест ниже
TEST_CASE ( "intersected(Triangle, Triangle)", "[intersections]" ) {
	Geometry::Triangle trg1(null_point, {1, 0, 0}, {0, 1, 0});

	SECTION( "on parallel planes (not intersected)" ) {
		Geometry::Triangle trg2({0, 0, 1}, {1, 0, 1}, {0, 1, 1});
		REQUIRE(!intersected(trg1, trg2));
	}
	SECTION ( "all points of trg2 are on one halfplane relatively to trg1" ) {
		Geometry::Triangle trg2({0, 0, 1}, {1, 0, 3}, {0, 1, 2});
		REQUIRE(!intersected(trg1, trg2));		
	}
	SECTION ( "intersection (as segment)" ) {
		Geometry::Triangle trg2({-1, -2, 1}, {1, 2, 4}, {2, 4, -3});
		REQUIRE(intersected(trg1, trg2));			
	}
	SECTION ( "trg2 have points on different halfplanes relatively to trg1"
		" but triangles are not intersected" ) {
		Geometry::Triangle trg2({-1, -2, 1}, {1, -2, 4}, {2, -4, -3});
		REQUIRE(intersected(trg1, trg2));			
	}
}
