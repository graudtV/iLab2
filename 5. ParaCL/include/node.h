#pragma once

#include "inode.h"
#include <vector>
#include <unordered_map>

namespace pcl {

class Declaration;
class DeclarationProxy;
class IExpr;
class ITypeExpr;

using UniqueITypeExprPtr			= std::unique_ptr<ITypeExpr>;

using IExprView						= NodeView<IExpr>;
using ITypeExprView					= NodeView<ITypeExpr>;
using DeclarationView				= NodeView<Declaration>;
using DeclarationProxyView			= NodeView<DeclarationProxy>;


class Declaration : public IDeclaration {
public:
	Declaration() : IDeclaration(NodeType::eDeclaration) {}

	INodeView type_expr() override { return m_type_expr.get(); }
	INodeView set_type_expr(UniqueINodePtr type_expr) override { m_type_expr = std::move(type_expr); return m_type_expr.get(); }
	const std::string_view name() const override { return m_name; }

	void deduce_implicit_types(bool implicit_decls_allowed) override;

	/* needed only for new declaration creation */
	void set_reference_to_original_name(std::string_view name) { m_name = name; }
private:
	std::string_view m_name; // original of name is stored in symtable
	UniqueINodePtr m_type_expr;
};

class Scope : public IScope {
public:
	explicit Scope(IScopeView parent_scope = nullptr) : IScope(NodeType::eScope, parent_scope) {}

	IDeclarationView insert(std::string_view name) override;
	IDeclarationView find(std::string_view name) override;
	IDeclarationView find_recursive(std::string_view name) override;
	void deduce_implicit_types(bool implicit_decls_allowed) override {}
private:
	std::unordered_map<std::string, Declaration> m_symtab;
};

class NodeList : public INodeList {
public:
	NodeList() : INodeList(NodeType::eNodeList) {}

	void push_back(UniqueINodePtr node) override
	{
		assert(node != nullptr);
		node->set_parent(this);
		m_nodes.push_back(std::move(node));		
	}

	void deduce_implicit_types(bool implicit_decls_allowed) override;

private:
	std::vector<UniqueINodePtr> m_nodes;
};

class TranslationUnit : public INode {
public:
	TranslationUnit(UniqueIScopePtr scope, UniqueINodeListPtr nodes) :
		INode(NodeType::eTranslationUnit),
		m_scope(std::move(scope)),
		m_nodes(std::move(nodes)) {}

	void deduce_implicit_types(bool implicit_decls_allowed) override;

private:
	UniqueIScopePtr m_scope;
	UniqueINodeListPtr m_nodes;
};

class IStatement : public INode {
public:
	explicit IStatement(NodeType type) : INode(type) {}
};

class ICondAndBody : public IStatement {
public:
	ICondAndBody(NodeType type, UniqueIScopePtr scope, UniqueINodePtr cond, UniqueINodePtr body) :
		IStatement(type),
		m_scope(std::move(scope)),
		m_cond(std::move(cond)),
		m_body(std::move(body))
	{
		assert(m_scope != nullptr);
		assert(m_cond != nullptr);
		assert(m_body != nullptr);
		m_scope->set_parent(this);
		m_cond->set_parent(this);
		m_body->set_parent(this);
	}

	IScopeView scope() { return m_scope.get(); }
	INodeView cond() { return m_cond.get(); }
	INodeView body() { return m_body.get(); }

	void deduce_implicit_types(bool implicit_decls_allowed) override;

protected:
	UniqueIScopePtr m_scope;
	UniqueINodePtr m_cond;
	UniqueINodePtr m_body;
};

// class If : public ICondAndBody {
// public:
// 	If(UniqueINodePtr cond, UniqueINodePtr body) :
// 		ICondAndBody(NodeType::eIf, std::move(cond), std::move(body)) {}
// };

class While : public ICondAndBody {
public:
	While(UniqueIScopePtr scope, UniqueINodePtr cond, UniqueINodePtr body) :
		ICondAndBody(NodeType::eWhile, std::move(scope), std::move(cond), std::move(body)) {}
};

class IExpr : public IStatement {
public:
	explicit IExpr(NodeType type) : IStatement(type) {}

	virtual UniqueITypeExprPtr type_expr() = 0;
};

class Op : public IExpr {
public:
	explicit Op(NodeType type) :
		IExpr(type), m_lhs(nullptr), m_rhs(nullptr) {}

	Op(NodeType type, UniqueINodePtr lhs /* != nullptr */) :
		IExpr(type), m_lhs(std::move(lhs)), m_rhs(nullptr)
	{
		assert(m_lhs != nullptr);
		m_lhs->set_parent(this);
	}
	
	Op(NodeType type, UniqueINodePtr lhs /* != nullptr */, UniqueINodePtr rhs /* != nullptr */) :
		IExpr(type),
		m_lhs(std::move(lhs)),
		m_rhs(std::move(rhs))
	{
		assert(m_lhs != nullptr);
		assert(m_rhs != nullptr);
		m_lhs->set_parent(this);
		m_rhs->set_parent(this);
	}

	INodeView lhs() { return m_lhs.get(); }
	INodeView rhs() { return m_rhs.get(); }

	void deduce_implicit_types(bool implicit_decls_allowed) override;
	UniqueITypeExprPtr type_expr() override;

private:
	UniqueINodePtr m_lhs;
	UniqueINodePtr m_rhs;
};

class IValue : public IExpr {
public:
	explicit IValue(NodeType type) : IExpr(type) {}

	virtual std::string to_string() const = 0;
	virtual size_t size() const = 0;

	void deduce_implicit_types(bool implicit_decls_allowed) override {}
};

class Integer : public IValue {
public:
	explicit Integer(int value) : IValue(NodeType::eIntegerConstant), m_value(value) {}

	std::string to_string() const override { return std::to_string(m_value); }
	size_t size() const override { return sizeof m_value; }

	int value() const { return m_value; }
	UniqueITypeExprPtr type_expr() override;

private:
	int m_value;
};

class ITypeExpr : public INode {
public:
	explicit ITypeExpr(NodeType type) : INode(type) {}

	virtual UniqueITypeExprPtr clone() const = 0;
	virtual std::string to_string() const = 0;

	void deduce_implicit_types(bool implicit_decls_allowed) override {}
};

class FundamentalType : public ITypeExpr {
public:
	explicit FundamentalType(NodeType type) : ITypeExpr(type) { assert(is_fundamental_type(type)); }

	std::string to_string() const override { return pcl::to_string(m_type); }
	UniqueITypeExprPtr clone() const override { return std::make_unique<FundamentalType>(m_type); }

	static UniqueITypeExprPtr make_int_type() { return std::make_unique<FundamentalType>(NodeType::eIntType); }
};

/*  Provides not owning access to Declaration node.
 *  Motivation: Nodes like Ops should almost always have owning access to their
 * children, but Declarations are shared between many nodes and thus stored
 * in symtable. So, Declaration cannot be stored in main tree directly. Proxy
 * solves this problem */
class DeclarationProxy : public IExpr {
public:
	explicit DeclarationProxy(INodeView node) : IExpr(NodeType::eDeclarationProxy), m_node(node) {}

	INodeView get() { return m_node; }

	void deduce_implicit_types(bool implicit_decls_allowed) override;
	UniqueITypeExprPtr type_expr() override;

private:
	INodeView m_node;
};

} // pcl namespace end