#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <iostream>
#include <cmath>
#include <array>
#include <variant>
#include <type_traits>

namespace Geometry {

/* Compares float according to float_tolerance */
class Float {
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

	friend bool operator ==(Float fst, Float snd) { return fabs(fst.m_val - snd.m_val) <= float_tolerance; }
	friend bool operator < (Float fst, Float snd) { return fst.m_val < snd.m_val - float_tolerance; }
	friend bool operator <=(Float fst, Float snd) { return fst.m_val <= snd.m_val + float_tolerance; }
	friend bool operator !=(Float fst, Float snd) { return !(fst == snd); }
	friend bool operator > (Float fst, Float snd) { return snd < fst; }
	friend bool operator >=(Float fst, Float snd) { return snd <= fst; }
	
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
	bool valid() const;

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

	bool valid() const;
	Float module() const
		{ return std::sqrt(x*x + y*y + z*z); }

	/* Decompostion into basis */
 	Float decompose(const Vector& basis) const;
	std::array<Float, 2> decompose(const Vector& fst, const Vector& snd) const;

	Vector operator /(Float value) const
		{ return Vector(x / value, y / value, z / value); }

	static Float inner_product(const Vector& fst, const Vector& snd)
		{ return fst.x * snd.x + fst.y * snd.y + fst.z * snd.z; }
	static Vector outer_product(const Vector& fst, const Vector& snd);
	static Float mixed_product(const Vector& fst,
		const Vector& snd, const Vector& thd);
	static Vector unit(const Vector& vec)
		{ return vec / vec.module(); }
	static bool collinear(const Vector& fst, const Vector& snd);

	static const Vector null_vector;

	struct DecompositionError {};
};

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
	bool valid() const;
};

std::ostream& operator <<(std::ostream& os, const Triangle& trg);


/* Отрезок */
struct Segment {
	Point a, b;

	Segment(const Point& aa, const Point& bb) : a(aa), b(bb) {}
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
	bool valid() const;
	bool contains(const Line& line) const;
	Vector normal() const;

	static const Plane oxy, oxz, oyz;
};

std::ostream& operator <<(std::ostream& os, const Plane& plane);

struct EmptySet {};

/* Distance as a vector */
Vector vdistance(const Point& fst, const Point& snd);
Vector vdistance(const Point& pnt, const Plane& plane);

bool intersected(const Triangle& fst, const Triangle& snd);

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

template <class Figure1, class Figure2>
bool intersected(const Figure1& fst, const Figure2& snd)
	{ return !std::holds_alternative<EmptySet>(intersection(fst, snd)); }

/*  Calculates number of mutual intersections of geometric figures.
 *  Self-intersections are not counted, but if there are two equal figures,
 * passed by different iterators, the intersection will be taken into
 * account.
 *  InputIt must be so, that a function bool intersected(*it1, *it2) is
 * defined. Figure must have method valid(), i.e. it->valid() is defined
 *  
 *  Complexity: O(n^2) in the worst case. For some figures
 * function could be redefined with better efficiency */
template <class InputIt>
int nintersections(InputIt figure_fst, InputIt figure_last);

/* Same as nintersections, but always uses generic algorithm O(n^2) */
template <class InputIt>
int nintersections_benchmark(InputIt figure_fst, InputIt figure_last);

/*  Caclulates number of intersections between two groups
 * of geometric objects
 *  Intesection between figures in on group is not taken into account
 * (only between groups)
 *  InputIt must satisfy the same criteria as in nintersections()
 *  Figures, for which valid() returns false, are ignored */
template <class InputIt1, class InputIt2>
int ncrossintersections(InputIt1 a_fst, InputIt1 a_last,
	InputIt2 b_fst, InputIt2 b_last);


} // Geometry namespace end

/* Some templates needed for nintersections() work */
#include "geometry_intersections_impl.h"

#endif // GEOMETRY_H_