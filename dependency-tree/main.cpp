
#include "dependency-tree.h"
#include "tree-printer.h"
#include <iostream>
#include <windows.h>

void PrintHelp() {
	std::cout << "dependency-tree.exe [options] <exe-path>\n"
		<< "options:\n"
		<< "   -v, --verbose : show all dlls, include windows core api dynamic libraries.\n"
		<< "   -V, --version : show version infomation.\n"
		<< "   -h, --Help : show this infomation."
		<< std::endl;
}

void PrintVersion() {
	std::cout << "dependency-tree.exe version 0.0.1 alpha." << std::endl;
}

bool IsFileExists(const std::string &path) {
	DWORD fileAttributes = GetFileAttributesA(path.c_str());
	return (fileAttributes != INVALID_FILE_ATTRIBUTES) && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}


void PrintDependencyTree(const std::string &path, bool verbose) {
	DependencyTree tree;
	tree.MakeTree(path);

	NodePtr root = tree.GetTreeRoot();

	TreePrinter printer;
	printer.Print(std::cout, root, verbose);
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		PrintHelp();
		return 1;
	}

	if (argc > 3) {
		std::cerr << "You specific wrong arguments." << std::endl;
		PrintHelp();
		return 1;
	}

	char *path = nullptr;
	bool verbose = false;
	if (argc == 2) {
		if (strcmp(argv[1], "-V") == 0 ||
			strcmp(argv[1], "--version") == 0) {
			PrintVersion();
			return 0;
		}
		else {
			path = argv[1];
		}
	}

	if (argc == 3) {
		if (strcmp(argv[1], "-v") == 0 ||
			strcmp(argv[1], "--verbose") == 0) {	
			path = argv[2];
			verbose = true;
		}
		else if (strcmp(argv[2], "-v") == 0 ||
			strcmp(argv[2], "--verbose") == 0) {
			path = argv[1];
			verbose = true;
		}
		else {
			std::cerr << "You specific wrong arguments." << std::endl;
			PrintHelp();
			return 1;
		}
	}

	if (!IsFileExists(path)) {
		std::cerr << "The file \"path\" is not exists." << std::endl;
		PrintHelp();
		return 1;
	}

	PrintDependencyTree(path, verbose);

	return 0;
}