/*
 * geomerty.h - declarations of all Geometry types and methods
 */

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <iostream>
#include <cmath>
#include <array>
#include <vector>
#include <set>
#include <variant>
#include <iterator>

namespace Geometry {

/* Compares float according to float_tolerance */
class Float final {
public:
	Float() {}
	Float(float val) : m_val(val) {}
	operator float() const { return m_val; }
	Float& operator +=(Float other) { m_val += other; return *this; }
	Float& operator -=(Float other) { m_val -= other; return *this; }
	Float& operator *=(Float other) { m_val *= other; return *this; }
	Float& operator /=(Float other) { m_val /= other; return *this; }

	friend Float operator +(Float fst, Float snd) { return fst.m_val + snd.m_val; }
	friend Float operator -(Float fst, Float snd) { return fst.m_val - snd.m_val; }
	friend Float operator *(Float fst, Float snd) { return fst.m_val * snd.m_val; }
	friend Float operator /(Float fst, Float snd) { return fst.m_val / snd.m_val; }

	friend bool operator ==(Float fst, Float snd) { return eq(fst, snd); }
	friend bool operator < (Float fst, Float snd) { return lower(fst, snd); }
	friend bool operator <=(Float fst, Float snd) { return lowereq(fst, snd); }
	friend bool operator !=(Float fst, Float snd) { return noteq(fst, snd); }
	friend bool operator > (Float fst, Float snd) { return greater(fst, snd); }
	friend bool operator >=(Float fst, Float snd) { return greatereq(fst, snd); }
	
	template <class T> friend Float operator +(Float fst, const T& snd) { return float(fst) + float(snd); }
	template <class T> friend Float operator -(Float fst, const T& snd) { return float(fst) - float(snd); }
	template <class T> friend Float operator *(Float fst, const T& snd) { return float(fst) * float(snd); }
	template <class T> friend Float operator /(Float fst, const T& snd) { return float(fst) / float(snd); }

	template <class T> friend Float operator +(const T& fst, Float snd) { return float(fst) + float(snd); }
	template <class T> friend Float operator -(const T& fst, Float snd) { return float(fst) - float(snd); }
	template <class T> friend Float operator *(const T& fst, Float snd) { return float(fst) * float(snd); }
	template <class T> friend Float operator /(const T& fst, Float snd) { return float(fst) / float(snd); }

	template <class T> friend bool operator ==(Float fst, const T& snd) { return fst == Float(snd); }
	template <class T> friend bool operator !=(Float fst, const T& snd) { return fst != Float(snd); }
	template <class T> friend bool operator < (Float fst, const T& snd) { return fst < Float(snd); }
	template <class T> friend bool operator <=(Float fst, const T& snd) { return fst <= Float(snd); }
	template <class T> friend bool operator > (Float fst, const T& snd) { return fst > Float(snd); }
	template <class T> friend bool operator >=(Float fst, const T& snd) { return fst >= Float(snd); }

	template <class T> friend bool operator ==(const T& fst, Float snd) { return snd == fst; }
	template <class T> friend bool operator !=(const T& fst, Float snd) { return snd != fst; }
	template <class T> friend bool operator < (const T& fst, Float snd) { return snd > fst; }
	template <class T> friend bool operator <=(const T& fst, Float snd) { return snd >= fst; }
	template <class T> friend bool operator > (const T& fst, Float snd) { return snd < fst; }
	template <class T> friend bool operator >=(const T& fst, Float snd) { return snd >= fst; }

	bool valid() const { return std::isnormal(m_val) || m_val == 0; }

	static bool eq			(Float fst, Float snd, Float tolerance = float_tolerance) { return fabs(fst.m_val - snd.m_val) <= float(tolerance); }
	static bool lower		(Float fst, Float snd, Float tolerance = float_tolerance) { return fst.m_val < snd.m_val - float(tolerance); }
	static bool lowereq		(Float fst, Float snd, Float tolerance = float_tolerance) { return fst.m_val <= snd.m_val + float(tolerance); }
	static bool noteq		(Float fst, Float snd, Float tolerance = float_tolerance) { return !eq(fst, snd, tolerance); }
	static bool greater		(Float fst, Float snd, Float tolerance = float_tolerance) { return lower(snd, fst, tolerance); }
	static bool greatereq	(Float fst, Float snd, Float tolerance = float_tolerance) { return lowereq(snd, fst, tolerance); }

	static float float_tolerance;
private:
	float m_val;

	friend std::istream& operator >>(std::istream& is, Float& f)
		{ return is >> f.m_val; }
};


struct Vector;

struct Point {
	Float x, y, z;

