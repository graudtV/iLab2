#ifndef INDEXATOR_H_
#define INDEXATOR_H_

#include <exception>

namespace other {

/*  Gives successive indicies to inserted values and provides
 * const-time translation from value to index and vice versa
 *  First index is 0 */
template <class Value, class Index = size_t>
class Indexator {
public:
	using index_type = Index;
	using value_type = Value;
	
	/*  inserts value if it was not inserted yet
	 * or returns index for the existing one otherwise */
	Index insert(const Value& v)
	{
		if (!contains_value(v)) {
			m_idx_to_val.push_back(v);
			return m_val_to_idx[v] = m_idx_to_val.size() - 1;	
		}
		return m_val_to_idx[v];
	}
	Index value_to_index(const Value& v) const
	{
		if (!contains_value(v))
			throw std::out_of_range("no such value");
		return m_val_to_idx.find(v)->second;
	}
	Value& index_to_value(Index i)
	{
		if (!contains_index(i))
			throw std::out_of_range("no such index");
		return m_idx_to_val[i];
	}
	const Value& index_to_value(Index i) const
	{
		if (!contains_index(i))
			throw std::out_of_range("no such index");
		return m_idx_to_val[i];		
	}
	bool contains_index(Index i) const
		{ return i >= 0 && i < m_idx_to_val.size(); }
	bool contains_value(const Value& v) const
		{ return m_val_to_idx.find(v) != m_val_to_idx.end(); }
	size_t size() const { return m_idx_to_val.size(); }
private:
	std::vector<Value> m_idx_to_val;
	std::unordered_map<Value, Index> m_val_to_idx;
};

}; // other namespace end

#endif // INDEXATOR_H_