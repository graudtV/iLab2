#include "geometry.h"
#include <cmath>

namespace Geometry {

float Float::float_tolerance = 0.01;
const Point Point::null_point(0, 0, 0);
const Vector Vector::null_vector(0, 0, 0);
const Plane Plane::oxy({0, 0, 0}, {1, 0, 0}, {0, 1, 0});
const Plane Plane::oxz({0, 0, 0}, {1, 0, 0}, {0, 0, 1});
const Plane Plane::oyz({0, 0, 0}, {0, 1, 0}, {0, 0, 1});

bool Point::valid() const
{ return x.valid() && y.valid() && z.valid(); }

/*  Точки равны, если их координаты равны. Координаты сравниваются
 * по правилам класса Geometry::Float */
bool operator ==(const Point& fst, const Point& snd)
	{ return fst.x == snd.x && fst.y == snd.y && fst.z == snd.z; }

std::ostream& operator <<(std::ostream& os, const Point& pnt)
	{ return os << "(" << pnt.x << "; " << pnt.y << "; " << pnt.z << ")"; }

bool Vector::valid() const
	{ return Point(x, y, z).valid(); }

bool operator ==(const Vector& fst, const Vector& snd)
	{ return Point(fst.x, fst.y, fst.z) == Point(snd.x, snd.y, snd.z); }
Vector operator +(const Vector& fst, const Vector& snd)
	{ return Vector(fst.x + snd.x, fst.y + snd.y, fst.z + snd.z); }
Vector operator -(const Vector& fst, const Vector& snd)
	{ return Vector(fst.x - snd.x, fst.y - snd.y, fst.z - snd.z); }
Vector operator *(const Vector& vec, Float value)
	{ return Vector(vec.x * value, vec.y * value, vec.z * value); }
Vector operator *(Float value, const Vector& vec)
	{ return vec * value; }


Vector Vector::outer_product(const Vector& fst, const Vector& snd)
{
	return Vector (
		fst.y * snd.z - fst.z * snd.y,
		fst.z * snd.x - fst.x * snd.z,
		fst.x * snd.y - fst.y * snd.x
		);
}

Float Vector::mixed_product(const Vector& fst,
	const Vector& snd, const Vector& thd)
{
	return fst.x * (snd.y * thd.z - snd.z * thd.y)
	     + fst.y * (snd.z * thd.x - snd.x * thd.z)
	     + fst.z * (snd.x * thd.y - snd.y * thd.x);
}

std::ostream& operator <<(std::ostream& os, const Vector& vec)
	{ return os << "(" << vec.x << "; " << vec.y << "; " << vec.z << ")"; }

bool Triangle::valid() const
{
	return a.valid() && b.valid() && c.valid()
		&& a != b && b != c && a != c;
}

std::ostream& operator <<(std::ostream& os, const Triangle& trg)
{
	return os << "<" << trg.a << ", " << trg.b << ", " << trg.c << ">";
}

bool Line::valid() const
	{ return a.valid() && b.valid() && (a != b); }

std::ostream& operator <<(std::ostream& os, const Line& line)
	{ return os << "<" << line.a << " - " << line.b << ">"; }

bool Segment::valid() const
	{ return a.valid() && b.valid() && (a != b); }

std::ostream& operator <<(std::ostream& os, const Segment& seg)
	{ return os << "[" << seg.a << "; " << seg.b << "]"; }

bool Plane::valid() const
{
	return a.valid() && b.valid() && c.valid()
		&& a != b && b != c && a != c;
}

std::ostream& operator <<(std::ostream& os, const Plane& plane)
	{ return os << "{" << plane.a << ", " << plane.b << ", " << plane.c << "}"; }

Vector vdistance(const Point& fst, const Point& snd)
	{ return Vector(snd.x - fst.x, snd.y - fst.y, snd.z - fst.z); }

Vector vdistance(const Point& pnt, const Plane& plane)
{
	Vector a = Vector(plane.a, plane.b);
	Vector b = Vector(plane.a, plane.c);
	Vector n = Vector::outer_product(a, b);
	Vector l = Vector(pnt, plane.a);

	return Vector::mixed_product(a, b, l) / Vector::mixed_product(a, b, n) * n;
}

bool intersected(const Triangle& fst, const Triangle& snd)
{
	Plane plane(fst);
	Vector r1 = vdistance(snd.a, plane);
	Vector r2 = vdistance(snd.b, plane);
	Vector r3 = vdistance(snd.c, plane);
	if (Vector::inner_product(r1, r2) > 0 && Vector::inner_product(r2, r3) > 0)
		return false; // Все вершины второго треугольника расположены по одну сторону от плоскости первого
	Plane plane2(snd);

	try {

	auto line = std::get<Line>(intersection(plane, plane2));
	auto intersection1 = std::get<FinitePointSet>(intersection(line, fst));
	auto intersection2 = std::get<FinitePointSet>(intersection(line, snd));
	if (intersection1.size() != 2 || intersection2.size() != 2)
		return false;
	Segment s1(intersection1.front(), intersection1.back());
	Segment s2(intersection2.front(), intersection2.back());
	auto segment_intersection = intersection(s1, s2);
	return (std::holds_alternative<EmptySet>(segment_intersection))
		? false : true;

	} catch(std::bad_variant_access&) {
		std::cerr << "Cannot understand if triangles " << fst
			<< " and " << snd << " intersected: too hard case. Returning false";
		return false;
	}
}

} // Geometry namespace end