	Point() : x(0), y(0), z(0) {}
	Point(Float xx, Float yy, Float zz) : x(xx), y(yy), z(zz) {}
	explicit Point(const Vector& vec);
	virtual ~Point() {}

	bool valid() const
		{ return x.valid() && y.valid() && z.valid(); }

	static const Point null_point;
};

std::ostream& operator <<(std::ostream& os, const Point& pnt);
bool operator ==(const Point& fst, const Point& snd);
inline bool operator !=(const Point& fst, const Point& snd)
	{ return !(fst == snd); }
Point operator +(const Point& pnt, const Vector& vec);


struct Vector {
	Float x, y, z;

	Vector() : x(0), y(0), z(0) {}
	Vector(Float xx, Float yy, Float zz) : x(xx), y(yy), z(zz) {}
	Vector(const Point& fst, const Point& snd) :
		x(snd.x - fst.x), y(snd.y - fst.y), z(snd.z - fst.z) {}
	virtual ~Vector() {}

	bool valid() const
		{ return Point(x, y, z).valid(); }
	
	Float module() const
		{ return std::sqrt(x*x + y*y + z*z); }

	/*  Decompostion into linear basis. If vector cannot be decomposed, decomposition
	 * of projection will be returned. If basis == null_vector, behaviour is unndefined */
 	Float decompose(const Vector& basis) const
 		{ return Vector::inner_product(*this, basis) / Vector::inner_product(basis, basis); }

	/*  Decompostion into planar basis. If vector cannot be decomposed, decomposition
	 * of projection will be returned. If (fst, snd) is not a basis, behaviour is undefined */
	std::array<Float, 2> decompose(const Vector& fst, const Vector& snd) const;

	Vector operator /(Float value) const
		{ return Vector(x / value, y / value, z / value); }

	static Float inner_product(const Vector& fst, const Vector& snd)
		{ return fst.x * snd.x + fst.y * snd.y + fst.z * snd.z; }
	static Vector outer_product(const Vector& fst, const Vector& snd)
		{ return Vector (fst.y * snd.z - fst.z * snd.y, fst.z * snd.x - fst.x * snd.z, fst.x * snd.y - fst.y * snd.x); }
	static Float mixed_product(const Vector& fst, const Vector& snd, const Vector& thd)
		{ return Vector::inner_product(fst, Vector::outer_product(snd, thd)); }
	static Vector unit(const Vector& vec)
		{ return vec / vec.module(); }
	static bool collinear(const Vector& fst, const Vector& snd);

	static const Vector null_vector;
};

inline Point::Point(const Vector& vec) :
	x(vec.x), y(vec.y), z(vec.z) {}
std::ostream& operator <<(std::ostream& os, const Vector& vec);
bool operator ==(const Vector& fst, const Vector& snd);
inline bool operator !=(const Vector& fst, const Vector& snd)
	{ return !(fst == snd); }
Vector operator +(const Vector& fst, const Vector& snd);
Vector operator -(const Vector& fst, const Vector& snd);
Vector operator *(const Vector& vec, Float value);
Vector operator *(Float value, const Vector& vec);


struct Triangle {
	Point a, b, c;

	Triangle(const Point& aa, const Point& bb, const Point& cc) :
		a(aa), b(bb), c(cc) {}
	virtual ~Triangle() {}

	bool valid() const;
};

std::ostream& operator <<(std::ostream& os, const Triangle& trg);


/* Отрезок */
struct Segment {
	Point a, b;

	Segment(const Point& aa, const Point& bb) : a(aa), b(bb) {}
	virtual ~Segment() {}

	bool valid() const;
	bool contains(const Point& pnt) const;
};

std::ostream& operator <<(std::ostream& os, const Segment& seg);
bool operator ==(const Segment& fst, const Segment& snd);
inline bool operator !=(const Segment& fst, const Segment& snd)
	{ return !(fst == snd); }


struct Line {
	Point a, b;

	Line(const Point& aa, const Point& bb) : a(aa), b(bb) {}
	Line(const Segment& seg) : a(seg.a), b(seg.b) {}
	virtual ~Line() {}

	bool valid() const;
	bool contains(const Point& pnt) const;
	Vector direction() const { return Vector(a, b); }
};

std::ostream& operator <<(std::ostream& os, const Line& line);
bool operator ==(const Line& fst, const Line& snd);
inline bool operator !=(const Line& fst, const Line& snd)
	{ return !(fst == snd); }


struct Plane {
	Point a, b, c;

