/* Реализованные кэши:
 * AbstractCache - общий интерфейс для всех кэшей
 * DummyCache - Хэш с 100% вероятностью промаха. Каждый раз обращается к базе данных
 * RandomCache - Выбрасывает случайную страницу
 * LRUCache - Least Recently Used
 */

#ifndef _CACHE_H_
#define _CACHE_H_

/* If CACHE_VERBOSE is defined, each call 
 * of get_page() or get_temp_page()
 * may print some info about this call */
// #define CACHE_VERBOSE

#include "database.h"
#include <unordered_map>
#include <list>

template <class DataBase>
class AbstractCache :
	public AbstractDB <
		typename DataBase::key_t,
		typename DataBase::page_t >
{
public:
	using key_t = typename DataBase::key_t;
	using page_t = typename DataBase::page_t;
	using database_t = DataBase;

	AbstractCache(const DataBase& db, size_t cache_sz) :
		m_db(db), m_cache_sz(cache_sz), m_nhits(0), m_nlookups(0) {}

	page_t get_page(const key_t& key) const override
		{ return get_temp_page(key); }

	/*  Выдает ссылку на объект прямо внутри кэша. Тем
	 * самым избегается лишнее копирование
	 *  Warning: Ссылка становится недействительной при
	 * следующем обращении к get_page() или get_temp_page() ! */
	virtual const page_t& get_temp_page(const key_t& key) const = 0;

	virtual bool is_cached(const key_t& key) const = 0;

	size_t cache_sz() const { return m_cache_sz; }
	const DataBase& db() const { return m_db; }
	
	int nhits() const { return m_nhits; }
	int nlookups() const { return m_nlookups; }
	double hit_ratio() const
		{ return (m_nlookups) ? (double(m_nhits) / m_nlookups) : 0.0; }

protected:
	const DataBase& m_db;
	size_t m_cache_sz;

	mutable int m_nhits, m_nlookups;

	void hit() const { ++m_nhits, ++m_nlookups; }
	void miss() const { ++m_nlookups; }
};


template <class DataBase>
class DummyCache : public AbstractCache<DataBase> {
public:
	using key_t = typename DataBase::key_t;
	using page_t = typename DataBase::page_t;

	explicit DummyCache(const DataBase& db) :
		AbstractCache<DataBase>(db, 0) {} // 0 - размер кэша
	
	const page_t& get_temp_page(const key_t& key) const override
	{
		this->miss();
		return page_buf = this->m_db.get_page(key);
	}

	bool is_cached(const key_t& key) const override { return false; }

private:
	mutable page_t page_buf;
};


template <class DataBase>
class RandomCache : public AbstractCache<DataBase> {
public:
	using key_t = typename DataBase::key_t;
	using page_t = typename DataBase::page_t;

	RandomCache(const DataBase& db, size_t cache_sz) :
		AbstractCache<DataBase>(db, cache_sz),
		m_cache()
		{ m_cache.reserve(cache_sz); }

	const page_t& get_temp_page(const key_t& key) const override;
	bool is_cached(const key_t& key) const override { return m_cache.count(key); }

private:
	using Hashtable = std::unordered_map<key_t, page_t>;
	mutable Hashtable m_cache;

	typename Hashtable::iterator
		get_random_it(Hashtable& htable) const;
};


template <class DataBase>
class LRUCache : public AbstractCache<DataBase> {
public:
	using key_t = typename DataBase::key_t;
	using page_t = typename DataBase::page_t;

	LRUCache(const DataBase& db, size_t cache_sz) :
		AbstractCache<DataBase>(db, cache_sz),
		m_lst(),
		m_hashtbl()
		{ m_hashtbl.reserve(cache_sz); }

	const page_t& get_temp_page(const key_t& key) const override;
	bool is_cached(const key_t& key) const override
		{ return m_hashtbl.count(key); }

private:
	using KeyAndPage = std::pair<key_t, page_t>;
	using List = std::list<KeyAndPage>;
	using Hashtable = std::unordered_map<key_t, typename List::iterator>;

	mutable List m_lst;
	mutable Hashtable m_hashtbl;
};


#include "cache_realization.h"

#endif