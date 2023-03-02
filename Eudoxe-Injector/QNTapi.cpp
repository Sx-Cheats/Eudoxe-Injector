#include "QNTapi.h"


PEB QNTapi::peb;

BOOL WINAPI QNTapi::__window_callback__(HWND hwnd, LPARAM lParam)
{
    
    QNTapi::__window_callback_arg_* a = (QNTapi::__window_callback_arg_*)lParam;
    DWORD hwnd_pid = 0;

    GetWindowThreadProcessId(hwnd, &hwnd_pid);
    if (a->pid != hwnd_pid)
        return TRUE;

    a->hwnd = hwnd;

    return FALSE;

}

HWND QNTapi::GetWindowByPid(DWORD pid)
{
    QNTapi::__window_callback_arg_ a{ 0,0 };

    a.pid = pid;

    EnumWindows(__window_callback__, (LPARAM)&a);

    return a.hwnd;
}



DWORD WINAPI QNTapi::SetPageAccess(void* base, DWORD ID)
{
    MEMORY_BASIC_INFORMATION src_plage_data;
    DWORD OldProtect;
    NtQueryVirtualMemory(NtCurrentProcess(), base, MemoryBasicInformation, &src_plage_data, sizeof(src_plage_data), NULL);
    NtProtectVirtualMemory(NtCurrentProcess(), &src_plage_data.BaseAddress, &src_plage_data.RegionSize, ID, &OldProtect);
    return OldProtect;
}


template<typename cb>
void WINAPI QNTapi::__enum_process__(cb callback)
{
    ULONG SysBufLength = 0;
    NtQuerySystemInformation(SystemProcessInformation, 0, 0, &SysBufLength);
    PSYSTEM_PROCESS_INFORMATION  ppi = (PSYSTEM_PROCESS_INFORMATION)std::calloc(1, SysBufLength);
    NtQuerySystemInformation(SystemProcessInformation, ppi, SysBufLength, &SysBufLength);
    while (ppi->NextEntryOffset)
    {
        if (!ppi->ImageName.Buffer)
            goto _continue_;

        if (callback(ppi))
            return;

    _continue_:
        ppi = (PSYSTEM_PROCESS_INFORMATION)((LPBYTE)ppi + ppi->NextEntryOffset);
    }
    ppi->UniqueProcessId = 0;
    return;

}

[[nodiscard]] EnumObj WINAPI QNTapi::GetProcesses()
{
    EnumObj AllProcessesInfo;
    __enum_process__([&](PSYSTEM_PROCESS_INFORMATION& _ppi_) -> bool
        {

            AllProcessesInfo = { _ppi_,(new EnumObj{AllProcessesInfo}) };

            return false;

        });
    return AllProcessesInfo;
}


 [[nodiscard]] PSYSTEM_PROCESS_INFORMATION WINAPI QNTapi::GetProcessInfo(std::string appname)
{
    PSYSTEM_PROCESS_INFORMATION ppi = (PSYSTEM_PROCESS_INFORMATION)calloc(1, sizeof(SYSTEM_PROCESS_INFORMATION));
    ppi->UniqueProcessId = 0;
    std::wstring n(appname.begin(), appname.end());
    __enum_process__([&](PSYSTEM_PROCESS_INFORMATION _ppi_) -> bool
        {
            if (!wcscmp(_ppi_->ImageName.Buffer, n.c_str()))
            {
                ppi = _ppi_;
                return true;
            }
            return false;
        });
    return ppi;
};

[[nodiscard]] PSYSTEM_PROCESS_INFORMATION WINAPI QNTapi::GetProcessInfo(DWORD pid)
{
    PSYSTEM_PROCESS_INFORMATION ppi = (PSYSTEM_PROCESS_INFORMATION)calloc(1, sizeof(SYSTEM_PROCESS_INFORMATION));
    ppi->UniqueProcessId = 0;
    __enum_process__([&](PSYSTEM_PROCESS_INFORMATION _ppi_) -> bool
        {
            if ((DWORD)_ppi_->UniqueProcessId == pid)
            {
                ppi = _ppi_;
                return true;
            }
            return false;
        });

    return ppi;
}

