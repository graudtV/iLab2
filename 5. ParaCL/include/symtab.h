#pragma once

#include "inode.h"
#include <unordered_map>
#include <string>
#include <cassert>

namespace pcl {
/*
class Symtab {
public:
	IDeclarationView find(std::string_view name)
	{
		auto search = m_entries.find(name.data());
		return (search == m_entries.end())
			? nullptr
			: search->second.get();
	}

	IDeclarationView insert(std::string_view name, UniqueIDeclarationPtr decl)
	{
		assert(!find(name));
		IDeclarationView raw_ptr = decl.get();
		m_entries[name.data()] = std::move(decl);
		return raw_ptr;
	}

private:
	std::unordered_map<std::string, UniqueIDeclarationPtr> m_entries;
};
*/
} // pcl namespace end