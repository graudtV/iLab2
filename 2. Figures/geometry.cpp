#include "geometry.h"
#include <vector>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <sstream>

namespace Geometry {

/*  Accuracy of current Vector::mixed_product implementation is about 0.0001f
 * So float_tolerance should have reserve, or combinations
 * of basic Geometry operations will not work properly
 * float_tolerance = 0.01f seems to be good */
float Float::float_tolerance = 0.001;
const Point Point::null_point(0, 0, 0);
const Vector Vector::null_vector(0, 0, 0);
const Plane Plane::oxy({0, 0, 0}, {1, 0, 0}, {0, 1, 0});
const Plane Plane::oxz({0, 0, 0}, {1, 0, 0}, {0, 0, 1});
const Plane Plane::oyz({0, 0, 0}, {0, 1, 0}, {0, 0, 1});


Point operator +(const Point& pnt, const Vector& vec)
	{ return Point(pnt.x + vec.x, pnt.y + vec.y, pnt.z + vec.z); }

/*  Points are equal if their coordinates are equal. Coordinates
 * are being compared by Geometry::Float rules */
bool operator ==(const Point& fst, const Point& snd)
	{ return fst.x == snd.x && fst.y == snd.y && fst.z == snd.z; }

std::ostream& operator <<(std::ostream& os, const Point& pnt)
	{ return os << "(" << pnt.x << "; " << pnt.y << "; " << pnt.z << ")"; }

std::array<Float, 2>
Vector::decompose(const Vector& fst, const Vector& snd) const
{
	Float a1 = Vector::inner_product(fst, fst);
	Float b1 = Vector::inner_product(fst, snd);
	Float b2 = Vector::inner_product(snd, snd);
	Float c1 = Vector::inner_product(*this, fst);
	Float c2 = Vector::inner_product(*this, snd);
	Float denominator = a1 * b2 - b1 * b1; // != 0 if (fst, snd) is basis
	Float delta1 = c1 * b2 - b1 * c2;
	Float delta2 = a1 * c2 - c1 * b1;
	return { delta1 / denominator, delta2 / denominator };
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
		&& Vector::outer_product(Vector(a, b), Vector(a, c)) != Vector::null_vector;
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

bool operator ==(const Segment& fst, const Segment& snd)
{
	if (fst.a == snd.a)
		return fst.b == snd.b;
	return fst.a == snd.b && fst.b == snd.a;
}

bool Plane::valid() const
{
	return a.valid() && b.valid() && c.valid()
		&& a != b && b != c && a != c;
}

bool Plane::contains(const Line& line) const
{
	Vector p(a, b);
	Vector q(a, c);
	return (Vector::mixed_product(p, q, Vector(line.a, line.b)) == 0)
		&& (Vector::mixed_product(p, q, Vector(a, line.a)) == 0);
}

std::ostream& operator <<(std::ostream& os, const Plane& plane)
	{ return os << "{" << plane.a << ", " << plane.b << ", " << plane.c << "}"; }

Vector vdistance(const Point& fst, const Point& snd)
	{ return Vector(snd.x - fst.x, snd.y - fst.y, snd.z - fst.z); }

Vector vdistance(const Point& pnt, const Plane& plane)
{
	Vector l = Vector(pnt, plane.a);
	Vector n = plane.unit_normal();
	return Vector::inner_product(l, n) * n;
}

std::variant<EmptySet, Line, Plane>
intersection(const Plane& fst, const Plane& snd)
{
	/* Algorithm taken from Geometric Tools for Computer Graphics section 11.5 */
	Vector n1 = fst.normal();
	Vector n2 = snd.normal();
	Vector l = Vector::outer_product(n1, n2); // line direction vector (направляющий вектор прямой)
	if (l == Vector::null_vector) {
		if (Vector::inner_product(Vector(fst.a, snd.a), n1) == 0) // planes coincide
			return Plane(fst);
		return EmptySet();
	}
	/* s1, s2 - numbers from plane equation in form (r, n) = s */
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
intersection(const Line& line, const Triangle& trg)
{
	Vector p = Vector(trg.a, trg.b);
	Vector q = Vector(trg.a, trg.c);
	Vector delta = Vector(trg.a, line.a); // Vector from triangle vertex to point on a plane
	Vector linedir = Vector(line.a, line.b);
	Float denominator = Vector::mixed_product(linedir, q, p);
	
	if (denominator == 0) { // Line is parallel to triangle's plane or lies in it
		if (Vector::mixed_product(delta, p, q) == 0) { // The line is in triangle's plane
			Segment trg_sides[] = {{trg.a, trg.b}, {trg.b, trg.c}, {trg.c, trg.a}};
			std::vector<Point> intrsctn_points;

			for (auto& side: trg_sides) {
				auto intrsctn = intersection(line, side);
				if (std::holds_alternative<Segment>(intrsctn))
					return side; // intersection is a whole triangle side
				else if (std::holds_alternative<Point>(intrsctn))
					intrsctn_points.push_back(std::get<Point>(intrsctn));
			}

			/*  If line intersects triangle in a vertex, there could be
			 * 3 intersection points. Moreover, because of accuracy issues,
			 * they may all be different! To solve with, points are
			 * sorted by x coordinate, then obvious dublicates are erased with
			 * std::unique. If there are still 3 points, the second one is
			 * ignored, and first and last are taken as segment ends.
			 */
			assert(intrsctn_points.size() <= 3); // I have never seen a situation if there are more than 3 points
			std::sort(intrsctn_points.begin(), intrsctn_points.end(),
				[](const Point& fst, const Point& snd) { return std::less<float>()(fst.x, snd.x); });
			intrsctn_points.erase(
				std::unique(intrsctn_points.begin(), intrsctn_points.end()),
				intrsctn_points.end());

			if (intrsctn_points.size() == 1)
				return intrsctn_points.front();
			if (intrsctn_points.size() == 2)
				return Segment(intrsctn_points.front(), intrsctn_points.back());
		}
		/* Line lies in another plane or in the same one, but without any intersection points */
		return EmptySet();
	}
	/* Line intersects triangle's plane in some point */
	/* This part of algorithm is taken from Geometric Tools for Computer
	 *  Graphics section 11.2 */
	/* u, v, w - balicentric coordinates
	 * t - coeffs in line equation in form r = line.a + t * linedir */
	Float t = Vector::mixed_product(delta, p, q) / denominator;
	Float u = Vector::mixed_product(linedir, q, delta) / denominator;
	Float v = Vector::mixed_product(delta, p, linedir) / denominator;
	Float w = 1 - u - v;
	if (0 <= u && u <= 1 && 0 <= v && v <= 1 && 0 <= w && w <= 1) // Пересечение есть
		return Point(Vector(Point::null_point, line.a) + t * linedir);
	return EmptySet();
}

std::variant<EmptySet, Point, Segment>
intersection(const Segment& seg, const Triangle& trg)
{
	auto intrsctn = intersection(Line(seg), trg);
	if (std::holds_alternative<EmptySet>(intrsctn))
		return EmptySet();
	if (std::holds_alternative<Point>(intrsctn)) {
		Point pnt = std::get<Point>(intrsctn);
		if (seg.contains(pnt))
			return pnt;
		return EmptySet();
	}
	/* intrsctn == Segment */
	return intersection(std::get<Segment>(intrsctn), seg);
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

	/*  Both segments are on one line */
	/*  Decomposing vectors from line origin to every of 4 ends of segments
	 * into basis of line's direction vector */
	Line line = std::get<Line>(lines_intersection);
	Vector linedir = line.direction();
	Point line_origin = line.a;

	Float a1 = Vector(line_origin, fst.a).decompose(linedir);
	Float a2 = Vector(line_origin, fst.b).decompose(linedir);
	Float b1 = Vector(line_origin, snd.a).decompose(linedir);
	Float b2 = Vector(line_origin, snd.b).decompose(linedir);

	/* Problem is reduced to the same problem, but on line
	 * a1,...,a4 - coordinates on a line */
	if (a1 > a2)
		std::swap(a1, a2);
	if (b1 > b2)
		std::swap(b1, b2);
	if (a1 > b1) {
		std::swap(a1, b1);
		std::swap(a2, b2);
	}

	/* Now a1 <= a2, a1 <= b1 <= b2;
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
	Vector d = Vector(fst.a, snd.a); // from point on the line to point of another one
	if (Vector::mixed_product(line1_dir, line2_dir, d) != 0)
		return EmptySet(); // Crossing lines (скрещивающиеся прямые)
	if (Vector::collinear(line1_dir, line2_dir)) {
		if (Vector::collinear(d, line1_dir))
			return fst; // <=> snd - coincident lines
		return EmptySet();
	}
	auto koeffs = d.decompose(line1_dir, line2_dir);
	return fst.a + koeffs[0] * line1_dir; // Point
}

std::variant<EmptySet, Point>
intersection(const Point& fst, const Point& snd)
{
	if (fst == snd)
		return fst; // fst == snd
	return EmptySet();
}

bool intersected_impl(const Triangle& fst, const Triangle& snd)
{
	/*  Просто и надежно, легко справляется со случаями, когда оба
	 * треугольника лежат в одной плоскости.
	 *  (Но в Geometric Tools for Computer Graphics раздел 11.5.4
	 * утверждается, что алгоритм через пересечение плоскостей треугольников
	 * быстрее. (Хотя альтернативный алгоритм вызывает почти те же методы,
	 * что и тут (потребуется пересекать линию и треугольник, что крайне
	 * мало отличается от пересечения отрезка и треугольника),
	 * + кучу дополнительных методов). Так же в том алгоритме тяжело
	 * обрабатывать случаи, когда 2 треугольника лежат в одной плоскости,
	 * потому что приходится решать задачу о пересечении двух плоскостных
	 * треугольников. Взамен, тот алгоритм позволяет получить непосредственно
	 * фигуру пересечения) */
	return intersected(Segment(snd.a, snd.b), fst)
		|| intersected(Segment(snd.b, snd.c), fst)
		|| intersected(Segment(snd.a, snd.c), fst)
		|| intersected(Segment(fst.a, fst.b), snd)
		|| intersected(Segment(fst.b, fst.c), snd)
		|| intersected(Segment(fst.a, fst.c), snd);
}

} // Geometry namespace end