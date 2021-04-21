%option c++
%option noyywrap
%option yyclass="pcl::Lexer"
%option yylineno

%{

#include "lexer.h"

%}

SPACE                   [ \t\n]
SPACES                  {SPACE}+
DIGIT                   [0-9]
INTEGER_CONSTANT        {DIGIT}+
IDENTIFIER              [a-zA-Z_][a-zA-Z_0-9]*

SHORT-COMMENT "//".*
LONG-COMMENT "/*"(.|{SPACE})*"*/"
COMMENT {SHORT-COMMENT}|{LONG-COMMENT}

%%

{SPACES}        /* do nothing */
{COMMENT}       /* do nothing */

    /***** operators, etc *****/
"*"             return yy::parser::token_type::MUL;
"/"             return yy::parser::token_type::DIV;
"+"             return yy::parser::token_type::PLUS;
"-"             return yy::parser::token_type::MINUS;
"<"             return yy::parser::token_type::LOWER;
">"             return yy::parser::token_type::GREATER;
"<="            return yy::parser::token_type::LOWER_EQ;
">="            return yy::parser::token_type::GREATER_EQ;
"="             return yy::parser::token_type::ASSIGN;
"("             return yy::parser::token_type::LPAREN;
")"             return yy::parser::token_type::RPAREN;
"{"             return yy::parser::token_type::LCURLY;
"}"             return yy::parser::token_type::RCURLY;
";"             return yy::parser::token_type::SEMICOL;

    /***** keywords *****/
"if"            return yy::parser::token_type::IF;
"while"         return yy::parser::token_type::WHILE;
"print"         return yy::parser::token_type::PRINT;
"?"             return yy::parser::token_type::INPUT;

    /***** complex lexems *****/
{INTEGER_CONSTANT}       return yy::parser::token_type::INTEGER_CONSTANT;
{IDENTIFIER}             return yy::parser::token_type::IDENTIFIER;

.               return process_error();

%%