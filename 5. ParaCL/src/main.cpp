#include "driver.h"
#include "ast_evaluator.h"
#include <fstream>
#include <memory>
#include <cstring>

int main(int argc, char *argv[])
try {
	std::unique_ptr<pcl::Lexer> lexer;
	std::unique_ptr<pcl::IASTHandler> handler = std::make_unique<pcl::ASTEvaluator>();
	std::ifstream source_code;

	if (argc == 2) {
		const char *filename = argv[1];
		
		source_code.open(filename);
		if (!source_code.is_open()) {
			std::cerr << "error: " << strerror(errno) << ": " << filename << std::endl;
			exit(EXIT_FAILURE);
		}
		lexer = std::make_unique<pcl::Lexer>(source_code, filename); 	
	} else {
		std::cerr << "Usage: ./pcl code.pcl" << std::endl;
		exit(EXIT_FAILURE);		
	}

	pcl::Driver driver(lexer.get(), handler.get());
	return driver.parse();
} catch(std::exception& e) {
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
}