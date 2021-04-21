#pragma once

#include "iast_handler.h"

namespace pcl {

class ASTEvaluator : public IASTHandler {
	void process_tree(UniqueINodePtr tree) override
	{
		tree->deduce_implicit_types();
	}
};

} // pcl namespace end