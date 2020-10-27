#define CATCH_CONFIG_MAIN
#include "../../catch.hpp"
#include "../geometry.h"
#include <cmath>

auto& null_point = Geometry::Point::null_point;
auto& null_vector = Geometry::Vector::null_vector;

TEST_CASE( "Point::valid()", "[Point]" ) {
	REQUIRE(Geometry::Point(0, 0, 0).valid());
	REQUIRE(Geometry::Point(1.2, 3.4, 4.5).valid());
	REQUIRE(!Geometry::Point(NAN, 0, 0).valid() );
	REQUIRE(!Geometry::Point(0, HUGE_VAL, 0).valid());
	REQUIRE(!Geometry::Point(0, 0, INFINITY).valid());
	REQUIRE(!Geometry::Point(FLT_MIN/2, 0, 0).valid());
}

TEST_CASE( "operator ==(Point, Point)", "[Point]") {
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

TEST_CASE ( "operator ==(Segment, Segment", "[Segment]") {
	Geometry::Point a(1, 2, 3), b(4, 5, 6);
	REQUIRE(Geometry::Segment(a, b) == Geometry::Segment(a, b));
	REQUIRE(Geometry::Segment(a, b) == Geometry::Segment(b, a));
	REQUIRE(!(Geometry::Segment(a, b) == Geometry::Segment(null_point, b)));
}

TEST_CASE ( "Line::contains()", "[Line]" ) {
	Geometry::Line line(null_point, {1, 0, 0});
	REQUIRE(line.contains({0.5, 0, 0}));
	REQUIRE(!line.contains({0.5, 0.5, 0}));
	REQUIRE(line.contains({2, 0, 0}));
}

TEST_CASE ("Vector::decompose(Vector)", "[Vector]" ) {
	SECTION ( "correct decomposition" ) {
		REQUIRE(Geometry::Vector(10, 20, 30).decompose({1, 2, 3}) == 10);
		REQUIRE(Geometry::Vector(-10, 0, -30).decompose({1, 0, 3}) == -10);
		REQUIRE(Geometry::Vector(0, 2, 0).decompose({0, 1, 0}) == 2);
	}
	SECTION ( "impossible decomposition" ) {
		bool exception_thrown = false;
		try { Geometry::Vector(10, 20, 30).decompose({1, 2, 3.5}); }
		catch (Geometry::Vector::DecompositionError&) { exception_thrown = true; }
		REQUIRE(exception_thrown);
	}
}

TEST_CASE ("Vector::decompose(Vector, Vector)", "[Vector]" ) {
	SECTION ( "correct decomposition test 1" ) {
		// v3 = 2.5*v1 - 3*v2
		Geometry::Vector v1 = {1, 2, 3}, v2 = {2, 7, 6}, v3 = {-3.5, -16, -10.5};
		auto koeffs = v3.decompose(v1, v2);
		REQUIRE(koeffs[0] == 2.5);
		REQUIRE(koeffs[1] == -3.0);
	}
	SECTION ( "correct decomposition test 2" ) {
		// v3 = v1 + v2
		Geometry::Vector v1 = {1, 0, 2}, v2 = {-1, 0, -1}, v3 = {0, 0, 1};
		auto koeffs = v3.decompose(v1, v2);
		REQUIRE(koeffs[0] == 1);
		REQUIRE(koeffs[1] == 1);
	}
	SECTION ( "impossible decomposition" ) {
		bool exception_thrown = false;
		try { Geometry::Vector(1, 0, 0).decompose({0, 1, 0}, {0, 0, 1}); }
		catch (Geometry::Vector::DecompositionError&) { exception_thrown = true; }
		REQUIRE(exception_thrown);
	}	
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
		== Geometry::Vector(-50.06, 43.56, -15.72));
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

TEST_CASE ( "Vector::collinear", "[Vector]" ) {
	REQUIRE(Geometry::Vector::collinear({10, 20, 30}, {15, 30, 45}));
	REQUIRE(!Geometry::Vector::collinear({10, 20, 30}, {15, 30, 46}));
	REQUIRE(Geometry::Vector::collinear({1, 0, 0}, {1, 0, 0}));
	REQUIRE(Geometry::Vector::collinear({10, 20, 30}, {0, 0, 0}));
}

TEST_CASE ( "Plane::contains(Line)", "[Plane]" ) {
	REQUIRE(Geometry::Plane::oxy.contains(Geometry::Line({10, 20, 0}, {30, 40, 0})));
	REQUIRE(!Geometry::Plane::oxy.contains(Geometry::Line({10, 20, 0}, {30, 40, 1})));
	REQUIRE(!Geometry::Plane::oxy.contains(Geometry::Line({10, 20, -1}, {30, 40, 1})));
	REQUIRE(!Geometry::Plane::oxy.contains(Geometry::Line({10, 20, 1}, {30, 40, 1})));
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
		REQUIRE(!intersected(trg1, trg2));			
	}
	SECTION ( "triangles are on the same plane, but not intersected (hard case)" ) {
		Geometry::Triangle trg2({1, 1, 0}, {2, 1, 0}, {1, 2, 0});
		REQUIRE(!intersected(trg1, trg2));
	}
	SECTION ( "triangles are on the same plane, and intersected (hard case)" ) {
		Geometry::Triangle trg2({1, 1, 0}, {-100, -99, 0}, {-99, -100, 0});
		REQUIRE(intersected(trg1, trg2));
	}
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
	SECTION ( "intersection is segment test 1" ) {
		Geometry::Line line({0.5, 0, 0}, {0, 0.2, 0});
		auto intrsctn = Geometry::intersection(line, triangle);
		REQUIRE(std::holds_alternative<Geometry::Segment>(intrsctn));
		REQUIRE(std::get<Geometry::Segment>(intrsctn)
			== Geometry::Segment({0.5, 0, 0}, {0, 0.2, 0}));
	}
	SECTION ( "intersection is segment test 2") {
		Geometry::Line line({0.0, 0, 0}, {1, 0, 2});
		Geometry::Triangle trg(null_point, {1, 0, 0}, {0, 0, 1});
		auto intrsctn = Geometry::intersection(line, trg);
		REQUIRE(std::holds_alternative<Geometry::Segment>(intrsctn));
		REQUIRE(std::get<Geometry::Segment>(intrsctn)
			== Geometry::Segment({0, 0, 0}, {1.0/3, 0.0, 2.0/3}));
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
		auto intrsctn = Geometry::intersection(line, triangle);
		REQUIRE(std::holds_alternative<Geometry::Point>(intrsctn));
		REQUIRE(std::get<Geometry::Point>(intrsctn) == Geometry::Point(0.5, 0, 0));
	}
	SECTION ( "line goes throw triangle (intersection is point)" ) {
		Geometry::Line line({0.2, 0.2, -1}, {0.2, 0.2, 1});
		auto intrsctn = Geometry::intersection(line, triangle);
		REQUIRE(std::holds_alternative<Geometry::Point>(intrsctn));	
		REQUIRE(std::get<Geometry::Point>(intrsctn) == Geometry::Point(0.2, 0.2, 0));
	}
	SECTION ( "triangle side lies on the line" ) {
		Geometry::Line line({1, 0, 0}, {0, 1, 0});
		auto intrsctn = Geometry::intersection(line, triangle);
		REQUIRE(std::holds_alternative<Geometry::Segment>(intrsctn));
		REQUIRE(std::get<Geometry::Segment>(intrsctn)
			== Geometry::Segment({1, 0, 0}, {0, 1, 0}));
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
		auto intrsctn = Geometry::intersection(seg1, seg2);
		REQUIRE(std::holds_alternative<Geometry::Segment>(intrsctn));
	}
	SECTION ( "one point intersection" ) {
		Geometry::Segment seg2({0.5, 1, 0}, {0.5, -1, 0});
		REQUIRE(std::holds_alternative<Geometry::Point>(intersection(seg1, seg2)));
	}
}

