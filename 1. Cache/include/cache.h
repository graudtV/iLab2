/* Реализованные кэши:
 * AbstractCache - общий интерфейс для всех кэшей
 * DummyCache - Хэш с 100% вероятностью промаха. Каждый раз обращается к базе данных
 * RandomCache - Выбрасывает случайную страницу
 * LRUCache - Least Recently Used algorithm
 * BeladyCache - Belady algorithm
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
#include <cassert>

namespace Cache {

class CacheAnalitics {
public:
	CacheAnalitics() :
		m_nhits(0), m_nlookups(0) {}
	~CacheAnalitics() = default;

	int nhits() const { return m_nhits; }
	int nlookups() const { return m_nlookups; }
	double hit_ratio() const
		{ return (m_nlookups) ? (static_cast<double>(m_nhits) / m_nlookups) : 0.0; }

protected:
	void hit() const { ++m_nhits, ++m_nlookups; }
	void miss() const { ++m_nlookups; }

private:
	mutable int m_nhits, m_nlookups;
};


template <class DataBase>
class AbstractCache :
	public CacheAnalitics,
	public DB::AbstractIDB <
		typename DataBase::key_t,
		typename DataBase::page_t >
{
public:
	using key_t = typename DataBase::key_t;
	using page_t = typename DataBase::page_t;
	using database_t = DataBase;

	AbstractCache(const DataBase& db, size_t cache_sz) :
		m_db(db), m_cache_sz(cache_sz) {}
	virtual ~AbstractCache() = default;

	AbstractCache(const AbstractCache& other) = delete;
	AbstractCache& operator =(const AbstractCache& other) = delete;

	bool contains(const key_t& key) const override
		{ return m_db.contains(key); }
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

protected:
	const DataBase& m_db;
	size_t m_cache_sz;
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
		m_hashtbl()
		{ assert(cache_sz > 0); m_hashtbl.reserve(cache_sz); }

	const page_t& get_temp_page(const key_t& key) const override;
	bool is_cached(const key_t& key) const override { return m_hashtbl.count(key); }

private:
	using Hashtable = std::unordered_map<key_t, page_t>;
	mutable Hashtable m_hashtbl;

	typename Hashtable::iterator
		get_random_it(Hashtable& hashtbl) const;
};


template <class DataBase>
class LRUCache : public AbstractCache<DataBase> {
public:
	using key_t = typename DataBase::key_t;
	using page_t = typename DataBase::page_t;

	LRUCache(const DataBase& db, size_t cache_sz) :
		AbstractCache<DataBase>(db, cache_sz)
		{ assert(cache_sz > 0); m_hashtbl.reserve(cache_sz); }

	const page_t& get_temp_page(const key_t& key) const override;
	bool is_cached(const key_t& key) const override
		{ return m_hashtbl.count(key); }

private:
	using KeyAndPage = std::pair<key_t, page_t>;
	struct ListEntry {
		key_t key;
		page_t page;
	};
	using List = std::list<ListEntry>;
	using Hashtable = std::unordered_map<key_t, typename List::iterator>;

	mutable List m_lst;
	mutable Hashtable m_hashtbl;
};

template <class DataBase>
class TWOQCache : public AbstractCache<DataBase> {
public:
	using key_t = typename DataBase::key_t;
	using page_t = typename DataBase::page_t;

	TWOQCache(const DataBase& db, size_t cache_sz) :
		AbstractCache<DataBase>(db, cache_sz)
	{
		assert(cache_sz > 1);
		m_hashtbl.reserve(cache_sz);
		m_fifo_sz = 0.2 * cache_sz + 1;
		m_lru_sz = cache_sz - m_fifo_sz;
		assert(m_fifo_sz > 0);
		assert(m_lru_sz > 0);
		assert(m_fifo_sz + m_lru_sz == cache_sz);
	}

	const page_t& get_temp_page(const key_t& key) const override;
	bool is_cached(const key_t& key) const override
		{ return m_hashtbl.count(key); }

private:
	struct ListEntry {
		key_t key;
		page_t page;
	};
	struct HashtblEntry {
		enum { FIFO_QUEUE, LRU_QUEUE } location; // в какой из очередей
		typename std::list<ListEntry>::iterator it;
	};

	mutable std::list<ListEntry> m_lru_queue;
	mutable std::list<ListEntry> m_fifo_queue;
	mutable std::unordered_map<key_t, HashtblEntry> m_hashtbl;

	size_t m_lru_sz;
	size_t m_fifo_sz;

	void pop_page(std::list<ListEntry>& queue) const;
};

/* BeladyCache не наследуется от AbstractCache,
 * т.к. функции get_page() и get_temp_page() отличаются для
 * этих классов */
template <class DataBase>
class BeladyCache :
	public CacheAnalitics
{
public:
	using key_t = typename DataBase::key_t;
	using page_t = typename DataBase::page_t;
	using database_t = DataBase;

	BeladyCache(const DataBase& db, size_t cache_sz) :
		m_db(db), m_cache_sz(cache_sz) { assert(cache_sz > 0); }
	virtual ~BeladyCache() = default;

	BeladyCache(const BeladyCache& other) = delete;
	BeladyCache& operator =(const BeladyCache& other) = delete;

	bool contains(const key_t& key) const { return m_db.contains(key); }

	template <class InputIt>
	page_t get_page(const key_t& key,
		InputIt prediction_from, InputIt prediction_to) const
		{ return get_temp_page(key, prediction_from, prediction_to); }

	/*  Выдает ссылку на объект прямо внутри кэша. Тем
	 * самым избегается лишнее копирование
	 *  Warning: Ссылка становится недействительной при
	 * следующем обращении к get_page() или get_temp_page() ! */
	template <class InputIt>
	const page_t& get_temp_page(const key_t& key,
		InputIt prediction_from, InputIt prediction_to) const;

	bool is_cached(const key_t& key) const { return m_hashtbl.count(key); }
	
	size_t cache_sz() const { return m_cache_sz; }
	const DataBase& db() const { return m_db; }

protected:
	const DataBase& m_db;
	size_t m_cache_sz;

	mutable std::unordered_map<key_t, page_t> m_hashtbl;
};

} // Cache namespace end

#include "cache_realization.h"

#endif