[[nodiscard]] EnumObj WINAPI QNTapi::GetAllProcessInfo(std::string ProcessName)
{
    EnumObj AllProcessesInfo;
    std::wstring n(ProcessName.begin(), ProcessName.end());
    __enum_process__([&](PSYSTEM_PROCESS_INFORMATION& _ppi_) -> bool
        {
            if (!_ppi_->ImageName.Buffer)
                return false;

            if (!wcscmp(_ppi_->ImageName.Buffer, n.c_str()))
                AllProcessesInfo = { _ppi_,(new EnumObj{AllProcessesInfo}) };

            return false;
        });
    return AllProcessesInfo;
}

[[nodiscard]] DEFAULT_SIZE WINAPI  QNTapi::GetModuleAddressA(std::string ModuleName)
{
    std::wstring n(ModuleName.begin(), ModuleName.end());
    LIST_ENTRY* current = peb.Ldr->InMemoryOrderModuleList.Flink->Flink;
    do
    {
        PLDR_DATA_TABLE_ENTRY pdte = ((PLDR_DATA_TABLE_ENTRY)(current - 1));
        if (!wcscmp(pdte->BaseDllName.Buffer, n.c_str()))
            return (DEFAULT_SIZE)pdte->DllBase;

        current = current->Flink;

    } while (current->Flink != &peb.Ldr->InMemoryOrderModuleList);
    return 0;
}


DEFAULT_SIZE WINAPI QNTapi::GetFuncAddress(DEFAULT_SIZE module, const char* function_name)
{
    if (function_name == NULL || !ModuleExist(module))
        return 0;

    PIMAGE_EXPORT_DIRECTORY ped = (PIMAGE_EXPORT_DIRECTORY)(module + (((PIMAGE_NT_HEADERS)(module + ((PIMAGE_DOS_HEADER)module)->e_lfanew))->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress));
    DWORD* AddressOfNames = (DWORD*)(module + ped->AddressOfNames);
    DWORD* AddressOfFunctions = (DWORD*)(module + ped->AddressOfFunctions);
    WORD* AddressOfNameIndex = (WORD*)(module + ped->AddressOfNameOrdinals);
    for (int i = 0; i < ped->NumberOfNames; i++)
    {
        if (!strcmp(function_name, (const char*)(module + AddressOfNames[i])))
            return  (DEFAULT_SIZE)(module + AddressOfFunctions[AddressOfNameIndex[i]]);
    }

    return 0;
}

[[nodiscard]] MODULE_ENTRY WINAPI QNTapi::GetModules()
{

    LIST_ENTRY* current = peb.Ldr->InMemoryOrderModuleList.Flink->Flink;

    MODULE_ENTRY ModuleEntry;
    do
    {
        PLDR_DATA_TABLE_ENTRY pdte = ((PLDR_DATA_TABLE_ENTRY)(current - 1));
        std::wstring n(pdte->BaseDllName.Buffer);

        ModuleEntry = { std::move(std::wstring(pdte->BaseDllName.Buffer)),std::move(std::wstring(pdte->FullDllName.Buffer)),(PVOID)pdte->DllBase,std::move(new MODULE_ENTRY{ModuleEntry.Name,ModuleEntry.path,ModuleEntry.Address,ModuleEntry.Next}) };

        current = current->Flink;

    } while (current->Flink != &peb.Ldr->InMemoryOrderModuleList);

    return ModuleEntry;
}

bool WINAPI QNTapi::ModuleExist(DEFAULT_SIZE base)
{
    LIST_ENTRY* current = peb.Ldr->InMemoryOrderModuleList.Flink->Flink;
    do
    {
        PLDR_DATA_TABLE_ENTRY pdte = ((PLDR_DATA_TABLE_ENTRY)(current - 1));
        if ((DEFAULT_SIZE)pdte->DllBase == base)
            return true;

        current = current->Flink;

    } while (current->Flink != &peb.Ldr->InMemoryOrderModuleList);

    return false;
}

