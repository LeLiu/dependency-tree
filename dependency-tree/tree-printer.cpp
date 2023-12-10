#include "tree-printer.h"


void TreePrinter::Print(std::ostream& outs, NodePtr tree_root, bool verbose) {
	outs << tree_root->module_info.path << std::endl;
	for (auto& sub_node : tree_root->children) {
		PrintNode(outs, sub_node, verbose);
	}
}

void TreePrinter::PrintNode(std::ostream& outs, NodePtr tree_node, bool verbose) {
	if (!NeedPrint(tree_node, verbose)) {
		return;
	}

	for (int i = 1; i < tree_node->level; ++i) {
		outs << "|   ";
	}
	outs << "|---- ";
	outs << tree_node->module_info.name;
	outs << "[" << tree_node->module_info.addr << "] ==> ";
	if (tree_node->module_info.path.length() > 0) {
		outs << tree_node->module_info.path;
	}
	else {
		outs << "Reference not found!";
	}
	outs << std::endl;

	for (auto& sub_node : tree_node->children) {
		PrintNode(outs, sub_node, verbose);
	}
}

bool TreePrinter::NeedPrint(NodePtr tree_node, bool verbose) {
	if (verbose) {
		return true;
	}

	if (tree_node->module_info.name.starts_with("api-ms-win-")) {
		return false;
	}

	return true;
}