TEST_CASE ( "intersection(Line, Segment)", "[intersections]" ) {
	Geometry::Segment seg(null_point, {1, 0, 0});
	SECTION ( "line is parallel to segment" ) {
		Geometry::Line line({1, 1, 1}, {2, 1, 1});
		REQUIRE(std::holds_alternative<Geometry::EmptySet>(intersection(line, seg)));
	}
	SECTION ( "line contains segment" ) {
		Geometry::Line line({-10, 0, 0}, {10, 0, 0});
		auto intrsctn = intersection(line, seg);
		REQUIRE(std::holds_alternative<Geometry::Segment>(intrsctn));
		REQUIRE(std::get<Geometry::Segment>(intrsctn) == seg);
	}
	SECTION ( "one common point test 1" ) {
		Geometry::Line line({0.5, -1, -1}, {0.5, 1, 1});
		auto intrsctn = intersection(line, seg);
		REQUIRE(std::holds_alternative<Geometry::Point>(intrsctn));
		REQUIRE(std::get<Geometry::Point>(intrsctn) == Geometry::Point(0.5, 0, 0));
	}
	SECTION ( "one common point test 2" ) {
		Geometry::Line line({0, 0, 0}, {1, 0, 2});
		Geometry::Segment seg({0, 0, 1}, {1, 0, 0});
		auto intrsctn = intersection(line, seg);
		REQUIRE(std::holds_alternative<Geometry::Point>(intrsctn));
		REQUIRE(std::get<Geometry::Point>(intrsctn) == Geometry::Point(1.0/3, 0.0, 2.0/3));
	}
}

TEST_CASE ( "intersection(Line, Line)", "[intersections]" ) {
	SECTION ( "parallel lines" ) {
		REQUIRE(std::holds_alternative<Geometry::EmptySet>(intersection(
			Geometry::Line({0, 0, 0}, {1, 0, 0}),
			Geometry::Line({0, 0, 1}, {1, 0, 1}))));
	}
	SECTION ( "crossed lines (not intersected)" ) {
		REQUIRE(std::holds_alternative<Geometry::EmptySet>(intersection(
			Geometry::Line({1, 2, 3}, {4, 5, 6}),
			Geometry::Line({7, 3, 5}, {4, 2, 1}))));	
	}
	SECTION ( "intersected lines" ) {
		auto intrsctn = Geometry::intersection(
			Geometry::Line({0, 0, 1}, {1, 1, 1}),
			Geometry::Line({1, 0, 1}, {0, 1, 1}));
		REQUIRE(std::holds_alternative<Geometry::Point>(intrsctn));
		REQUIRE(std::get<Geometry::Point>(intrsctn) == Geometry::Point(0.5, 0.5, 1));
	}
	SECTION ( "the same line" ) {
		Geometry::Line line(null_point, {1, 0, 0});
		REQUIRE(std::holds_alternative<Geometry::Line>(intersection(line, line)));
		REQUIRE(std::holds_alternative<Geometry::Line>(intersection(
			line, Geometry::Line({0.5, 0, 0}, {1.5, 0, 0}))));
	}
}
