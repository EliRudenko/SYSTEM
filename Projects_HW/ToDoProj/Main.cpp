#include <windows.h>
#include <queue>
#include <string>
#include <fstream>
#include "resource.h"

struct Task
{
    std::wstring name;
    int time;
};

std::queue<Task> tasks;
HWND hDlg1, hDlg2;
UINT_PTR timerId;

BOOL CALLBACK Dlg1Proc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK Dlg2Proc(HWND, UINT, WPARAM, LPARAM);
void AddTask();
void DeleteTask();
void StartTimer();
void StopTimer();
void TaskCompleted(bool success);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)Dlg1Proc);
    return 0;
}

BOOL CALLBACK Dlg1Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    hDlg1 = hDlg;
    switch (message)
    {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON2:
            AddTask();
            return TRUE;
        case IDC_BUTTON3: 
            DeleteTask();
            return TRUE;
        }
        break;
    case WM_TIMER:
        if (tasks.front().time-- <= 0)
        {
            StopTimer();
            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG2), hDlg, (DLGPROC)Dlg2Proc);
        }
        else { SetDlgItemInt(hDlg, IDC_EDIT1, tasks.front().time, FALSE); }
        return TRUE;
    }
    return FALSE;
}

BOOL CALLBACK Dlg2Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    hDlg2 = hDlg;
    switch (message)
    {
    case WM_INITDIALOG:
        SetDlgItemText(hDlg, IDC_EDIT4, tasks.front().name.c_str());
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON1:
            TaskCompleted(true);
            return TRUE;
        case IDC_BUTTON2:
            TaskCompleted(false);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

void AddTask()
{
    TCHAR buffer[256];
    GetDlgItemText(hDlg1, IDC_EDIT2, buffer, 256);
    std::wstring name(buffer);
    int time = GetDlgItemInt(hDlg1, IDC_EDIT3, NULL, FALSE);
    tasks.push({ name, time });
    SendDlgItemMessage(hDlg1, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)name.c_str());
    if (tasks.size() == 1)
    {
        SetDlgItemInt(hDlg1, IDC_EDIT1, time, FALSE);
        StartTimer();
    }
}

void DeleteTask()
{
    int index = SendDlgItemMessage(hDlg1, IDC_LIST2, LB_GETCURSEL, 0, 0);
    if (index != LB_ERR)
    {
        SendDlgItemMessage(hDlg1, IDC_LIST2, LB_DELETESTRING, index, 0);
        tasks.pop();
        if (!tasks.empty())
        {
            SetDlgItemInt(hDlg1, IDC_EDIT1, tasks.front().time, FALSE);
            StartTimer();
        }
        else
        {
            SetDlgItemText(hDlg1, IDC_EDIT1, L"");
            StopTimer();
        }
        MessageBox(hDlg1, L"Deleted Successfully", L"Success", MB_OK);
    }
}

void StartTimer() { timerId = SetTimer(hDlg1, 1, 1000, NULL); }

void StopTimer() { KillTimer(hDlg1, timerId); }

void TaskCompleted(bool success)
{
    std::wofstream file("Completed Tasks.txt", std::ios::app);
    file << tasks.front().name << (success ? L" - Successful\n" : L" - Failed\n");
    file.close();
    EndDialog(hDlg2, 0);
    DeleteTask();
}
