/*! \file
 * \brief Realization of graph methods from graph.h
 * \author Graudt V. Link to initial repository: https://github.com/graudtV/Graph
 * \version 1.0
 */

#ifndef _GRAPH_H_
#define _GRAPH_H_

// TODO: erase_link(), erase_node()

#include <list>

/*! \brief Graph realization
 *
 *  T - тип хранимого в узлах значения
 *  Граф может содержать любое число узлов с одинаковыми значениями */
template <class T>
class Graph {
public:
	class Iterator;
	class ConstIterator;

	Graph() = default;
	~Graph();

	Iterator begin();
	ConstIterator begin() const;
	ConstIterator cbegin() const;

	Iterator end();
	ConstIterator end() const;
	ConstIterator cend() const;


	Iterator add_node(const T& value);
	void add_link(Iterator fst, Iterator snd);
	void erase_node(Iterator it);
	void erase_link(Iterator fst, Iterator snd);

	int count(const T& value) const;
	bool includes_node(ConstIterator it) const;
	bool empty() const;
	int nnodes() const;
	int nlinks() const;

	Iterator find(const T& value);
	ConstIterator find(const T& value) const;

private:
	struct Node;
	std::list<Node *> m_nodes;

};

template <class T>
struct Graph<T>::Node {
	T m_value;
	std::list<Node *> m_adjacent_nodes;

	Node(const T& value) : m_value(value) {}
	~Node() = default;
};

template <class T>
class Graph<T>::Iterator {
public:
	using degree_t = int;

	Iterator();
	~Iterator() = default;

	Iterator& operator =(Iterator other);

	// обход соседних узлов
	Iterator begin();
	ConstIterator begin() const;
	ConstIterator cbegin() const;

	Iterator end();
	ConstIterator end() const;
	ConstIterator cend() const;

	Iterator& operator ++();
	Iterator operator ++(int);
	Iterator& operator --();
	Iterator operator --(int);
	T& operator *();

	degree_t degree() const; // степень вершины, т.е. число смежных ребер
	bool linked_with(ConstIterator other) const;

	/*  Если вынести тела этих функций из класса,
	 * то будут проблемы, так что не надо */
	friend bool operator ==(Iterator fst, Iterator snd)
		{ return fst.m_it ==  snd.m_it; }
	friend bool operator !=(Iterator fst, Iterator snd)
		{ return fst.m_it != snd.m_it; }

private:
	using list_iterator = typename std::list<Node *>::const_iterator;
	list_iterator m_it;

	Iterator(list_iterator it) : m_it(it) {}

	friend class Graph<T>;
};

template <class T>
class Graph<T>::ConstIterator {
public:
	using degree_t = int;

	ConstIterator();
	ConstIterator(Iterator other);
	~ConstIterator() = default;

	ConstIterator& operator =(ConstIterator other);

	// обход соседних узлов
	ConstIterator begin() const;
	ConstIterator cbegin() const;

	ConstIterator end() const;
	ConstIterator cend() const;

	ConstIterator& operator ++();
	ConstIterator operator ++(int);
	ConstIterator& operator --();
	ConstIterator operator --(int);
	const T& operator *();

	degree_t degree() const; // степень вершины, т.е. число смежных ребер
	bool linked_with(ConstIterator other) const;

	/*  Если вынести тела этих функций из класса,
	 * то будут проблемы, так что не надо */
	friend bool operator ==(ConstIterator fst, ConstIterator snd)
		{ return fst.m_it ==  snd.m_it; }
	friend bool operator !=(ConstIterator fst, ConstIterator snd)
		{ return fst.m_it != snd.m_it; }

private:
	using list_iterator = typename std::list<Node *>::const_iterator;
	list_iterator m_it;

	ConstIterator(list_iterator it) : m_it(it) {}

	friend class Graph<T>;
};

#include "graph_realization.h"

#endif // _GRAPH_H_
