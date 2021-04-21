%language "c++"
%skeleton "lalr1.cc"

%defines
%define api.value.type variant
%define parse.error verbose

%param {pcl::Driver *driver}

%code requires {

namespace pcl { class Driver; }
#include "inode.h"

}

%code {

#include <memory>
#include <string>
#include "driver.h"

namespace yy {

parser::token_type yylex(parser::semantic_type *yylval, pcl::Driver *driver);

} // yy namespace end

/* local variables for parser */
namespace {

pcl::IScope *current_scope;

} // anonymous namespace end

using namespace pcl::unsafe_node_builders;
using pcl::UniqueINodePtr;
using pcl::NodeType;

void push_scope() { current_scope = make_scope(current_scope); }
void pop_scope() { current_scope = current_scope->parent_scope(); }

#define PUSH_SCOPE push_scope();
#define POP_SCOPE pop_scope();

}

/***** operators, etc *****/
%token
    MUL         "*"
    DIV         "/"
    PLUS        "+"
    MINUS       "-"
    LOWER       "<"
    GREATER     ">"
    LOWER_EQ    "<="
    GREATER_EQ  ">="
    ASSIGN      "="
    LPAREN      "("
    RPAREN      ")"
    LCURLY      "{"
    RCURLY      "}"
    SEMICOL     ";"

/***** keywords *****/
%token
    IF          "if"
    WHILE       "while"
    PRINT       "print"
    INPUT       "?"

/***** tokens with value *****/
%token
    INTEGER_CONSTANT    "integer"
    IDENTIFIER          "identifier"

%nterm<pcl::INode *> 
    stmt
    iteration-stmt
    body-block
    expr
    assignment-expr
    relational-expr
    additive-expr
    multiplicative-expr
    primary-expr
    constant

%nterm<pcl::INodeList *>
    optional-stmt-list
    stmt-list

%%

program
    :                                               { current_scope = make_scope(); /* global scope */ }
    optional-stmt-list                              { assert(current_scope->parent_scope() == nullptr); driver->ast_handler()->process_tree(UniqueINodePtr(make_translation_unit(current_scope, $2))); }
    ;

optional-stmt-list
    : stmt-list
    | %empty                                        { $$ = make_stmt_list(); }
    ;

stmt-list
    : stmt-list stmt                                { $$ = $1; $$->push_back(UniqueINodePtr($2)); }
    | stmt                                          { $$ = make_stmt_list(); $$->push_back(UniqueINodePtr($1)); }
    ;

stmt
    : ";"                                           { $$ = make_nullary_op(NodeType::eNop); }
    | expr ";"
    | PRINT expr ";"                                { $$ = make_unary_op(NodeType::eOutput, $2); }
    | iteration-stmt
    ;

iteration-stmt
    : WHILE {PUSH_SCOPE} "(" expr ")" body-block    { $$ = make_while(current_scope, $4, $6); POP_SCOPE}
    ;

body-block
    : stmt
    | "{" optional-stmt-list "}"                    { $$ = $2; }
    ;

expr: assignment-expr

assignment-expr
    : relational-expr "=" assignment-expr           { $$ = make_binary_op(NodeType::eAssign, $1, $3); }
    | relational-expr
    ;

relational-expr
    : relational-expr "<"  additive-expr            { $$ = make_binary_op(NodeType::eLower, $1, $3); }
    | relational-expr ">"  additive-expr            { $$ = make_binary_op(NodeType::eGreater, $1, $3); }
    | relational-expr "<=" additive-expr            { $$ = make_binary_op(NodeType::eLowerEq, $1, $3); }
    | relational-expr ">=" additive-expr            { $$ = make_binary_op(NodeType::eGreaterEq, $1, $3); }
    | additive-expr
    ;

additive-expr
    : additive-expr "+" multiplicative-expr         { $$ = make_binary_op(NodeType::ePlus, $1, $3); }
    | additive-expr "-" multiplicative-expr         { $$ = make_binary_op(NodeType::eMinus, $1, $3); }
    | multiplicative-expr
    ;

multiplicative-expr
    : multiplicative-expr "*" primary-expr          { $$ = make_binary_op(NodeType::eMul, $1, $3); }
    | multiplicative-expr "/" primary-expr          { $$ = make_binary_op(NodeType::eDiv, $1, $3); }  
    | primary-expr
    ;

primary-expr
    : IDENTIFIER                                    {
                                                        auto id = driver->lexem_as<std::string_view>();
                                                        if (auto *decl = current_scope->find_recursive(id)) {
                                                            //std::cerr << "found: " << id << std::endl;
                                                            $$ = make_decl_proxy(decl);
                                                        }
                                                        else {
                                                            //std::cerr << "created: " << id << std::endl;
                                                            $$ = make_decl_proxy(current_scope->insert(id));
                                                        }
                                                    }
    | constant
    | INPUT                                         { $$ = make_nullary_op(NodeType::eInput); }
    | "(" expr ")"                                  { $$ = $2; }
    ;

constant
    : INTEGER_CONSTANT                              { $$ = make_integer(driver->lexem_as<int>()); }
    /* FLOAT */
    ;

%%

/*****************/

namespace yy {

parser::token_type yylex(parser::semantic_type *yylval, pcl::Driver *driver)
    { return driver->yylex(yylval); }

void parser::error(const std::string& s)
{
    std::cerr << driver->strpos() << ": " << s << std::endl;
}

} // yy namespace end