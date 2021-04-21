#pragma once

#include "lexer.h"
#include "iast_handler.h"

namespace pcl {

class Driver {
public:
	Driver(Lexer *lexer, IASTHandler *handler) :
		m_lexer(lexer), m_handler(handler)
	{
		assert(lexer != nullptr);
		assert(handler != nullptr);
	}

	int parse()
	{
		yy::parser parser(this);
		return parser.parse(); /* parser invokes handler from itself after successful parsing */
	}

	yy::parser::token_type yylex(yy::parser::semantic_type *)
		{ return (m_current_token = m_lexer->next_token()).type(); }

	std::string strpos() const { return m_lexer->strpos(); }

	template <class T> T lexem_as() { return m_current_token.as<T>(); }

	IASTHandler *ast_handler() { return m_handler; }

private:
	Lexer *m_lexer;
	IASTHandler *m_handler;
	Token m_current_token;

	template <class T>
	void save_value(yy::parser::semantic_type *yylval, const Token& token)
	{
		yylval->as<T>() = token.as<T>();
	}
};

} // pcl namespace end