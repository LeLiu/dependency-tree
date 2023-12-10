
#include "dependency-tree.h"
#include "simple-pe-file.h"

#include <queue>

DependencyNode::DependencyNode() : level(0), is_leaf(true) {}

DependencyNode::DependencyNode(const ModuleInfo& module_info) : level(0), is_leaf(true) {
    this->module_info = module_info;
}
DependencyNode::DependencyNode(ModuleInfo&& module_info) : level(0), is_leaf(true) {
    this->module_info = module_info;
}

DependencyNode::~DependencyNode() {}

DependencyTree::DependencyTree() : root_(nullptr) {}

DependencyTree::~DependencyTree() {}

bool DependencyTree::MakeTree(const std::string& module_path) {
    bool success = false;
    success = MakeRoot(module_path);
    if (!success) {
        return false;
    }

    std::queue<NodePtr> process_queue;
    process_queue.push(root_);
    while (!process_queue.empty()) {
        NodePtr current_node = process_queue.front();
        process_queue.pop();
        success = ProcessNode(current_node);
        if (!success) {
            return false;
        }
        for (auto sub_node : current_node->children) {
            process_queue.push(sub_node);
        }
    }
    return true;
}

NodePtr DependencyTree::GetTreeRoot() {
    return root_;
}

bool DependencyTree::MakeRoot(const std::string& module_path) {
    ModuleInfo module_info;
    
    std::string name = module_path.substr(module_path.find_last_of("\\") + 1,  module_path.length() + 1);
    module_info.name = name;
    module_info.path = module_path;
    module_info.addr = 0;

    root_ = std::make_shared<DependencyNode>(std::move(module_info));
    return true;
}

bool DependencyTree::ProcessNode(NodePtr node) {
    ModuleInfo &info = node->module_info;
    bool success = false;

    SimplePeFile pe_file(info.path);
    success = pe_file.Open();
    if (!success) {
        return false;
    }

    std::vector<ModuleInfo> sub_mod_infos;
    success = pe_file.GetSubModules(sub_mod_infos);
    if (!success) {
        return false;
    }

    pe_file.Close();

    if (sub_mod_infos.empty()) {
        node->is_leaf = true;
        return true;
    }

    node->is_leaf = false;
    for (auto &sub_info : sub_mod_infos) {
        NodePtr sub_node = std::make_shared<DependencyNode>(std::move(sub_info));
        sub_node->level = node->level + 1;
        node->children.push_back(sub_node);
    }
    
    return true;
}
