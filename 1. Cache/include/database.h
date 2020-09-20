/* Реализованные "базы данных":
 * AbstractIDB - Input Data Base (IDB) - интерфейс только на чтение
 * AbstractODB - Output Data Base (ODB) - интерфейс только на запись
 * AbstractIODB - Input-Output Data Base (IODB) - и на чтение, и на запись
 * EndlessDB - содержит бесконечно много различных страниц. Key - int, Page - std::string
 * FileSystemDB - Key - имя файла (std::string), Page - его содержимое в виде std::string
 */

#ifndef _DATA_BASE_H_
#define _DATA_BASE_H_

/* SIMPLE_DB_TIMEOUT_US sets timeout for
 * each call to EndlessDB::get_page() */
#ifndef SIMPLE_DB_TIMEOUT_US
#define SIMPLE_DB_TIMEOUT_US 0
#endif

/* If SIMPLE_DB_VERBOSE is defined, each call 
 * of EndlessDB::get_page() will print some info about this call */
// #define SIMPLE_DB_VERBOSE

#include <string>
#include <vector>
#include <iostream>
#include <unistd.h> // for usleep()
#include <sys/stat.h> // for stat()
#include <unordered_map>

/* Служебный класс
 * Нужен для объявления метода contains() */
template <class Key>
class _AbstractDB {
public:
	using key_t = Key;

	_AbstractDB() = default;
	virtual ~_AbstractDB() = default;

	virtual bool contains(const key_t& key) const = 0;
};

template <class Key, class Page>
class AbstractIDB :
	public _AbstractDB<Key>
{
public:
	using key_t = Key;
	using page_t = Page;
	class PageNotFound;

	virtual page_t get_page(const key_t& key) const = 0;
};

template <class Key, class Page>
class AbstractIDB<Key, Page>::PageNotFound {
public:
	Key unfound_key;
	std::string error_description;

	PageNotFound(const Key& key, const std::string& description = "") :
		unfound_key(key),
		error_description(description) {}
};

template <class Key, class Page>
class AbstractODB :
	_AbstractDB<Key>
{
public:
	using key_t = Key;
	using page_t = Page;

	virtual void insert_page(const key_t& key, const page_t& page) = 0;
};

template <class Key, class Page>
class AbstractIODB :
	public AbstractIDB<Key, Page>,
	public AbstractODB<Key, Page>
{
public:
	using key_t = Key;
	using page_t = Page;
};


class EndlessDB :
	public AbstractIDB<int, std::string>
{
public:
	page_t get_page(const key_t& key) const override;
	bool contains(const key_t& key) const override
		{ return true; }
};


class FileSystemDB :
	public AbstractIDB<std::string, std::string>
{
public:
	/* Возвращает содержимое файла */
	std::string get_page(const std::string& filename) const override;
	bool contains(const std::string& filename) const override;
};


template <class Key, class Page>
class SimpleDB :
	public AbstractIODB<Key, Page>
{
public:
	using key_t = Key;
	using page_t = Page;
	using PageNotFound = typename AbstractIDB<Key, Page>::PageNotFound;

	page_t get_page(const key_t& key) const override;
	bool contains(const key_t& key) const override
		{ return m_hashtbl.count(key); }
	void insert_page(const key_t& key, const page_t& page) override
		{ m_hashtbl[key] = page; }
private:
	std::unordered_map<key_t, page_t> m_hashtbl;
};




/******* realization *******/

EndlessDB::page_t EndlessDB::get_page(const key_t& key) const
{
#ifdef SIMPLE_DB_VERBOSE
	std::cout << "> db: loading page " << key;
	std::cout.flush();
#endif // SIMPLE_DB_VERBOSE

	usleep(SIMPLE_DB_TIMEOUT_US);

#ifdef SIMPLE_DB_VERBOSE
	std::cout << "\t\t[ OK ]\n";
#endif // SIMPLE_DB_VERBOSE

	return "This is page " + std::to_string(key);
}

std::string FileSystemDB::get_page
	(const std::string& filename) const
{
	std::string v;
	if (FILE *fp = fopen(filename.c_str(), "r")) {
		char buf[BUFSIZ];

		while (size_t len = fread(buf, 1, sizeof(buf), fp))
			v.insert(v.end(), buf, buf + len);
		fclose(fp);
	}
	else
		throw PageNotFound(filename, "FileSystemDB: cannot"
			" open file '" + filename + "': " + strerror(errno));
	return v;
}

bool FileSystemDB::contains(const std::string& filename) const
{
	struct stat statbuf;
	return (stat(filename.c_str(), &statbuf) == 0);
}

template <class Key, class Page>
typename SimpleDB<Key, Page>::page_t
SimpleDB<Key, Page>::get_page(const key_t& key) const
{
	try {
		return m_hashtbl.at(key);
	}
	catch (std::out_of_range&) {
		throw PageNotFound(key, "");
	}
}


#endif // _DATA_BASE_H_