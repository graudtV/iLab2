#include "node.h"

namespace pcl {

IDeclarationView Scope::insert(std::string_view name)
{
	auto it = m_symtab.insert(std::make_pair(std::string(name), Declaration{}));
	assert(it.second == true && "trying to insert element which is already inserted");
	it.first->second.set_reference_to_original_name(it.first->first);
	return &it.first->second;
}

IDeclarationView Scope::find_recursive(std::string_view name)
{
	for (IScopeView scope = this; scope; scope = scope->parent_scope())
		if (IDeclarationView decl = scope->find(name))
			return decl;
	return nullptr;
}

IDeclarationView Scope::find(std::string_view name)
{
	/* temporary std::string is created here, but it's hard to be avoided
	 * since where is no heterogeneous lookup until C++20 */
	auto search = m_symtab.find(name.data());
	return (search != m_symtab.end())
		? &search->second /* references and pointers to elements of std::unordered_map are not invalidated by operations on the map */
		: nullptr;
}

namespace unsafe_node_builders {

INode *make_integer(int value)
	{ return new Integer(value); }

INode *make_decl_proxy(INode *node)
	{ return new DeclarationProxy(node); }

INode *make_while(IScope *scope, INode *cond, INode *body)
	{ return new While(UniqueIScopePtr(scope), UniqueINodePtr(cond), UniqueINodePtr(body)); }

IScope *make_scope(IScopeView parent_scope /* = nullptr */)
	{ return new Scope(parent_scope); }

INode *make_translation_unit(IScope *scope, INodeList *nodes)
	{ return new TranslationUnit(UniqueIScopePtr(scope), UniqueINodeListPtr(nodes)); }

INode *make_nullary_op(NodeType type)
	{ return new Op(type); }

INode *make_unary_op(NodeType type, INode *node)
	{ return new Op(type, UniqueINodePtr(node)); }

INode *make_binary_op(NodeType type, INode *lhs, INode *rhs)
	{ return new Op(type, UniqueINodePtr(lhs), UniqueINodePtr(rhs)); }

INodeList *make_stmt_list()
	{ return new NodeList; }

INode *make_int_type()
	{ return new FundamentalType(NodeType::eIntType); }

} // unsafe_node_builders namespace end

} // pcl namespace end
