#ifndef _DATA_BASE_H_
#define _DATA_BASE_H_

/* SIMPLE_DB_TIMEOUT_US sets timeout for
 * each call to SimpleDB::get_page() */
#ifndef SIMPLE_DB_TIMEOUT_US
#define SIMPLE_DB_TIMEOUT_US 0
#endif

/* If SIMPLE_DB_VERBOSE is defined, each call 
 * of SimpleDB::get_page() will print some info about this call */
// #define SIMPLE_DB_VERBOSE

#include <string>
#include <vector>
#include <iostream>
#include <unistd.h> // for usleep()


template <class Key, class Page>
class AbstractDB {
public:
	using key_t = Key;
	using page_t = Page;

	AbstractDB() = default;
	virtual ~AbstractDB() = default;

	virtual page_t get_page(const key_t& key) const = 0;
};

class SimpleDB : public AbstractDB<int, std::string> {
public:
	page_t get_page(const key_t& key) const override;
};

SimpleDB::page_t SimpleDB::get_page(const key_t& key) const
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



class FileSystemDB :
	public AbstractDB<std::string, std::string>
{
public:
	/* Возвращает содержимое файла */
	std::string get_page(const std::string& filename) const override;
};


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
		throw std::runtime_error("FileSystemDB: cannot"
			" open file '" + filename + "': " + strerror(errno));
	return v;
}

#endif // _DATA_BASE_H_