	Plane(const Point& aa, const Point& bb, const Point& cc) :
		a(aa), b(bb), c(cc) {}
	explicit Plane(const Triangle& trg) :
		a(trg.a), b(trg.b), c(trg.c) {}
	virtual ~Plane() {}

	bool valid() const;
	bool contains(const Point& pnt) const;
	bool contains(const Line& line) const;

	Vector normal() const
		{ return Vector::outer_product(Vector(a, b), Vector(a, c)); }

	Vector unit_normal() const
		{ return Vector::unit(normal()); }

	static const Plane oxy, oxz, oyz;
};

std::ostream& operator <<(std::ostream& os, const Plane& plane);

struct EmptySet {};

/* Distance as a vector */
Vector vdistance(const Point& fst, const Point& snd);
Vector vdistance(const Point& pnt, const Plane& plane);

std::variant<EmptySet, Line, Plane>
intersection(const Plane& fst, const Plane& snd);

std::variant<EmptySet, Point, Segment>
intersection(const Line& line, const Triangle& trg);

std::variant<EmptySet, Point, Segment>
intersection(const Segment& seg, const Triangle& trg);

std::variant<EmptySet, Point, Segment>
intersection(const Segment& fst, const Segment& snd);

std::variant<EmptySet, Point, Segment>
intersection(const Line& line, const Segment& seg);

std::variant<EmptySet, Point, Line>
intersection(const Line& fst, const Line& snd);

std::variant<EmptySet, Point>
intersection(const Point& fst, const Point& snd);

template <class Figure1, class Figure2>
Float distance(const Figure1& fst, const Figure2& snd)
	{ return vdistance(fst, snd).module(); }

/*  Requires either intersected_impl(fst, snd) or intersection(fst, snd).
 *  Note. Don't redefine intersected() itself, it works badly with
 * inheritance. Provide intersected_impl(fst, snd) or intersection(fst, snd). */
template <class Figure1, class Figure2>
bool intersected(const Figure1& fst, const Figure2& snd);

/*  Calculates number of mutual intersections of geometric figures.
 *  Self-intersections are not counted, but if there are two equal figures,
 * passed by different iterators, the intersection will be taken into
 * account.
 *  InputIt must be so, that a function bool intersected(*it1, *it2) is
 * defined. Figure must have method valid(), i.e. it->valid() is defined
 *  Figures, for which valid() returns false, are ignored 
 *  If benchmark = true, generic algorithm O(n^2) is used
 *
 *  Complexity: O(n^2) in the worst case. For some figures
 * function uses more effective algorithm (if specialization of
 * nintersections_helper<>() is provided for this figure. See
 * geometry_intersections_impl.h) */
template <class InputIt, bool benchmark = false>
int nintersections(InputIt figure_fst, InputIt figure_last);

/*  Caclulates number of intersections between two groups
 * of geometric objects
 *  Intesection between figures in on group is not taken into account
 * (only between groups)
 *  InputIt must satisfy the same criteria as for nintersections()
 *  Figures, for which valid() returns false, are ignored */
template <class InputIt1, class InputIt2>
int ncrossintersections(InputIt1 a_fst, InputIt1 a_last,
	InputIt2 b_fst, InputIt2 b_last);

using IntersectionsTable = std::vector<std::array<int, 2>>;

/*  Returns a table which describes intersections between geometric
 * figures. All pairs of figures, which are intersected, and only they
 * have a corresponding entry in a table, which is a pair of these figures
 * indices. Indices starts from 0.
 *  InputIt must satisfy the same criteria as for nintersections()
 *  Figures, for which valid() returns false, are ignored
 *  If benchmark = true, generic algorithm O(n^2) is used */
template <class InputIt, bool benchmark = false>
IntersectionsTable
build_intersections_table(InputIt figure_fst, InputIt figure_last);

/*  Returns indices of all figures, which are intersected with any other one
 *  If benchmark = true, generic algorithm O(n^2) is used*/
template <class InputIt, bool benchmark = false>
std::set<int>
get_intersected_figures_indices(InputIt figure_fst, InputIt figure_last);

/* Benchmark wrappers */
template <class InputIt> const auto nintersections_benchmark = nintersections<InputIt, true>;
template <class InputIt> const auto build_intersections_table_benchmark = build_intersections_table<InputIt, true>;
template <class InputIt> const auto get_intersected_figures_indices_benchmark = get_intersected_figures_indices<InputIt, true>;


} // Geometry namespace end

/* Some templates for distance() and intersected() */
#include "geometry_impl.h"

/* Some templates needed for nintersections() work */
#include "geometry_intersections_impl.h"

#endif // GEOMETRY_H_