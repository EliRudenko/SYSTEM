#include <windows.h>
#include <tchar.h>
#include <fstream>
#include <vector>
#include <string>
#include "resource.h"

std::wstring GetFileName(int index) 
{
    return L"File" + std::to_wstring(index) + L".txt";
}

std::wstring GetResultFileName() { return L"Res.txt"; }

std::wstring GenerateRandomString() { return L"12A34B56C"; }

CRITICAL_SECTION gCriticalSection;

void WriteThreadFunction() 
{
    for (int i = 1; i <= 5; ++i) 
    {
        std::wstring fileName = GetFileName(i);
        {
            EnterCriticalSection(&gCriticalSection);
            std::wofstream fileStream(fileName);
            if (fileStream.is_open()) 
            {
                for (int j = 0; j < 10; ++j) { fileStream << GenerateRandomString() << std::endl; }
                fileStream.close();
            }
            LeaveCriticalSection(&gCriticalSection);
        }
    }
    MessageBox(NULL, L"Creating and Writing is done", L"Info", MB_OK);
}

void ReadThreadFunction() 
{
    std::wofstream resultFileStream(GetResultFileName());
    if (resultFileStream.is_open()) 
    {
        for (int i = 1; i <= 5; ++i) 
        {
            std::wstring fileName = GetFileName(i);
            {
                EnterCriticalSection(&gCriticalSection);
                std::wifstream fileStream(fileName);
                if (fileStream.is_open()) 
                {
                    std::wstring line;
                    while (std::getline(fileStream, line)) { resultFileStream << line << std::endl; }
                    fileStream.close();
                }
                LeaveCriticalSection(&gCriticalSection);
            }
        }
        resultFileStream.close();
    }
}

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    switch (uMsg) 
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) 
        {
        case IDC_BUTTON1:
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WriteThreadFunction, NULL, 0, NULL);
            return TRUE;

        case IDC_BUTTON2:
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadThreadFunction, NULL, 0, NULL);
            return TRUE;

        default:
            break;
        }
        break;

    case WM_CLOSE:
        EndDialog(hwndDlg, 0);
        return TRUE;
    }

    return FALSE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {

    InitializeCriticalSection(&gCriticalSection);

    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);

    DeleteCriticalSection(&gCriticalSection);

    return 0;
}
