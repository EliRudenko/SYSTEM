#include <windows.h>
#include <vector>
#include <string>
#include "resource.h"

HWND hDlg;
HWND hListOdd, hListEven, hListRandom;
HANDLE hOddThread, hEvenThread, hRandomThread;
HANDLE hOddEvent, hEvenEvent, hRandomEvent;
bool oddThreadRunning = false;
bool evenThreadRunning = false;
bool randomThreadRunning = false;


DWORD WINAPI GenerateOddNumbers(LPVOID lpParam) 
{//√≈Õ»–¿÷»ﬂ ◊»—≈À, œŒ“Œ  1
    while (true) 
    {
        WaitForSingleObject(hOddEvent, INFINITE);
        if (!oddThreadRunning)
            break;
        
        //Õ≈◊≈“Õ€≈
        int number = rand() % 100 + 1;

        if (number % 2 != 0) 
        {//¬€¬Œƒ ¬ —œ»—Œ 
            SendMessage(hListOdd, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(std::to_wstring(number).c_str()));
        }

        SetEvent(hOddEvent);
        Sleep(1000); //œ¿”«¿
    }
    return 0;
}

DWORD WINAPI GenerateEvenNumbers(LPVOID lpParam) 
{//√≈Õ»–¿÷»ﬂ ◊»—À≈, œŒ“Œ  2
    while (true) 
    {
        WaitForSingleObject(hEvenEvent, INFINITE);
        if (!evenThreadRunning)
            break;

        //◊≈“Õ€≈ 
        int number = rand() % 100 + 1;

        if (number % 2 == 0) 
        {//ƒŒ¿¬À≈Õ»≈ ¬ —œ»—Œ 
            SendMessage(hListEven, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(std::to_wstring(number).c_str()));
        }

        SetEvent(hEvenEvent);
        Sleep(1000); //œ¿”«¿
    }
    return 0;
}


DWORD WINAPI GenerateRandomNumbers(LPVOID lpParam) 
{//–¿ÕƒŒÃÕ€≈ ƒŒ 200000
    while (true)
    {
        WaitForSingleObject(hRandomEvent, INFINITE);
        if (!randomThreadRunning)
            break;

        int number = rand() % 200000 + 1;

        //¬€¬Œƒ ¬ —œ»—Œ 
        SendMessage(hListRandom, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(std::to_wstring(number).c_str()));

        SetEvent(hRandomEvent);
        Sleep(1000); //œ¿”«¿
    }

    return 0;
}


INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
    case WM_INITDIALOG:
        hDlg = hwndDlg;
        hListOdd = GetDlgItem(hwndDlg, IDC_LIST1);
        hListEven = GetDlgItem(hwndDlg, IDC_LIST2);
        hListRandom = GetDlgItem(hwndDlg, IDC_LIST3);
        hOddEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
        hEvenEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
        hRandomEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) 
        {
        case IDC_BUTTON1: 
            oddThreadRunning = !oddThreadRunning;
            SetDlgItemText(hDlg, IDC_BUTTON1, oddThreadRunning ? L"Pause" : L"Start");
            if (oddThreadRunning) 
            {
                hOddThread = CreateThread(NULL, 0, GenerateOddNumbers, NULL, 0, NULL);
            }
            return TRUE;
        case IDC_BUTTON2:
            TerminateThread(hOddThread, 0);
            oddThreadRunning = FALSE;
            ShowWindow(GetDlgItem(hDlg, IDC_BUTTON1), SW_HIDE);
            return TRUE;
        case IDC_BUTTON3: // Start/Pause Even Numbers
            evenThreadRunning = !evenThreadRunning;
            SetDlgItemText(hDlg, IDC_BUTTON3, evenThreadRunning ? L"Pause" : L"Start");
            if (evenThreadRunning) 
            {
                hEvenThread = CreateThread(NULL, 0, GenerateEvenNumbers, NULL, 0, NULL);
            }
            return TRUE;
        case IDC_BUTTON4:
            TerminateThread(hEvenThread, 0);
            evenThreadRunning = FALSE;
            ShowWindow(GetDlgItem(hDlg, IDC_BUTTON3), SW_HIDE);
            return TRUE;
        case IDC_BUTTON5: 
            randomThreadRunning = !randomThreadRunning;
            SetDlgItemText(hDlg, IDC_BUTTON5, randomThreadRunning ? L"Pause" : L"Start");
            if (randomThreadRunning) 
            {
                hRandomThread = CreateThread(NULL, 0, GenerateRandomNumbers, NULL, 0, NULL);
            }
            return TRUE;
        case IDC_BUTTON6: 
            TerminateThread(hRandomThread, 0);
            randomThreadRunning = FALSE;
            ShowWindow(GetDlgItem(hDlg, IDC_BUTTON3), SW_HIDE);
            return TRUE;
        }
        break;
    case WM_CLOSE:
        CloseHandle(hOddEvent);
        CloseHandle(hEvenEvent);
        CloseHandle(hRandomEvent);
        EndDialog(hwndDlg, 0);
        return TRUE;
    }
    return FALSE;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
}
