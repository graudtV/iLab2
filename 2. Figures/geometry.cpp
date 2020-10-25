#include "geometry.h"
#include <cmath>
#include <sstream>

namespace Geometry {

float Float::float_tolerance = 0.01;
const Point Point::null_point(0, 0, 0);
const Vector Vector::null_vector(0, 0, 0);
const Plane Plane::oxy({0, 0, 0}, {1, 0, 0}, {0, 1, 0});
const Plane Plane::oxz({0, 0, 0}, {1, 0, 0}, {0, 0, 1});
const Plane Plane::oyz({0, 0, 0}, {0, 1, 0}, {0, 0, 1});

Point::Point(const Vector& vec) :
	x(vec.x), y(vec.y), z(vec.z) {}

bool Point::valid() const
	{ return x.valid() && y.valid() && z.valid(); }

Point operator +(const Point& pnt, const Vector& vec)
	{ return Point(pnt.x + vec.x, pnt.y + vec.y, pnt.z + vec.z); }

/*  Точки равны, если их координаты равны. Координаты сравниваются
 * по правилам класса Geometry::Float */
bool operator ==(const Point& fst, const Point& snd)
	{ return fst.x == snd.x && fst.y == snd.y && fst.z == snd.z; }

std::ostream& operator <<(std::ostream& os, const Point& pnt)
	{ return os << "(" << pnt.x << "; " << pnt.y << "; " << pnt.z << ")"; }

bool Vector::valid() const
	{ return Point(x, y, z).valid(); }

Float Vector::decompose(const Vector& basis) const
{
	if (!collinear(*this, basis) || basis == Vector::null_vector)
		throw DecompositionError();
	return x/basis.x;
}

std::array<Float, 2>
Vector::decompose(const Vector& fst, const Vector& snd) const
{
	if (Vector::mixed_product(*this, fst, snd) != 0
		|| Vector::outer_product(fst, snd) == Vector::null_vector)
		throw DecompositionError();
	Float denominator = fst.x*snd.y-fst.y*snd.x;
	Float alpha = (snd.y*x - y*snd.x) / denominator;
	Float betta = (fst.x*y - x*fst.y) / denominator;
	return {alpha, betta};
}

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

bool Vector::collinear(const Vector& fst, const Vector& snd)
	{ return Vector::outer_product(fst, snd) == Vector::null_vector; }

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

bool Segment::valid() const
	{ return a.valid() && b.valid() && (a != b); }

bool Segment::contains(const Point& pnt) const
{
	auto vec1 = vdistance(a, pnt);
	auto vec2 = vdistance(b, pnt);
	return (Vector::inner_product(vec1, vec2) <= 0
		&& Vector::outer_product(vec1, vec2) == Vector::null_vector);
}

bool Line::contains(const Point& pnt) const
{
	auto vec1 = vdistance(a, pnt);
	auto vec2 = vdistance(b, pnt);
	return (Vector::outer_product(vec1, vec2) == Vector::null_vector);
}

bool Line::valid() const
	{ return a.valid() && b.valid() && (a != b); }

bool operator ==(const Line& fst, const Line& snd)
	{ return (fst.contains(snd.a) && fst.contains(snd.b)); }

std::ostream& operator <<(std::ostream& os, const Line& line)
	{ return os << "<" << line.a << " - " << line.b << ">"; }

std::ostream& operator <<(std::ostream& os, const Segment& seg)
	{ return os << "[" << seg.a << "; " << seg.b << "]"; }

bool Plane::valid() const
{
	return a.valid() && b.valid() && c.valid()
		&& a != b && b != c && a != c;
}

Vector Plane::normal() const
	{ return Vector::outer_product(Vector(a, b), Vector(a, c)); }

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

std::variant<EmptySet, Line, Plane>
intersection(const Plane& fst, const Plane& snd)
{
	/* Алгоритм взят из Geometric Tools for Computer Graphics раздел 11.5 */
	Vector n1 = fst.normal();
	Vector n2 = snd.normal();
	Vector l = Vector::outer_product(n1, n2); // Направляющий вектор прямой
	if (l == Vector::null_vector) {
		if (Vector::inner_product(Vector(fst.a, snd.a), n1) == 0) // плоскости совпадают
			return Plane(fst);
		return EmptySet();
	}
	/* s1, s2 - числа в уравнении плоскостей в виде (r, n) = s */
	Float s1 = Vector::inner_product(Vector(Point::null_point, fst.a), n1);
	Float s2 = Vector::inner_product(Vector(Point::null_point, snd.a), n2);
	Float n1n2 = Vector::inner_product(n1, n2);
	Float n1n1 = Vector::inner_product(n1, n1);
	Float n2n2 = Vector::inner_product(n2, n2);
	Float denominator = n1n2 * n1n2 - n1n1 * n2n2;

	Float a = (s2 * n1n2 - s1 * n2n2) / denominator;
	Float b = (s1 * n1n2 - s2 * n1n1) / denominator;
	Point p1 = Point(a * n1 + b * n2);
	Point p2 = p1 + l;
	return Line(p1, p2);
}

std::variant<EmptySet, Point, Segment>
intersection(const Line& line, const Triangle& trg) // not implemented
{
	// std::variant<EmptySet, Point, Segment> intrscts[] = {
	// 	intersection(line, Segment(trg.a, trg.b)),
	// 	intersection(line, Segment(trg.b, trg.c)),
	// 	intersection(line, Segment(trg.c, trg.a)),
	// };
	// for (auto& intrsct : intrscts)
	// 	if (std::holds_alternative<Segment>(intrsct))
	// 		return std::get<Segment>(intrsct);

	// std::vector<Point> points;
	// for (auto& intrsct : intrscts)
	// 	if (std::holds_alternative<Point>(intrsct))
	// 		points.push_back(std::get<Point>(intrsct));
	// auto it_last = delete_repeats(points);
	return EmptySet();
}

std::variant<EmptySet, Point, Segment>
intersection(const Segment& fst, const Segment& snd)
{
	auto lines_intersection = intersection(Line(fst), Line(snd));
	if (std::holds_alternative<Point>(lines_intersection)) {
		Point pnt = std::get<Point>(lines_intersection);
		if (fst.contains(pnt) && snd.contains(pnt))
			return pnt;
		return EmptySet();
	}
	if (std::holds_alternative<EmptySet>(lines_intersection))
		return EmptySet();

	/* Два отрезка лежат на одной прямой */
	/*  Раскладываем вектора из начальной точки прямой в каждый из концов
	 * отрезка по базису из направляющего вектора прямой */
	Line line = std::get<Line>(lines_intersection);
	Vector linedir = line.direction();
	Point line_origin = line.a;
	Float a1 = Vector(line_origin, fst.a).decompose(linedir);
	Float a2 = Vector(line_origin, fst.b).decompose(linedir);
	Float b1 = Vector(line_origin, snd.a).decompose(linedir);
	Float b2 = Vector(line_origin, snd.b).decompose(linedir);

	/* Задача свелась к задаче на прямой. a1,...,a4 - координаты на прямой */
	if (a1 > a2)
		std::swap(a1, a2);
	if (b1 > b2)
		std::swap(b1, b2);
	if (a1 > b1) {
		std::swap(a1, b1);
		std::swap(a2, b2);
	}
	/* Итого a1 <= a2, a1 <= b1 <= b2;
	 * case1: a1 <= a2 <= b1 <= b2
	 * case2: a1 <= b1 <= a2 <= b2
	 * case3: a1 <= b1 <= b2 <= a2 */
	if (a2 < b1)
		return EmptySet();
	if (a2 == b1)
		return Point(line_origin + a2*linedir);
	if (a2 <= b2)
		return Segment(line_origin + b1*linedir, line_origin + a2*linedir);
	return Segment(line_origin + b1*linedir, line_origin + b2*linedir);
}

std::variant<EmptySet, Point, Segment>
intersection(const Line& line, const Segment& seg)
{
	auto lines_intersection = intersection(line, Line(seg));
	if (std::holds_alternative<EmptySet>(lines_intersection))
		return EmptySet();
	if (std::holds_alternative<Line>(lines_intersection))
		return seg;
	Point pnt = std::get<Point>(lines_intersection);
	if (seg.contains(pnt))
		return pnt;
	return EmptySet();
}

std::variant<EmptySet, Point, Line>
intersection(const Line& fst, const Line& snd)
{
	Vector line1_dir = fst.direction();
	Vector line2_dir = snd.direction();
	Vector d = Vector(fst.a, snd.a); // Из точки одной прямой в точку другой
	if (Vector::mixed_product(line1_dir, line2_dir, d) != 0)
		return EmptySet(); // Скрещивающиеся прямые
	if (Vector::collinear(line1_dir, line2_dir)) {
		if (Vector::collinear(d, line1_dir))
			return fst; // <=> snd - прямые совпадают
		return EmptySet();
	}
	auto koeffs = d.decompose(line1_dir, line2_dir);
	return fst.a + koeffs[0] * line1_dir; // Point
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
	Segment s1 = std::get<Segment>(intersection(line, fst));
	Segment s2 = std::get<Segment>(intersection(line, snd));

	auto segment_intersection = intersection(s1, s2);
	return (std::holds_alternative<EmptySet>(segment_intersection))
		? false : true;

	} catch(std::bad_variant_access&) {
		std::ostringstream err_msg;
		err_msg << "Cannot understand if triangles " << fst
			<< " and " << snd << " are intersected: too hard case";
		throw std::runtime_error(err_msg.str());
	}
}

} // Geometry namespace end