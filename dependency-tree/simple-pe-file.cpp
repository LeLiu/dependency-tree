#include "simple-pe-file.h"

#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

SimplePeFile::SimplePeFile(): 
    path_(""), is_opend_(false), is_valid_(true) {
}

SimplePeFile::SimplePeFile(const std::string &path):
    path_(path), is_opend_(false), is_valid_(true) {
}

SimplePeFile::~SimplePeFile() {
    Close();
}

bool SimplePeFile::Open() {
    if (path_.length() > 0) {
        return Open(path_);
    }
    return false;
}

bool SimplePeFile::Open(const std::string &path) {
    LPCWSTR temp = LPCWSTR(path.c_str());
    file_handle_ = ::CreateFile(LPCSTR(path.c_str()),
                                      GENERIC_READ, FILE_SHARE_READ, NULL,
                                      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle_ == INVALID_HANDLE_VALUE) {
        is_valid_ = false;
        return false;
    }

    map_handle_ = CreateFileMapping(file_handle_, NULL, PAGE_READONLY, 0, 0, NULL);
    if (map_handle_ == NULL) {
        is_valid_ = false;
        return false;
    }

    base_address_ = MapViewOfFile(map_handle_, FILE_MAP_READ, 0, 0, 0);
    if (base_address_ == NULL) {
        is_valid_ = false;
        return false;
    }
   
    dos_header_ = (PIMAGE_DOS_HEADER)base_address_;
    if (dos_header_->e_magic != IMAGE_DOS_SIGNATURE) {
        is_valid_ = false;
        return false;
    }

    nt_header32_ = (PIMAGE_NT_HEADERS32)((LPBYTE)base_address_ + dos_header_->e_lfanew);
    if (nt_header32_->Signature != IMAGE_NT_SIGNATURE) {
        is_valid_ = false;
        return false;
    }

    is_x64_ = (nt_header32_->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64 ||
               nt_header32_->FileHeader.Machine == IMAGE_FILE_MACHINE_IA64);
    if (is_x64_) {
        nt_header64_ = (PIMAGE_NT_HEADERS64)nt_header32_;
    }

    is_valid_ = true;
    is_opend_ = true;
    return true;
}

bool SimplePeFile::Close() {
    if (!is_opend_) {
        return true;
    }
    if (file_handle_ != INVALID_HANDLE_VALUE) {
        CloseHandle(file_handle_);
        file_handle_ = INVALID_HANDLE_VALUE;
    }
    if (map_handle_) {
        if (base_address_) {
            UnmapViewOfFile(base_address_);
            base_address_ = NULL;
        }
        CloseHandle(map_handle_);
        map_handle_ = NULL;
    }   
    is_opend_ = false;
    return true;
}

bool SimplePeFile::IsValid() {
    return is_valid_;
}

bool SimplePeFile::IsOpend() {
    return is_opend_;
}

bool SimplePeFile::IsX64() {
    return is_x64_;
}

bool SimplePeFile::GetSubModules(std::vector<ModuleInfo> &sub_mods) {
    DWORD rva_import_table;
    if (is_x64_) {
        if (nt_header64_ == NULL) {
            is_valid_ = false;
            return false;
        }
        rva_import_table = nt_header64_->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
        if (rva_import_table == NULL) {
            is_valid_ = false;
            return false;
        }
    }
    else {
        if (nt_header32_ == NULL) {
            is_valid_ = false;
            return false;
        }
        rva_import_table = nt_header32_->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
        if (rva_import_table == NULL) {
            is_valid_ = false;
            return false;
        }
    }

    PIMAGE_IMPORT_DESCRIPTOR import_table =
        (PIMAGE_IMPORT_DESCRIPTOR)ImageRvaToVa((PIMAGE_NT_HEADERS)nt_header32_, base_address_, rva_import_table, NULL);

    int idx = 0;
    while (true) {
        IMAGE_IMPORT_DESCRIPTOR import_desc = {0};
        ModuleInfo module_info;
        memcpy(&import_desc, import_table + idx, sizeof(IMAGE_IMPORT_DESCRIPTOR));
        if (import_desc.TimeDateStamp == NULL && import_desc.Name == NULL) {
            break;
        }
        LPCSTR dll_name = (LPCSTR)ImageRvaToVa((PIMAGE_NT_HEADERS)nt_header32_, base_address_, import_table[idx].Name, NULL);

        module_info.name = dll_name;
        module_info.path = GetModuleFullPath(dll_name);
        module_info.addr = (void*)import_desc.FirstThunk;

        sub_mods.push_back(module_info);
        idx++;
    }
    return true;
}


bool SimplePeFile::IsFileExists(const std::string& path) {
    DWORD fileAttributes = GetFileAttributesA(path.c_str());
    return (fileAttributes != INVALID_FILE_ATTRIBUTES) && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

std::string SimplePeFile::GetModuleFullPath(const std::string& name) {
    std::vector<std::string> search_paths;

    // Get application dir.
    std::string app_dir = path_.substr(0, path_.find_last_of("\\") + 1);
    search_paths.push_back(app_dir);

    // Get system directory.
    char sys_dir[MAX_PATH];
    GetSystemDirectoryA(sys_dir, MAX_PATH);
    search_paths.push_back(std::string(sys_dir));

    // Get windows dirrectory.
    char win_dir[MAX_PATH];
    GetWindowsDirectoryA(win_dir, MAX_PATH);
    search_paths.push_back(std::string(win_dir));


    // Get Env::PATH.
    char *path_env_var;
    size_t len;
    if (_dupenv_s(&path_env_var, &len, "PATH") == 0 && path_env_var != nullptr) {
        char* next_token = nullptr;
        char* dir = strtok_s(path_env_var, ";", &next_token);
        while (dir != nullptr) {
            search_paths.push_back(dir);
            dir = strtok_s(nullptr, ";", &next_token);
        }
        free(path_env_var);
    }

    for (const std::string& dir : search_paths) {
        std::string module_path = dir + "\\" + name;
        if (IsFileExists(module_path)) {
            return std::move(module_path);
        }
    }

    // not found.
    return "";
}