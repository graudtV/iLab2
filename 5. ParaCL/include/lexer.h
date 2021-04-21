#pragma once

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif // yyFlexLexer

#include "parser.tab.hh"
#include <charconv>

namespace pcl {

class Token {
public:
	using TokenType = yy::parser::token_type;

	Token() = default;
	Token(TokenType type, std::string_view text) : m_type(type), m_text(text) {}
	
	TokenType type() const { return m_type; }

	/* converts text to signed or unsigned integer type */
	template <class T> T as() const
	{
		T val;
		auto res = std::from_chars(m_text.data(), m_text.data() + m_text.size(), val);
		if (res.ec == std::errc::invalid_argument)
			throw std::invalid_argument("Token::as()");
		return val;
	}
private:
	TokenType m_type;
	std::string_view m_text;
};

template <> inline std::string Token::as<std::string>() const { return std::string(m_text); }
template <> inline std::string_view Token::as<std::string_view>() const { return m_text; }

class Lexer : public yyFlexLexer {
public:
	Lexer(std::istream& source_file, std::string_view filename) :
		yyFlexLexer(&source_file), m_filename(filename) {}

	Token next_token()
	{
		auto token_type = static_cast<yy::parser::token_type>(yylex());
		return { token_type, yytext };
	}

	auto lineno() const { return yylineno; }
	std::string_view filename() const { return m_filename; }

	std::string strpos() const
		{ return std::string(filename()) + ":" + std::to_string(lineno()); }

private:
	int yylex() override;

	int process_error()
	{
		std::cerr << strpos() << ": unexpected symbol '" << yytext << "'" << std::endl;
		return yy::parser::token_type::YYerror;
	}

	std::string m_filename;
};

} // pcl namespace end