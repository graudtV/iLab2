#ifndef _GRAPH_H_
#define _GRAPH_H_

// TODO: erase(), count(), linked_with(), nadjacent()

#include <vector>

/* T - тип хранимого в узлах значения */
template <class T>
class Graph {
public:
	/*  Предназначен для обхода всех узлов графа или всех смежных
	 * узлов для какого-то узла.
	 *  Все (!) итераторы этого типа invalidated
	 * при вызовах add_node() и erase() (для любого узла!) */
	class Iterator;
	class ConstIterator;

	/* Допускает любые "прогулки" по дереву.
	 * Invalidated только при удалении узла, на который указывает итератор */
	class WalkIterator;
	class ConstWalkIterator;

	Graph();
	~Graph();

	Iterator begin(); // обход каждого узла по одному разу
	ConstIterator begin() const;
	ConstIterator cbegin() const;

	Iterator end();
	ConstIterator end() const;
	ConstIterator cend() const;

	WalkIterator wbegin();
	ConstWalkIterator wbegin() const;
	ConstWalkIterator cwbegin() const;

	WalkIterator add_node(const T& value);
	void add_link(WalkIterator from, WalkIterator to);
	void add_bidir_link(WalkIterator fst, WalkIterator snd);
	void erase(WalkIterator it);

	int count(const T& value) const;
	bool includes_node(ConstWalkIterator it) const;
	bool empty() const { return m_nodes.empty(); }
	int nnodes() const { return m_nodes.size(); }
	int nlinks() const;

private:
	struct Node;
	std::vector<Node *> m_nodes;

	static Node m_fictious_node;
};

template <class T>
struct Graph<T>::Node {
	T m_value;
	std::vector<Node *> m_adjacent_nodes;

	Node(const T& value) : m_value(value) {}
	~Node() = default;
};

template <class T>
class Graph<T>::Iterator {
public:
	using difference_type = ssize_t;

	Iterator();
	~Iterator() = default;

	// обход соседних узлов
	Iterator begin();
	ConstIterator begin() const;
	ConstIterator cbegin() const;

	Iterator end();
	ConstIterator end() const;
	ConstIterator cend() const;


	Iterator& operator ++();
	Iterator operator ++(int);
	Iterator& operator +=(int n);
	Iterator& operator -=(int n);
	Iterator operator -(int n);
	T& operator *();

	/*  Если вынести тела этих функций из класса,
	 * то будут проблемы, так что не надо */
	friend Iterator operator +(Iterator it, int n)
		{ return Iterator(it.m_it + n); }
	friend Iterator operator +(int n, Iterator it)
		{ return Iterator(it.m_it + n); }
	friend difference_type operator -(Iterator fst, Iterator snd)
		{ return fst.m_it - snd.m_it; }
	friend bool operator < (Iterator fst, Iterator snd)
		{ return fst.m_it < snd.m_it; }
	friend bool operator > (Iterator fst, Iterator snd)
		{ return fst.m_it > snd.m_it; }
	friend bool operator <= (Iterator fst, Iterator snd)
		{ return fst.m_it <= snd.m_it; }
	friend bool operator >= (Iterator fst, Iterator snd)
		{ return fst.m_it >= snd.m_it; }
	friend bool operator ==(Iterator fst, Iterator snd)
		{ return fst.m_it ==  snd.m_it; }
	friend bool operator !=(Iterator fst, Iterator snd)
		{ return fst.m_it != snd.m_it; }

private:
	using vector_iterator = typename std::vector<Node *>::const_iterator;
	vector_iterator m_it;

	Iterator(vector_iterator it) : m_it(it) {}

	friend class Graph<T>;
};

template <class T>
class Graph<T>::ConstIterator {
public:
	using difference_type = ssize_t;

	ConstIterator();
	ConstIterator(Iterator other) : m_it(other.m_it) {}
	~ConstIterator() = default;

	ConstIterator& operator =(ConstIterator other) { m_it = other.m_it; return *this; }

