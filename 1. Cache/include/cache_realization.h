#ifndef _CACHE_REALIZATION_H_
#define _CACHE_REALIZATION_H_

#include <cassert>
#include <set>

/*  Чтобы не захламлять функции #define-ами и #endif-ами, печать вынесена
 * в отдельные макросы */
#ifdef CACHE_VERBOSE
// PRINTMSG = print message
#define _CACHE_PRINTMSG_REQUESTED_PAGE(cache_name, key) \
	std::cout << "> " #cache_name ": requested page '" << (key) << "'\n"
#define _CACHE_PRINTMSG_FOUND_IN_CACHE(cache_name, key) \
	std::cout << "> " #cache_name ": page '" << (key) << "' found in cache\n"
#define _CACHE_PRINTMSG_VACANT_SPACE(cache_name) \
	std::cout << "> " #cache_name ": using vacant space in cache\n"
#define _CACHE_PRINTMSG_DELETING_PAGE(cache_name, key) \
	std::cout << "> " #cache_name ": deleting page '" << (key) << "'\n"

#else

#define _CACHE_PRINTMSG_REQUESTED_PAGE(cache_name, key)
#define _CACHE_PRINTMSG_FOUND_IN_CACHE(cache_name, key)
#define _CACHE_PRINTMSG_VACANT_SPACE(cache_name)
#define _CACHE_PRINTMSG_DELETING_PAGE(cache_name, key)

#endif // CACHE_VERBOSE


template <class DataBase>
const typename RandomCache<DataBase>::page_t&
RandomCache<DataBase>::get_temp_page(const key_t& key) const
{
	auto it = m_hashtbl.find(key);

	_CACHE_PRINTMSG_REQUESTED_PAGE(RandomCache, key);

	if (it != m_hashtbl.end()) {
		_CACHE_PRINTMSG_FOUND_IN_CACHE(RandomCache, key);

		this->hit();
		return it->second;
	}
	
	this->miss();
	if (m_hashtbl.size() < this->m_cache_sz) { // есть свободное место в кэше
		_CACHE_PRINTMSG_VACANT_SPACE(RandomCache);

		return m_hashtbl[key] = this->m_db.get_page(key);
	}
	
	it = get_random_it(m_hashtbl);
	assert(it != m_hashtbl.end());
	_CACHE_PRINTMSG_DELETING_PAGE(RandomCache, it->first);
	m_hashtbl.erase(it);
	return m_hashtbl[key] = this->m_db.get_page(key);
}

template <class DataBase>
typename RandomCache<DataBase>::Hashtable::iterator
RandomCache<DataBase>::get_random_it(Hashtable& hashtbl) const
{
	auto it = hashtbl.begin();
	std::advance(it, rand() % hashtbl.size()); // O(n) !
	return it;
}


template <class DataBase>
const typename LRUCache<DataBase>::page_t&
LRUCache<DataBase>::get_temp_page(const key_t& key) const
{
	assert(m_lst.size() == m_hashtbl.size());

	_CACHE_PRINTMSG_REQUESTED_PAGE(LRUCache, key);

	auto search = m_hashtbl.find(key);
	if (search != m_hashtbl.end()) { // страница найдена в кэше
		_CACHE_PRINTMSG_FOUND_IN_CACHE(LRUCache, key);
		this->hit();

		auto listit = search->second;
		if (listit != m_lst.begin())
			m_lst.splice(m_lst.cbegin(), m_lst, listit);
		return listit->page;
	}

	this->miss();
	if (m_lst.size() >= this->m_cache_sz) { // вытеснение LRU страницы
		_CACHE_PRINTMSG_DELETING_PAGE(LRUCache, m_lst.back().key);

		m_hashtbl.erase(m_lst.back().key);
		m_lst.pop_back();		
	} else {
		_CACHE_PRINTMSG_VACANT_SPACE(LRUCache);
	}

	m_lst.push_front({key, this->m_db.get_page(key)});
	m_hashtbl[key] = m_lst.begin();
	return m_lst.front().page;
}

