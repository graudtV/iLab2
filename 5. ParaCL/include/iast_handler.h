#pragma once

#include "inode.h"

namespace pcl {

class IASTHandler {
public:
	virtual void process_tree(UniqueINodePtr tree) = 0;
	virtual ~IASTHandler() {}
};

} // pcl namespace end