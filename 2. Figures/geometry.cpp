#include "geometry.h"
#include <cmath>
#include <sstream>
#include <algorithm>
#include <cassert>

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
	return Vector::inner_product(*this, basis) /
		Vector::inner_product(basis, basis);
}

std::array<Float, 2>
Vector::decompose(const Vector& fst, const Vector& snd) const
{
	if (Vector::mixed_product(*this, fst, snd) != 0
		|| Vector::outer_product(fst, snd) == Vector::null_vector)
		throw DecompositionError();
	Float alpha = 0, betta = 0;
	Float denominator = 0;
	if ((denominator = fst.x*snd.y - snd.x*fst.y) != 0) {
		alpha = x*snd.y - snd.x*y;
		betta = fst.x*y - x*fst.y;
	} else if ((denominator = fst.y*snd.z - snd.y*fst.z) != 0) {
		alpha = y*snd.z - snd.y*z;
		betta = fst.y*z - y*fst.z;
	} else {
		denominator = fst.x*snd.z - snd.x*fst.z;
		assert(denominator != 0);
		alpha = x*snd.z - snd.x*z;
		betta = fst.x*z - x*fst.z;
	}
	return {alpha / denominator, betta / denominator};
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
		&& Vector::outer_product(Vector(a, b), Vector(a, c)) != Vector::null_vector
		&& b != c;
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

	return Vector::mixed_product(a, b, l) / Vector::inner_product(n, n) * n;
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
intersection(const Line& line, const Triangle& trg)
{
	//std::cout << ">::: intersection(line, trg): " << line << " and " << trg << std::endl;
	Vector p = Vector(trg.a, trg.b);
	Vector q = Vector(trg.a, trg.c);
	Vector delta = Vector(trg.a, line.a); // Вектор из точки треугольника в точку на прямой 
	Vector linedir = Vector(line.a, line.b);
	Float denominator = Vector::mixed_product(linedir, q, p);
	
	//std::cout << ">::: denom = " << denominator << std::endl;
	if (denominator == 0) { // Прямая параллельна плоскости треугольника или лежит в ней
		//std::cout << ">::: (delta, p, q) = " << Vector::mixed_product(delta, p, q) << std::endl;
		if (Vector::mixed_product(delta, p, q) == 0) { // Прямая лежит в плоскости треугольника
			Segment trg_sides[] = {{trg.a, trg.b}, {trg.b, trg.c}, {trg.c, trg.a}};
			std::vector<Point> intrsctn_points;

			for (auto& side: trg_sides) {
				auto intrsctn = intersection(line, side);
				//std::cout << ">::: variant index = " << intrsctn.index() << std::endl;
				if (std::holds_alternative<Segment>(intrsctn))
					return side; // пересечение по целой стороне
				else if (std::holds_alternative<Point>(intrsctn)) {
					Point pnt = std::get<Point>(intrsctn);
					if (std::find(intrsctn_points.begin(), intrsctn_points.end(), pnt)
						== intrsctn_points.end())
						intrsctn_points.push_back(pnt);
				}
			}
			assert(intrsctn_points.size() <= 2);
			if (intrsctn_points.size() == 1)
				return intrsctn_points.front();
			if (intrsctn_points.size() == 2)
				return Segment(intrsctn_points.front(), intrsctn_points.back());
		}
		/* Прямая лежит в другой плоскости или в той же, но точек пересечения нет */
		return EmptySet();
	}
	/* Прямая пересекает плоскость треугольника в некоторой точке */
	/* Эта часть алгоритма взята из Geometric Tools for Computer Graphics раздел 11.2 */
	/* u, v, w - балицентрические координаты
	 * t - коэфф. в уравнении прямой в виде r = line.a + t * linedir */
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
	//std::cout << ">::: intersection(line, seg): " << line << " and " << seg << std::endl;
	auto lines_intersection = intersection(line, Line(seg));
	//std::cout << ">::: - idx = " << lines_intersection.index() << std::endl;
	if (std::holds_alternative<EmptySet>(lines_intersection))
		return EmptySet();
	if (std::holds_alternative<Line>(lines_intersection))
		return seg;
	Point pnt = std::get<Point>(lines_intersection);
	//std::cout << ">::: - pnt = " << pnt << std::endl;
	if (seg.contains(pnt)) {
	//	std::cout << ">::: -contains\n";
		return pnt;
	}
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