std::wstring WINAPI QNTapi::ReadUTF16EX(HANDLE hauth, PWSTR  address)
{
    std::wstring ws = L"";
    if (!address)
        return ws;

    for (int x = 0;; x++)
    {
        wchar_t c;

        NtReadVirtualMemory(hauth, (address + x), &c, sizeof(wchar_t), nullptr);
        ws += c;
        if (c == '\0')
            break;
    }
    return ws;
}

void QNTapi::SetWindowFocus(DWORD pid)
{
    HWND hwnd = GetWindowByPid(pid);
    SetFocus(hwnd);
    PostMessage(hwnd, WM_SETFOCUS, 0, 0);
    SendMessage(hwnd, WM_SETFOCUS, 0, 0);
    PostMessage(hwnd, WM_APP, 0, 0);
    SendMessage(hwnd, WM_APP, 0, 0);
}


std::string WINAPI QNTapi::ReadUTF8EX(HANDLE hauth, PSTR  address)
{
    std::string s = "";
    if (!address)
        return s;

    for (int x = 0;; x++)
    {
        char c;

        NtReadVirtualMemory(hauth, (address + x), &c, sizeof(char), nullptr);
        s += c;
        if (c == '\0')
            break;
    }
    return s;
}

[[nodiscard]] MODULE_ENTRY WINAPI QNTapi::__get_modules_ex__(DWORD pid)
{
    MODULE_ENTRY ModuleEntry;
    HANDLE hauth = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pid);
    PROCESS_BASIC_INFORMATION pi;
    NtQueryInformationProcess(hauth, ProcessBasicInformation, &pi, sizeof(PROCESS_BASIC_INFORMATION), nullptr);
    PEB peb = { 0 };
    NtReadVirtualMemory(hauth, pi.PebBaseAddress, &peb, sizeof(PEB), nullptr);
    PEB_LDR_DATA  ldr;
    NtReadVirtualMemory(hauth, peb.Ldr, &ldr, sizeof(PEB_LDR_DATA), nullptr);
    LIST_ENTRY ListEntry, HeadEntry;
    NtReadVirtualMemory(hauth, ldr.InMemoryOrderModuleList.Flink, &ListEntry, sizeof(LIST_ENTRY), nullptr);
    NtReadVirtualMemory(hauth, ldr.InMemoryOrderModuleList.Flink, &HeadEntry, sizeof(LIST_ENTRY), nullptr);
    do
    {
        NtReadVirtualMemory(hauth, ListEntry.Flink, &ListEntry, sizeof(LIST_ENTRY), nullptr);
        LDR_DATA_TABLE_ENTRY pdte;
        NtReadVirtualMemory(hauth, (ListEntry.Flink - 1), &pdte, sizeof(LDR_DATA_TABLE_ENTRY), nullptr);
        ModuleEntry = { ReadUTF16EX(hauth, pdte.BaseDllName.Buffer),ReadUTF16EX(hauth, pdte.FullDllName.Buffer),(PVOID)pdte.DllBase,new MODULE_ENTRY{ModuleEntry.Name,ModuleEntry.path,ModuleEntry.Address,ModuleEntry.Next} };

    } while (memcmp(&ListEntry, &HeadEntry, sizeof(LIST_ENTRY)));

    NtClose(hauth);
    return ModuleEntry;
}

[[nodiscard]] MODULE_ENTRY WINAPI QNTapi::GetModulesEx(const char* AppName)
{
    PSYSTEM_PROCESS_INFORMATION  ProcessInfo = GetProcessInfo(AppName);
    return __get_modules_ex__((DWORD)ProcessInfo->UniqueProcessId);
}

[[nodiscard]] MODULE_ENTRY QNTapi::GetModulesEx(DWORD pid)
{
    return __get_modules_ex__(pid);
}

