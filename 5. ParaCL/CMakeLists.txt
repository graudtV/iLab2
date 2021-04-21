cmake_minimum_required(VERSION 3.10)

project(pcl VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

flex_target(flex-lexer
	src/lexer.l
	${CMAKE_CURRENT_BINARY_DIR}/lexer.yy.cc
	)

bison_target(bison-parser
	src/parser.y
	${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cc
	DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hh"
	)

add_flex_bison_dependency(flex-lexer bison-parser)

add_executable(pcl)

target_include_directories(pcl PRIVATE
	include
	${CMAKE_CURRENT_BINARY_DIR}
	)

target_sources(pcl PRIVATE
	src/main.cpp
	src/node.cpp
	src/node_type_deduction.cpp
	${FLEX_flex-lexer_OUTPUTS}
	${BISON_bison-parser_OUTPUTS}
	)