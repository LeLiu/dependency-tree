#ifndef __MODULE_INFO_H__
#define __MODULE_INFO_H__

#include <string>
#include <vector>

class ModuleInfo{
public:
    std::string name;
    std::string path;
    void       *addr;

public:
    ModuleInfo();
    ~ModuleInfo();

    bool IsValid();
};

#endif // !__MODULE_INFO_H__