	// обход соседних узлов
	ConstIterator begin() const;
	ConstIterator cbegin() const;

	ConstIterator end() const;
	ConstIterator cend() const;


	ConstIterator& operator ++();
	ConstIterator operator ++(int);
	ConstIterator& operator +=(int n);
	ConstIterator& operator -=(int n);
	ConstIterator operator -(int n);
	const T& operator *();

	/*  Если вынести тела этих функций из класса,
	 * то будут проблемы, так что не надо */
	friend ConstIterator operator +(ConstIterator it, int n)
		{ return ConstIterator(it.m_it + n); }
	friend ConstIterator operator +(int n, ConstIterator it)
		{ return ConstIterator(it.m_it + n); }
	friend difference_type operator -(ConstIterator fst, ConstIterator snd)
		{ return fst.m_it - snd.m_it; }
	friend bool operator < (ConstIterator fst, ConstIterator snd)
		{ return fst.m_it < snd.m_it; }
	friend bool operator > (ConstIterator fst, ConstIterator snd)
		{ return fst.m_it > snd.m_it; }
	friend bool operator <= (ConstIterator fst, ConstIterator snd)
		{ return fst.m_it <= snd.m_it; }
	friend bool operator >= (ConstIterator fst, ConstIterator snd)
		{ return fst.m_it >= snd.m_it; }
	friend bool operator ==(ConstIterator fst, ConstIterator snd)
		{ return fst.m_it ==  snd.m_it; }
	friend bool operator !=(ConstIterator fst, ConstIterator snd)
		{ return fst.m_it != snd.m_it; }

private:
	using vector_iterator = typename std::vector<Node *>::const_iterator;
	vector_iterator m_it;

	ConstIterator(vector_iterator it) : m_it(it) {}

	friend class Graph<T>;
};

template <class T>
class Graph<T>::WalkIterator {
public:
	WalkIterator() : m_curnode(nullptr) {}
	WalkIterator(Iterator other);
	~WalkIterator() = default;

	WalkIterator& operator =(const WalkIterator& other) = default;
	WalkIterator& operator =(Iterator other);
	
	// обход соседних узлов
	Iterator begin();
	ConstIterator begin() const;
	ConstIterator cbegin() const;

	Iterator end();
	ConstIterator end() const;
	ConstIterator cend() const;

	T& operator *();

	friend bool operator ==(WalkIterator fst, WalkIterator snd)
		{ return fst.m_curnode == snd.m_curnode; }
	friend bool operator !=(WalkIterator fst, WalkIterator snd)
		{ return fst.m_curnode != snd.m_curnode; }

private:
	Node *m_curnode;
	WalkIterator(Node *node) : m_curnode(node) {}

	friend class Graph<T>;
};

template <class T>
class Graph<T>::ConstWalkIterator {
public:
	ConstWalkIterator() : m_curnode(nullptr) {}
	ConstWalkIterator(Iterator other);
	ConstWalkIterator(ConstIterator other);
	ConstWalkIterator(WalkIterator other);
	~ConstWalkIterator() = default;

	ConstWalkIterator& operator =(const ConstWalkIterator& other) = default;
	ConstWalkIterator& operator =(Iterator other);
	ConstWalkIterator& operator =(ConstIterator other);
	ConstWalkIterator& operator =(WalkIterator other);

	// обход соседних узлов
	ConstIterator begin() const;
	ConstIterator cbegin() const;

	ConstIterator end() const;
	ConstIterator cend() const;

	const T& operator *();

	friend bool operator ==(ConstWalkIterator fst, ConstWalkIterator snd)
		{ return fst.m_curnode == snd.m_curnode; }
	friend bool operator !=(ConstWalkIterator fst, ConstWalkIterator snd)
		{ return fst.m_curnode != snd.m_curnode; }

private:
	Node *m_curnode;
	ConstWalkIterator(Node *node) : m_curnode(node) {}

	friend class Graph<T>;
};

#include "graph_realization.h"

#endif // _GRAPH_H_