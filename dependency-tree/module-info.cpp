
#include "module-info.h"
#include <windows.h>
#include <dbghelp.h>
#include <vector>

ModuleInfo::ModuleInfo() : addr(nullptr) {}

ModuleInfo::~ModuleInfo() {}

bool ModuleInfo::IsValid() {
    return addr != nullptr;
}