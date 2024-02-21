#include <windows.h>
#include <psapi.h>
#include <vector>
#include <string>
#include <tchar.h>
#include "resource.h"
#include <tlhelp32.h>
#include <iostream>

#pragma comment(lib, "Psapi.lib")

HWND hListBox, hEditBox;
std::vector<std::wstring> runningProcesses;

void UpdateProcessList() 
{
    SendMessage(hListBox, LB_RESETCONTENT, 0, 0);

    DWORD processIds[1024], bytesReturned;
    if (EnumProcesses(processIds, sizeof(processIds), &bytesReturned)) 
    {
        //¬€◊»—À≈Õ»≈
        DWORD numProcesses = bytesReturned / sizeof(DWORD);
        for (DWORD i = 0; i < numProcesses; ++i) 
        {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processIds[i]);
            if (hProcess != NULL) 
            {
                TCHAR processName[MAX_PATH];
                if (GetModuleBaseName(hProcess, NULL, processName, MAX_PATH) > 0) 
                {
                    // ƒÓ·‡‚ÎÂÌËÂ Ì‡Á‚‡ÌËˇ ÔÓˆÂÒÒ‡ ‚ ÒÔËÒÓÍ
                    std::wstring processNameStr(processName);
                    SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)processNameStr.c_str());
                }
                //Á‡Í˚ÚËÂ
                CloseHandle(hProcess);
            }
        }
    }
}

void TerminateSelectedProcess() 
{
    //»Õƒ≈ —€
    int selectedIndex = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
    if (selectedIndex != LB_ERR) 
    {
        TCHAR processName[MAX_PATH];
        SendMessage(hListBox, LB_GETTEXT, selectedIndex, (LPARAM)processName);

        DWORD processId = 0;
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot != INVALID_HANDLE_VALUE) 
        {
            PROCESSENTRY32 pe32;
            pe32.dwSize = sizeof(PROCESSENTRY32);
            if (Process32First(hSnapshot, &pe32)) 
            {
                do 
                {
                    if (_tcscmp(pe32.szExeFile, processName) == 0) 
                    {
                        processId = pe32.th32ProcessID;
                        break;
                    }
                } while (Process32Next(hSnapshot, &pe32));
            }
            CloseHandle(hSnapshot);
        }

        if (processId != 0) 
        {//Terminate
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
            if (hProcess != NULL) 
            {
                TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
               
                UpdateProcessList();
            }
        }
    }
}

void CreateNewProcess() 
{
    TCHAR buffer[MAX_PATH];
    GetWindowText(hEditBox, buffer, MAX_PATH);

    //—Œ«ƒ¿Õ»≈ œ–Œ÷≈——¿
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    BOOL success = CreateProcess(NULL, buffer, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    if (success) 
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        //«¿ –€“‹, «¿ ŒÕ◊»“‹ œ–Œ÷
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        //Œ¡ÕŒ¬À≈Õ»≈ —œ»— ¿
        UpdateProcessList();
    }
    else { MessageBox(NULL, TEXT("Failed to create process."), TEXT("Error"), MB_OK | MB_ICONERROR); }
}

INT_PTR CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
    case WM_INITDIALOG:
      
        hListBox = GetDlgItem(hwnd, IDC_LIST1);
        hEditBox = GetDlgItem(hwnd, IDC_EDIT1);
        //Œ¡ÕŒ¬À≈Õ»≈ —œ»— ¿
        UpdateProcessList();
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) 
        {
        case IDC_BUTTON1:
            // Create
            CreateNewProcess();
            return TRUE;

        case IDC_BUTTON2:
            // Update
            UpdateProcessList();
            return TRUE;

        case IDC_BUTTON3:
            //Terminate
            TerminateSelectedProcess();
            return TRUE;
        }
        case IDC_BUTTON4:
        
            return TRUE;
        break;

    case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
    }
    return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);

    return 0;
}
