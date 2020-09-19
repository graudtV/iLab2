#ifndef _GRAPH_METHODS_H_
#define _GRAPH_METHODS_H_

// TODO: РЕШИТЬ ПРОБЛЕМУ НИЖЕ + ОПИСАТЬ
// (переделать итераторы)
/*  Необходимо уметь проверять валидность итератора.
 * Иначе, преобразования наподобие Graph::Iterator -> Graph::WalkIterator
 * могут вести к неочевидным последствиям, если хотя бы один из итераторов - end().  
 * (Например, сравнение graph.wbegin() == graph.end()
 * гарантированно приведет к обращению за границы массива, и,
 * возможно, к segfault, если граф пуст. (graph.end() неявно
 * преобразуется к типу WalkIterator)) */

template <class T>
typename Graph<T>::Node Graph<T>::m_fictious_node = Graph<T>::Node(T());

//******* Graph methods *******/

template <class T>
Graph<T>::Graph()
	: m_nodes(1, &m_fictious_node) {}

template <class T>
Graph<T>::~Graph()
{
	for (auto it = begin(); it != end(); ++it)
		delete *it.m_it;
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
	{ return Iterator(std::prev(m_nodes.cend())); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::end() const
	{ return ConstIterator(std::prev(m_nodes.cend())); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::cend() const
	{ return ConstIterator(std::prev(m_nodes.cend())); }

template <class T>
typename Graph<T>::WalkIterator
Graph<T>::wbegin()
	{ return WalkIterator(m_nodes.front()); }

template <class T>
typename Graph<T>::ConstWalkIterator
Graph<T>::wbegin() const
	{ return ConstWalkIterator(m_nodes.front()); }

template <class T>
typename Graph<T>::ConstWalkIterator
Graph<T>::cwbegin() const
	{ return ConstWalkIterator(m_nodes.front()); }

template <class T>	
typename Graph<T>::WalkIterator
Graph<T>::add_node(const T& value)
{
	Node *node = new Node(value);
	m_nodes.back() = node; // В последнем узле всегда записан &m_fictious_node
	m_nodes.push_back(&m_fictious_node); // Добавляем &m_fictious_node в конец, как и положено
	return WalkIterator(node);
}

template <class T>	
void Graph<T>::add_link(WalkIterator from, WalkIterator to)
{
	assert(includes_node(from) && includes_node(to)
		&& "Graph::add_link: trying to link nodes, which are not in one graph");

	if (from != to)
		from.m_curnode->m_adjacent_nodes.push_back(to.m_curnode);
}

template <class T>	
void Graph<T>::add_bidir_link(WalkIterator fst, WalkIterator snd)
{
	assert(includes_node(fst) && includes_node(snd)
		&& "Graph::add_bidir_link: trying to link nodes, which are not in one graph");

	if (fst != snd) {
		fst.m_curnode->m_adjacent_nodes.push_back(snd.m_curnode);
		snd.m_curnode->m_adjacent_nodes.push_back(fst.m_curnode);
	}
}

template <class T>
int Graph<T>::count(const T& value) const
{
	int cnt = 0;
	for (auto it = begin(); it != end(); ++it)
		if (*it == value)
			++cnt;
	return cnt;
}

template <class T>
bool Graph<T>::includes_node(ConstWalkIterator it) const
{
	for (auto it2 = begin(); it2 != end(); ++it2)
		if (it2 == it)
			return true;
	return false;
}

template <class T>
int Graph<T>::nlinks() const
{
	int cnt = 0;
	for (auto it = begin(); it != end(); ++it)
		cnt += (*it)->m_adjacent_nodes.size();
	return cnt / 2; // каждая связь была учтена дважды
}



//******* Graph::Iterator methods *******/

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
Graph<T>::Iterator::operator +=(int n)
	{ m_it += n; return *this; }

template <class T>
typename Graph<T>::Iterator&
Graph<T>::Iterator::operator -=(int n)
	{ m_it -= n; return *this; }

template <class T>
typename Graph<T>::Iterator
Graph<T>::Iterator::operator -(int n)
	{ return Iterator(m_it - n); }

template <class T>
T&
Graph<T>::Iterator::operator *()
	{ return (*m_it)->m_value; }




//******* Graph::ConstIterator methods *******/

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
Graph<T>::ConstIterator::operator +=(int n)
	{ m_it += n; return *this; }

template <class T>
typename Graph<T>::ConstIterator&
Graph<T>::ConstIterator::operator -=(int n)
	{ m_it -= n; return *this; }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::ConstIterator::operator -(int n)
	{ return ConstIterator(m_it - n); }

template <class T>
const T&
Graph<T>::ConstIterator::operator *()
	{ return (*m_it)->m_value; }




//******* Graph::WalkIterator methods *******/

template <class T>
Graph<T>::WalkIterator::WalkIterator(Iterator other)
	: m_curnode(*other.m_it) {}

template <class T>
typename Graph<T>::WalkIterator&
Graph<T>::WalkIterator::operator =(Graph<T>::Iterator other)
	{ m_curnode = *other.m_it; return *this; }


template <class T>
typename Graph<T>::Iterator
Graph<T>::WalkIterator::begin()
	{ return Iterator(m_curnode->m_adjacent_nodes.cbegin()); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::WalkIterator::begin() const
	{ return ConstIterator(m_curnode->m_adjacent_nodes.cbegin()); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::WalkIterator::cbegin() const
	{ return ConstIterator(m_curnode->m_adjacent_nodes.cbegin()); }

template <class T>
typename Graph<T>::Iterator
Graph<T>::WalkIterator::end()
	{ return Iterator(m_curnode->m_adjacent_nodes.cend()); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::WalkIterator::end() const
	{ return Iterator(m_curnode->m_adjacent_nodes.cend()); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::WalkIterator::cend() const
	{ return Iterator(m_curnode->m_adjacent_nodes.cend()); }

template <class T>
T&
Graph<T>::WalkIterator::operator *()
{
	assert(m_curnode != &m_fictious_node
		&& "Graph::WalkIterator::operator *: trying to indirect iterator," 
		" which is equal to Graph::end()");
	return m_curnode->m_value;
}


//******* Graph::ConstWalkIterator methods *******/
template <class T>
Graph<T>::ConstWalkIterator::ConstWalkIterator(Iterator other) 
	: m_curnode(*other.m_it) {}

template <class T>
Graph<T>::ConstWalkIterator::ConstWalkIterator(ConstIterator other) 
	: m_curnode(*other.m_it) {}

template <class T>
Graph<T>::ConstWalkIterator::ConstWalkIterator(WalkIterator other) 
	: m_curnode(other.m_curnode) {}

template <class T>
typename Graph<T>::ConstWalkIterator&
Graph<T>::ConstWalkIterator::operator =(Iterator other)
	{ m_curnode = *other.m_it; return *this; }

template <class T>
typename Graph<T>::ConstWalkIterator&
Graph<T>::ConstWalkIterator::operator =(ConstIterator other)
	{ m_curnode = *other.m_it; return *this; }

template <class T>
typename Graph<T>::ConstWalkIterator&
Graph<T>::ConstWalkIterator::operator =(WalkIterator other)
	{ m_curnode = other.m_curnode; return *this; }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::ConstWalkIterator::begin() const
	{ return ConstIterator(m_curnode->m_adjacent_nodes.cbegin()); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::ConstWalkIterator::cbegin() const
	{ return ConstIterator(m_curnode->m_adjacent_nodes.cbegin()); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::ConstWalkIterator::end() const
	{ return Iterator(m_curnode->m_adjacent_nodes.cend()); }

template <class T>
typename Graph<T>::ConstIterator
Graph<T>::ConstWalkIterator::cend() const
	{ return Iterator(m_curnode->m_adjacent_nodes.cend()); }

template <class T>
const T&
Graph<T>::ConstWalkIterator::operator *()
{
	assert(m_curnode != &m_fictious_node
		&& "Graph::ConstWalkIterator::operator *: trying to indirect iterator," 
		" which is equal to Graph::end()");
	return m_curnode->m_value;
}

#endif // _GRAPH_METHODS_H_