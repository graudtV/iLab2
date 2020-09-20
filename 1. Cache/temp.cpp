// #include <iostream>

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

int main()
{


  return 0;
}
