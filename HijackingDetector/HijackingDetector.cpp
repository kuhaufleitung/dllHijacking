#include <chrono>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include <fstream>
#include <thread>
#include <map>

struct DllInfo
{
    std::wstring name;
    std::wstring path;
};

std::vector<DllInfo> GetLoadedDLLs(const std::wstring& processName)
{
    std::vector<DllInfo> dllList;
    DWORD processId = 0;

    // Get process ID from process name
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
    {
        std::wcerr << L"Failed to create process snapshot" << std::endl;
        return dllList;
    }

    PROCESSENTRY32W processEntry = { sizeof(PROCESSENTRY32W) };

    if (Process32FirstW(snapshot, &processEntry))
    {
        do
        {
            if (_wcsicmp(processEntry.szExeFile, processName.c_str()) == 0)
            {
                processId = processEntry.th32ProcessID;
                break;
            }
        } while (Process32NextW(snapshot, &processEntry));
    }

    CloseHandle(snapshot);

    if (processId == 0)
    {
        return dllList;
    }

    // Get DLL list for the process
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (snapshot == INVALID_HANDLE_VALUE)
    {
        std::wcerr << L"Failed to create module snapshot. Are you running with admin rights?" << std::endl;
        return dllList;
    }

    MODULEENTRY32W moduleEntry = { sizeof(MODULEENTRY32W) };

    if (Module32FirstW(snapshot, &moduleEntry))
    {
        do
        {
            DllInfo info;
            info.name = moduleEntry.szModule;
            info.path = moduleEntry.szExePath;
            dllList.push_back(std::move(info));
        } while (Module32NextW(snapshot, &moduleEntry));
    }

    CloseHandle(snapshot);
    return dllList;
}

bool FileExists(const char* filename)
{
    DWORD dwAttrib = GetFileAttributesA(filename);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
            !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void WriteDatabase(const std::wstring& procName, const std::vector<DllInfo>& dlls)
{
    std::wofstream db("db.txt");

    if (!db.is_open())
    {
        std::wcerr << L"Failed to create database file!" << std::endl;
        return;
    }

    db << procName << std::endl;
    for (const auto& dll : dlls)
    {
        db << dll.name << std::endl;
        db << L"   " << dll.path << std::endl;
    }

    std::wcout << L"Database created successfully!" << std::endl;
}

std::map<std::wstring, std::wstring> ReadDatabase(const std::wstring& procName)
{
    std::map<std::wstring, std::wstring> dllPaths;
    std::wifstream db("db.txt");

    if (!db.is_open())
    {
        return dllPaths;
    }

    std::wstring line;
    std::getline(db, line);

    if (line != procName)
    {
        db.close();
        return dllPaths;
    }

    std::wstring currentDll;
    // Tab indicates path, otherwise dllName
    while (std::getline(db, line))
    {
        if (line.empty()) continue;

        if (line[0] == L' ')
        {
            if (!currentDll.empty())
            {
                dllPaths[currentDll] = line.substr(3);
            }
        }
        else
        {
            currentDll = line;
        }
    }

    return dllPaths;
}

void CheckAgainstDatabase(const std::wstring& procName, const std::vector<DllInfo>& dlls)
{
    // If database doesn't exist, create it
    if (!FileExists("db.txt"))
    {
        std::wcout << L"Database not found. Creating new database..." << std::endl;
        WriteDatabase(procName, dlls);
        return;
    }

    // Read existing database
    auto dbPaths = ReadDatabase(procName);
    if (dbPaths.empty())
    {
        std::wcout << L"Process not found in database or database is empty. Creating new entry..." << std::endl;
        WriteDatabase(procName, dlls);
        return;
    }

    // Compare current DLLs with database
    bool differences_found = false;
    for (const auto& dll : dlls)
    {
        auto it = dbPaths.find(dll.name);
        if (it != dbPaths.end())
        {
            if (it->second != dll.path)
            {
                std::wcout << L"Path mismatch for DLL: " << dll.name << std::endl;
                std::wcout << L"Database path: " << it->second << std::endl;
                std::wcout << L"Current path:  " << dll.path << std::endl << std::endl;
                differences_found = true;
            }
        }
        else
        {
            std::wcout << L"New DLL found: " << dll.name << std::endl;
            std::wcout << L"Path: " << dll.path << std::endl << std::endl;
            differences_found = true;
        }
    }

    for (const auto& dbDll : dbPaths)
    {
        bool found = false;
        for (const auto& dll : dlls)
        {
            if (dll.name == dbDll.first)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            std::wcout << L"DLL no longer present: " << dbDll.first << std::endl;
            std::wcout << L"Old path: " << dbDll.second << std::endl << std::endl;
            differences_found = true;
        }
    }

    if (!differences_found)
    {
        std::wcout << L"All DLLs match the database." << std::endl;
    }
    else
    {
        MessageBoxW(NULL, L"DLL paths have changed, possible Hijacking detected! See console for details.", L"DLL Path Change Detected", MB_ICONWARNING | MB_OK);
    }
}


int main()
{
    const std::wstring processName = L"VulnerableProgram.exe";
    std::wcout << L"Waiting for process " << processName << L" to start...\n" << std::endl;
    auto dlls = GetLoadedDLLs(processName);
    bool isOpen = false;
    while (true)
    {
        const auto dlls = GetLoadedDLLs(processName);

        if (!dlls.empty())
        {
            if (!isOpen)
            {
                std::wcout << "Found following DLLs from " << processName << "..." << std::endl;
                for (const auto& dll : dlls)
                {
                    std::wcout << L"Name: " << dll.name << std::endl;
                    std::wcout << L"Path: " << dll.path << L"\n" << std::endl;
                }
				CheckAgainstDatabase(processName, dlls);
            }
            isOpen = true;
        }
        else
        {
            isOpen = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    std::wcin.get();

    return 0;
}