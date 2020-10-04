/*! \file
 * \brief Math graph interface
 * \author Graudt V. Link to initial repository: https://github.com/graudtV/Graph
 * \version 1.0
 */

#ifndef _GRAPH_METHODS_H_
#define _GRAPH_METHODS_H_

//******* Graph methods *******/

template <class T>
Graph<T>::~Graph()
{
	for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it)
		delete *it;
}

template <class T>
typename Graph<T>::Iterator
Graph<T>::begin()
	{ return Iterator(m_nodes.cbegin()); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::begin() const
	{ return ConstIterator(m_nodes.cbegin()); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::cbegin() const
	{ return ConstIterator(m_nodes.cbegin()); }

template <class T>
typename Graph<T>::Iterator
Graph<T>::end()
	{ return Iterator(m_nodes.cend()); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::end() const
	{ return ConstIterator(m_nodes.cend()); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::cend() const
	{ return ConstIterator(m_nodes.cend()); }

/*! \brief Adds a node to the graph
 *
 * \returns iterator on the added node or end() on failure */
template <class T>	
typename Graph<T>::Iterator
Graph<T>::add_node(const T& value)
{
	Node *node = new Node(value);
	if (node == nullptr)
		return end();
	m_nodes.push_back(node);
	return Iterator(std::prev(m_nodes.end()));
}

/*! \brief Makes a link between two nodes 
 *
 *  If nodes are not elements of one graph, or at least one of 
 * them is equal to end(), behaviour is undefined
 *  If nodes are already linked, behaviour is undefined
 * (linkage can be checked by method Iterator::linked_with)
 *  If iterators point on the same node, nothing happens */
template <class T>	
void Graph<T>::add_link(Iterator fst, Iterator snd)
{
	assert(includes_node(fst) && includes_node(snd)
		&& "Graph::add_link: trying to link nodes, which are not in one graph");
	assert(!fst.linked_with(snd)
		&& "Graph::add_link: nodes are already linked");

	if (fst != snd) {
		(*fst.m_it)->m_adjacent_nodes.push_back(*snd.m_it);
		(*snd.m_it)->m_adjacent_nodes.push_back(*fst.m_it);
	}
}

/*! \brief Counts number of nodes in graph, which have such value
 *
 * Complexity: O(N), N - number of nodes in graph */
template <class T>
int Graph<T>::count(const T& value) const
{
	int cnt = 0;
	for (auto it = begin(); it != end(); ++it)
		if (*it == value)
			++cnt;
	return cnt;
}

/*! \brief Returns first node equal to value
 * 
 * Complexity: O(N), N - number of nodes in graph */
template <class T>
typename Graph<T>::Iterator
Graph<T>::find(const T& value)
{
	auto it = begin();
	for (; it != end(); ++it)
		if (*it == value)
			break;
	return it;
}

/*! \brief Same as Graph::find(), buf for const Graph */
template <class T>
typename Graph<T>::ConstIterator
Graph<T>::find(const T& value) const
{
	auto it = begin();
	for (; it != end(); ++it)
		if (*it == value)
			break;
	return it;
}

/*! \brief Returns true if node is an element of graph, false otherwise
 *
 * If it is equal to end(), returns false
 *
 * Complexity: O(N), N - number of nodes in graph */
template <class T>
bool Graph<T>::includes_node(ConstIterator it) const
{
	for (auto it2 = begin(); it2 != end(); ++it2)
		if (it2 == it)
			return true;
	return false;
}

/*! \brief Returns true if graph is empty, false otherwise
 * \note graph.empty() is equal to (graph.size() == 0) */
template <class T>
inline bool
Graph<T>::empty() const
	{ return m_nodes.empty(); }

/*! \brief Returns total numer of nodes in graph */
template <class T>
inline int
Graph<T>::nnodes() const
	{ return m_nodes.size(); }

/*! \brief Returns total numer of edges(links) in graph
 *
 * Complexity: O(N) */
template <class T>
int Graph<T>::nlinks() const
{
	int cnt = 0;
	for (auto it = begin(); it != end(); ++it)
		cnt += (*it)->m_adjacent_nodes.size();
	return cnt / 2; // каждая связь была учтена дважды
}

// /*! \brief Deletes node from graph
//  *
//  * Behaviour is undefined, if node is equal to end()
//  *
//  *  Complexity: O(M^2 + N), M - average number of links per node,
//  * N - number of nodes in graph */
// template <class T>
// void Graph<T>::erase_node(Iterator it)
// {
// 	/* Удаляем все связи с другими узлами */
// 	for (auto adjacent = it.begin(); adjacent != it.end(); ++adjacent)
// 		erase_link(it, adjacent);
// 	m_nodes.erase(it.m_it);
// }




//******* Graph::Iterator methods *******/

template <class T>
typename Graph<T>::Iterator&
Graph<T>::Iterator::operator =(Iterator other)
	{ m_it = other.m_it; return *this; }

template <class T>
typename Graph<T>::Iterator
Graph<T>::Iterator::begin()
	{ return Iterator((*m_it)->m_adjacent_nodes.cbegin()); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::Iterator::begin() const
	{ return ConstIterator((*m_it)->m_adjacent_nodes.cbegin()); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::Iterator::cbegin() const
	{ return ConstIterator((*m_it)->m_adjacent_nodes.cbegin()); }

template <class T>
typename Graph<T>::Iterator
Graph<T>::Iterator::end()
	{ return Iterator((*m_it)->m_adjacent_nodes.cend()); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::Iterator::end() const
	{ return ConstIterator((*m_it)->m_adjacent_nodes.cend()); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::Iterator::cend() const
	{ return ConstIterator((*m_it)->m_adjacent_nodes.cend()); }

template <class T>
typename Graph<T>::Iterator&
Graph<T>::Iterator::operator ++()
	{ ++m_it; return *this; }

template <class T>
typename Graph<T>::Iterator
Graph<T>::Iterator::operator ++(int)
	{ return Iterator(std::next(m_it)); }

template <class T>
typename Graph<T>::Iterator&
Graph<T>::Iterator::operator --()
	{ --m_it; return *this; }

template <class T>
typename Graph<T>::Iterator
Graph<T>::Iterator::operator --(int)
	{ return Iterator(std::prev(m_it)); }

/*! \brief Returns the value, which is stored in the node */
template <class T>
T&
Graph<T>::Iterator::operator *()
	{ return (*m_it)->m_value; }

/*! \brief Returns node's degree, i.e. number of adjacent nodes
 * 
 * Behaviour is undefined, if iterator is equal to end() */
template <class T>
typename Graph<T>::Iterator::degree_t
Graph<T>::Iterator::degree() const
	{ return (*m_it)->m_adjacent_nodes.size(); }

/*! \brief Returns true if node linked with other node, false otherwise
 *
 * If other points to the same node, returns true
 * Behaviour is undefined, if one of the iterators is equal to end()
 *
 * Complexity: O(M), M - degree of node, for which method is called */
template <class T>
bool
Graph<T>::Iterator::linked_with(ConstIterator other) const
{
	if (*this == other)
		return true;
	for (auto it = begin(); it != end(); ++it)
		if (*it.m_it == *other.m_it)
			return true;
	return false;
}




//******* Graph::ConstIterator methods *******/

template <class T>
Graph<T>::ConstIterator::ConstIterator(Iterator other)
	: m_it(other.m_it) {}

template <class T>
typename Graph<T>::ConstIterator&
Graph<T>::ConstIterator::operator =(ConstIterator other)
	{ m_it = other.m_it; return *this; }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::ConstIterator::begin() const
	{ return ConstIterator((*m_it)->m_adjacent_nodes.cbegin()); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::ConstIterator::cbegin() const
	{ return ConstIterator((*m_it)->m_adjacent_nodes.cbegin()); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::ConstIterator::end() const
	{ return ConstIterator((*m_it)->m_adjacent_nodes.cend()); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::ConstIterator::cend() const
	{ return ConstIterator((*m_it)->m_adjacent_nodes.cend()); }

template <class T>
typename Graph<T>::ConstIterator&
Graph<T>::ConstIterator::operator ++()
	{ ++m_it; return *this; }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::ConstIterator::operator ++(int)
	{ return ConstIterator(std::next(m_it)); }

template <class T>
typename Graph<T>::ConstIterator&
Graph<T>::ConstIterator::operator --()
	{ --m_it; return *this; }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::ConstIterator::operator --(int)
	{ return ConstIterator(std::prev(m_it)); }

/*! \brief Same as Iterator::operator *, but for ConstantIterator */
template <class T>
const T&
Graph<T>::ConstIterator::operator *()
	{ return (*m_it)->m_value; }

/*! \brief Same as Iterator::degree_t, but for ConstantIterator */
template <class T>
typename Graph<T>::ConstIterator::degree_t
Graph<T>::ConstIterator::degree() const
	{ return (*m_it)->m_adjacent_nodes.size(); }

/*! \brief Same as Iterator::linked_with, but for ConstantIterator */
template <class T>
bool
Graph<T>::ConstIterator::linked_with(ConstIterator other) const
{
	if (*this == other)
		return true;
	for (auto it = begin(); it != end(); ++it)
		if (*it.m_it == *other.m_it)
			return true;
	return false;
}

#endif // _GRAPH_METHODS_H_