#ifndef __SIMPLE_PE_FILE_H__
#define __SIMPLE_PE_FILE_H__

#undef UNICODE
#include <Windows.h>
#include <string>

#include "module-info.h"

class SimplePeFile {
public:
    SimplePeFile();
    SimplePeFile(const std::string &path); 
    ~SimplePeFile();

    bool Open();
    bool Open(const std::string &path);
    bool Close();

    bool IsValid();
    bool IsOpend();
    bool IsX64();
    bool GetSubModules(std::vector<ModuleInfo> &sub_mods);

private:
    bool IsFileExists(const std::string &path);
    std::string GetModuleFullPath(const std::string& name);

private:
    std::string path_;
    bool        is_opend_;
    bool        is_valid_;

    bool is_x64_ = false;
    LPVOID base_address_ = NULL;
    HANDLE map_handle_ = NULL;
    HANDLE file_handle_ = INVALID_HANDLE_VALUE;
    PIMAGE_NT_HEADERS32 nt_header32_ = NULL;
    PIMAGE_NT_HEADERS64 nt_header64_ = NULL;
    PIMAGE_DOS_HEADER dos_header_ = NULL;
    IMAGE_SECTION_HEADER* section_heaer_ = NULL;
};

#endif //!__SIMPLE_PE_FILE_H__