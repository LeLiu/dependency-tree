#ifndef __TREE_PRINTER_H__
#define __TREE_PRINTER_H__

#include <iostream>
#include "dependency-tree.h"

class TreePrinter {
public:
	void Print(std::ostream& outs, NodePtr tree_root, bool verbose = false);

private:
	void PrintNode(std::ostream& outs, NodePtr tree_node, bool verbose);
	bool NeedPrint(NodePtr tree_node, bool verbose);
};
#endif // !__TREE_PRINTER_H__
