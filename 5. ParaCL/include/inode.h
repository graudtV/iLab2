#pragma once

#include <iostream>
#include <cassert>
#include <memory>

namespace pcl {

enum class NodeType {
	eNodeList,
	eScope,
	eTranslationUnit,
	eDeclarationProxy,

	eIdentifier,
	eIf,
	eWhile,
	eDeclaration,

	
	/* ops */
	eNop, // node without action
	eInput,
	eOutput,
	eMul,
	eDiv,
	ePlus,
	eMinus,
	eLower,
	eGreater,
	eLowerEq,
	eGreaterEq,
	eAssign,

	/* fundamental types */
	eVoidType,
	eIntType,
	eFloatType,

	/* constants */
	eIntegerConstant,
	eFloatConstant
};

inline std::string to_string(NodeType type)
{
	switch (type) {
	case NodeType::eNodeList:			return "<node-list>";
	case NodeType::eTranslationUnit:	return "<translation-unit>";
	case NodeType::eIdentifier:			return "identifier";
	case NodeType::eIf:					return "if";
	case NodeType::eWhile:				return "while";
	case NodeType::eDeclaration:		return "<declaration>";
	case NodeType::eDeclarationProxy:	return "<declaration-proxy>";
	case NodeType::eScope:				return "scope";
	case NodeType::eNop:				return "nop";
	case NodeType::eInput:				return "?";
	case NodeType::eOutput:				return "print";
	case NodeType::eMul:				return "*";
	case NodeType::eDiv:				return "/";
	case NodeType::ePlus:				return "+";
	case NodeType::eMinus:				return "-";
	case NodeType::eLower:				return "<";
	case NodeType::eGreater:			return ">";
	case NodeType::eLowerEq:			return "<=";
	case NodeType::eGreaterEq:			return ">=";
	case NodeType::eAssign:				return "=";
	case NodeType::eVoidType:			return "void";
	case NodeType::eIntType:			return "int";
	case NodeType::eFloatType:			return "float";
	case NodeType::eIntegerConstant:	return "<int>";
	case NodeType::eFloatConstant:		return "<float>";
	/* no other cases - relying on compiler warning */
	}
}

inline bool is_op(NodeType type) { return type >= NodeType::eInput && type <= NodeType::eAssign; }
inline bool is_nullary_op(NodeType type) { return type >= NodeType::eNop && type <= NodeType::eInput; }
inline bool is_unary_op(NodeType type) { return type == NodeType::eOutput; }
inline bool is_binary_op(NodeType type) { return type >= NodeType::eMul && type <= NodeType::eAssign; }
inline bool is_infix_unary_op(NodeType type) { return is_unary_op(type) && type == NodeType::eOutput; }
inline bool is_fundamental_type(NodeType type) { return type >= NodeType::eVoidType && type <= NodeType::eFloatType; }

class INode;
class IScope;
class INodeList;
class IDeclaration;

using UniqueINodePtr			= std::unique_ptr<INode>;
using UniqueIScopePtr			= std::unique_ptr<IScope>;
using UniqueINodeListPtr		= std::unique_ptr<INodeList>;
using UniqueIDeclarationPtr		= std::unique_ptr<IDeclaration>;

template <class Node> using NodeView = Node *;

using INodeView					= NodeView<INode>;
using IScopeView				= NodeView<IScope>;
using INodeListView				= NodeView<INodeList>;
using IDeclarationView			= NodeView<IDeclaration>;

class INode {
public:
	explicit INode(NodeType type) : m_type(type), m_parent(nullptr) {}
	virtual ~INode() {}

	virtual void deduce_implicit_types(bool implicit_decls_allowed = true) = 0;

	INode(const INode& other) = delete;
	INode& operator =(const INode& other) = delete;

	INode(INode&& other) = default;
	INode& operator =(INode&& other) = default;

	NodeType type() const { return m_type; }

	void set_parent(INodeView parent) { assert(m_parent == nullptr); m_parent = parent; }
	INodeView parent() { return m_parent; }

protected:
	NodeType m_type;
	INodeView m_parent;
};

class IScope : public INode {
public:
	IScope(NodeType type, IScopeView parent_scope = nullptr) :
		INode(type), m_parent_scope(parent_scope) {}

	virtual IDeclarationView insert(std::string_view name) = 0; // creates decl
	virtual IDeclarationView find(std::string_view name) = 0;
	virtual IDeclarationView find_recursive(std::string_view name) = 0;

	IScopeView parent_scope() { return m_parent_scope; }

protected:
	IScopeView m_parent_scope;
};

class INodeList : public INode {
public:
	INodeList(NodeType type) : INode(type) {}

	virtual void push_back(UniqueINodePtr node) = 0;
};

class IDeclaration : public INode {
public:
	IDeclaration(NodeType type) : INode(type) {}

	virtual INodeView type_expr() = 0;
	virtual INodeView set_type_expr(UniqueINodePtr type_expr) = 0;
	virtual const std::string_view name() const = 0;
};

namespace unsafe_node_builders {

/*  I've decided to use raw pointers while building tree to avoid
 * excessive moves. However, almost all pointers here have a meaning of unique ptrs */

INode *make_integer(int value);
INode *make_decl_proxy(INode *node);

INode *make_while(IScope *scope, INode *cond, INode *body);
IScope *make_scope(IScopeView parent_scope = nullptr);

INode *make_translation_unit(IScope *scope, INodeList *nodes);

INode *make_nullary_op(NodeType type);
INode *make_unary_op(NodeType type, INode *node);
INode *make_binary_op(NodeType type, INode *lhs, INode *rhs);

INodeList *make_stmt_list();

INode *make_int_type();

} // unsafe_node_builders namespace end

} // pcl namespace end