template <class DataBase>
const typename TWOQCache<DataBase>::page_t&
TWOQCache<DataBase>::get_temp_page(const key_t& key) const
{
	assert(m_lru_queue.size() <= m_lru_sz);
	assert(m_fifo_queue.size() <= m_fifo_sz);
	assert(m_hashtbl.size() == m_lru_queue.size() + m_fifo_queue.size());

	_CACHE_PRINTMSG_REQUESTED_PAGE(TWOQCache, key);

	auto search = m_hashtbl.find(key);
	if (search != m_hashtbl.end()) {
		_CACHE_PRINTMSG_FOUND_IN_CACHE(TWOQCache, key);
		this->hit();

		auto& found_page = search->second;
		if (found_page.location == HashtblEntry::FIFO_QUEUE) {
			/*  Страница найдена в FIFO очереди, переносим в более приоритетную
			 * LRU очередь */
			if (m_lru_queue.size() == m_lru_sz) {
				_CACHE_PRINTMSG_DELETING_PAGE(TWOQCache, m_lru_queue.back().key);
				pop_page(m_lru_queue);
			}
			m_lru_queue.splice(m_lru_queue.begin(), m_fifo_queue, found_page.it); // Переносим в LRU
			found_page.location = HashtblEntry::LRU_QUEUE; // Меняем флажок нахождения
		} else if (found_page.it != m_lru_queue.begin()) {
			/*  Страница найдена в LRU очереди. Если она не первая, то
			 * перемещаем в начало этой же очереди */
			m_lru_queue.splice(m_lru_queue.begin(), m_lru_queue, found_page.it);
		}
		return found_page.it->page;
	}

	this->miss();
	/* Если страница не была найдена */
	if (m_fifo_queue.size() == m_fifo_sz) {
		_CACHE_PRINTMSG_DELETING_PAGE(TWOQCache, m_fifo_queue.back().key);
		pop_page(m_fifo_queue);
	} else {
		_CACHE_PRINTMSG_VACANT_SPACE(TWOQCache);
	}
	m_fifo_queue.push_front({key, this->m_db.get_page(key)});
	m_hashtbl[key] = { HashtblEntry::FIFO_QUEUE, m_fifo_queue.begin() };
	return m_fifo_queue.front().page;
}

template <class DataBase>
void TWOQCache<DataBase>::pop_page(std::list<ListEntry>& queue) const
{
	assert(queue.size() != 0);
	assert(m_hashtbl.size() != 0);

	m_hashtbl.erase(queue.back().key);
	queue.pop_back();
}


template <class DataBase>
template <class InputIt>
const typename BeladyCache<DataBase>::page_t&
BeladyCache<DataBase>::get_temp_page(const key_t& key,
	InputIt prediction_from, InputIt prediction_to) const
{
	assert(m_hashtbl.size() <= m_cache_sz);
	_CACHE_PRINTMSG_REQUESTED_PAGE(BeladyCache, key);

	auto search = m_hashtbl.find(key);
	if (search != m_hashtbl.end()) {
		_CACHE_PRINTMSG_FOUND_IN_CACHE(BeladyCache, key);
		this->hit();
		return search->second;
	}

	this->miss();
	if (m_hashtbl.size() >= m_cache_sz) {
		/* Нужно удалить страницу, которая будет запрошена последней */

		std::set<key_t> found_keys;
		
		/*  Перебираем ожидаемые запросы, пока не останется только
		 * одна незапрошенная страница. Ее и нужно будет удалить */
		while (found_keys.size() < m_cache_sz - 1
				&& prediction_from != prediction_to) {
			found_keys.insert(*prediction_from);
			++prediction_from;
		}
		for (auto it = m_hashtbl.begin(); it != m_hashtbl.end(); ++it)
			if (found_keys.count(it->first) == 0) { // Ищем ненайденный элемент
				_CACHE_PRINTMSG_DELETING_PAGE(BeladyCache, it->first);
				m_hashtbl.erase(it);
				break;
			}
	} else {
		_CACHE_PRINTMSG_VACANT_SPACE(BeladyCache);
	}
	return m_hashtbl[key] = m_db.get_page(key);
}


#undef _CACHE_PRINTMSG_REQUESTED_PAGE
#undef _CACHE_PRINTMSG_FOUND_IN_CACHE
#undef _CACHE_PRINTMSG_VACANT_SPACE
#undef _CACHE_PRINTMSG_DELETING_PAGE


#endif // _CACHE_REALIZATION_H_