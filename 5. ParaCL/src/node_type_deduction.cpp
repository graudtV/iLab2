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
	/* If tree subpass reached declaration and its type is unknown yet, then it
	 * cannot be deduced. Reporting error. */

	std::cerr << "error: '" << m_name << "' is not declared in this scope" << std::endl;
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
	if (type() == NodeType::eAssign) {
		if (m_lhs->type() != NodeType::eDeclarationProxy) { // lhs is some complex expression
			m_lhs->deduce_implicit_types(false);
			m_rhs->deduce_implicit_types(false);
		} else {
			auto decl = cast<DeclarationView>(cast<DeclarationProxyView>(lhs())->get());
			if (decl->type_expr()) { // lhs was already declared
				/*  Mixing of declared and undeclared variables should not be allowed.
				 * Treating this case just like other non-assign Ops */
				rhs()->deduce_implicit_types(false); 
			} else if (!implicit_decls_allowed) {
				rhs()->deduce_implicit_types(implicit_decls_allowed);
				std::cerr << "error: '" << decl->name() << "' is not declared in this scope"
					" and its type cannot be implicitly deduced, because one of previous values"
					" in assignment was declared before this statement" << std::endl;
				++g_number_of_deduction_errors;
			} else { // deducing type of identifier on lhs
				rhs()->deduce_implicit_types(implicit_decls_allowed);
				decl->set_type_expr(cast<IExprView>(rhs())->type_expr());
				// std::cerr << "deduced: '" << decl->name() << "': " << cast<ITypeExprView>(decl->type_expr())->to_string() << std::endl;			
			}
		}
	} else {
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
	/* most of type deduction will probably be here, but it is just int for now */
	return FundamentalType::make_int_type();
}

UniqueITypeExprPtr Integer::type_expr()
	{ return FundamentalType::make_int_type(); }

UniqueITypeExprPtr DeclarationProxy::type_expr()
{
	auto type_expr = cast<DeclarationView>(m_node)->type_expr();
	return (type_expr) ?
		cast<ITypeExprView>(type_expr)->clone()
		: nullptr;
}

} // pcl namespace end