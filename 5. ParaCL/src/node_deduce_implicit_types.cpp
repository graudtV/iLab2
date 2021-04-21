/* Implementation of type deduction methods */

#include "node.h"
#include "debug.h"

namespace {
int g_number_of_deduction_errors = 0;
} // anonymous namespace end

namespace pcl {

void Declaration::deduce_implicit_types(bool implicit_decls_allowed)
{
	if (m_type_expr)
		return;
	/* If tree subpass reached declaration and its type is unknown, it cannot be
	 * deduced. Reporting error. */

	std::cerr << "'" << m_name << "' is not declared in this scope"
		" and its type cannot be implicitly deduced in this context" << std::endl;
	++g_number_of_deduction_errors;
}

void NodeList::deduce_implicit_types(bool implicit_decls_allowed)
{
	for (auto&& node : m_nodes)
		node->deduce_implicit_types(implicit_decls_allowed);
}

void TranslationUnit::deduce_implicit_types(bool implicit_decls_allowed)
{
	g_number_of_deduction_errors = 0;
	m_nodes->deduce_implicit_types(implicit_decls_allowed);
	if (g_number_of_deduction_errors) {
		throw std::runtime_error("errors occured: " + std::to_string(g_number_of_deduction_errors));
	}
}

void ICondAndBody::deduce_implicit_types(bool implicit_decls_allowed)
{
	m_cond->deduce_implicit_types(implicit_decls_allowed);
	m_body->deduce_implicit_types(implicit_decls_allowed);
}

void Op::deduce_implicit_types(bool implicit_decls_allowed)
{
	//std::cerr << "op " << to_string(type()) << std::endl;
	if (type() == NodeType::eAssign) {
		//std::cerr << "probably implicit decl" << std::endl;
		if (m_lhs->type() != NodeType::eDeclarationProxy) { // lhs is some complex expression
			m_lhs->deduce_implicit_types(false);
			m_rhs->deduce_implicit_types(false);
		} else { // deducing type of identifier on lhs
			auto decl = cast<DeclarationView>(cast<DeclarationProxyView>(lhs())->get());
			rhs()->deduce_implicit_types(implicit_decls_allowed);
			decl->set_type_expr(cast<IExprView>(rhs())->type_expr());

			std::cerr << "'" << decl->name() << "': " << cast<ITypeExprView>(decl->type_expr())->to_string() << std::endl;
		}
	} else {
		//std::cerr << "entering not declarative context" << std::endl;
		if (m_lhs)
			m_lhs->deduce_implicit_types(false);
		if (m_rhs)
			m_rhs->deduce_implicit_types(false);
	}
}

void DeclarationProxy::deduce_implicit_types(bool implicit_decls_allowed)
{
	cast<DeclarationView>(m_node)->deduce_implicit_types(implicit_decls_allowed);
}

UniqueITypeExprPtr Op::type_expr()
{
	return FundamentalType::make_int_type();
}

UniqueITypeExprPtr Integer::type_expr()
	{ return FundamentalType::make_int_type(); }

} // pcl namespace end