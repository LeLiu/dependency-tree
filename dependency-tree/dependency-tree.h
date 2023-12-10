#ifndef __DEPENDENCY_TREE_H__
#define __DEPENDENCY_TREE_H__

#include <string>
#include <vector>
#include <memory>

#include "module-info.h"

class DependencyNode {
    typedef std::shared_ptr<DependencyNode> PtrType;
public:
    DependencyNode();
    DependencyNode(const ModuleInfo& module_info);
    DependencyNode(ModuleInfo&& module_info);
    ~DependencyNode();

public:
    ModuleInfo module_info;
    int level;
    bool is_leaf;
    std::vector<PtrType> children;
};

typedef std::shared_ptr<DependencyNode> NodePtr;

class DependencyTree {
public:
    DependencyTree();
    ~DependencyTree();

public:
    bool MakeTree(const std::string &module_path);
    NodePtr GetTreeRoot();
private:
    bool MakeRoot(const std::string& module_path);
    bool ProcessNode(NodePtr node);

 private:
    NodePtr root_;
};
#endif //__DEPENDENCY_TREE_H__
