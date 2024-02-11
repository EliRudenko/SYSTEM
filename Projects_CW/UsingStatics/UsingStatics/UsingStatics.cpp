//2,3

#include <windows.h>
#include <tchar.h>
#include "resource.h"

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

HWND hStatic1, hStatic2;
TCHAR szCoordinates[20];
HINSTANCE hInst;
const int LEFT = 15, TOP = 110, WIDTH = 380, HEIGHT = 50;
int staticCounter = 1;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPTSTR lpszCmdLine, int nCmdShow)
{
    hInst = hInstance;
    // ������ ������� ���� ���������� �� ������ ���������� �������
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
}

BOOL GetStaticInfo(HWND hStatic, TCHAR* szInfo, int maxLen)
{// �������� ���������� � ������� 

    int staticID = GetDlgCtrlID(hStatic);
    RECT rc;
    GetWindowRect(hStatic, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    wsprintf(szInfo, TEXT("������ %d, �������: %dx%d, ����������: %d,%d"), staticID, width, height, rc.left, rc.top);

    return TRUE;
}

BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    { // 1 �������, 2 ���������� ������� � �������� �������
    case WM_CLOSE:
        EndDialog(hWnd, 0);
        return TRUE;
    case WM_INITDIALOG:
        hStatic1 = GetDlgItem(hWnd, IDC_STATIC1);

        hStatic2 = CreateWindowEx(0, TEXT("STATIC"), 0,
            WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER | WS_EX_CLIENTEDGE,
            LEFT, TOP, WIDTH, HEIGHT, hWnd, 0, hInst, 0);
        return TRUE;
    
    case WM_MOUSEMOVE:
        wsprintf(szCoordinates, TEXT("X=%d  Y=%d"), LOWORD(lParam), HIWORD(lParam));
        SetWindowText(hStatic1, szCoordinates);
        return TRUE;
    case WM_LBUTTONDOWN:
        SetWindowText(hStatic2, TEXT("������ ����� ������ ����"));
        return TRUE;
    case WM_LBUTTONUP:
    {
        int xRelease = LOWORD(lParam);
        int yRelease = HIWORD(lParam);

        if ((xRelease - LEFT) >= 10 && (yRelease - TOP) >= 10)
        {
            HWND hNewStatic = CreateWindowEx(0, TEXT("STATIC"), 0,
                WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER | WS_EX_CLIENTEDGE,
                LEFT, TOP, xRelease - LEFT, yRelease - TOP, hWnd, 0, hInst, 0);

            TCHAR szStaticText[20];
            wsprintf(szStaticText, TEXT("%d"), staticCounter++);
            SetWindowText(hNewStatic, szStaticText);
        }
        else
        {
            MessageBox(hWnd, TEXT("������ ������� ������ ���� �� ����� 10x10"), TEXT("��������������"), MB_OK | MB_ICONWARNING);
        }

        return TRUE;
    }
    case WM_RBUTTONDOWN:
    {
        // �������� ���������� ������ ������ ������� ����
        int xClick = LOWORD(lParam);
        int yClick = HIWORD(lParam);

        //����� ������� �� �����������
        HWND hClickedStatic = ChildWindowFromPoint(hWnd, { xClick, yClick });

        if (hClickedStatic != NULL && IsWindow(hClickedStatic))
        {
            // �������� ���������� � ������� � ������� � � ��������� ����
            TCHAR szStaticInfo[100];
            GetStaticInfo(hClickedStatic, szStaticInfo, sizeof(szStaticInfo) / sizeof(szStaticInfo[0]));
            SetWindowText(hWnd, szStaticInfo);
        }

        return TRUE;
    }
    case WM_LBUTTONDBLCLK:
    {
        // �������� ���������� �������� ������ ����� ������� ����
        int xDoubleClick = LOWORD(lParam);
        int yDoubleClick = HIWORD(lParam);

        HWND hClickedStatic = ChildWindowFromPoint(hWnd, { xDoubleClick, yDoubleClick });

        if (hClickedStatic != NULL && IsWindow(hClickedStatic))
        {
            // ������� ������ � ���������� ���������� �������
            int staticID = GetDlgCtrlID(hClickedStatic);
            DestroyWindow(hClickedStatic);
            SetWindowText(hWnd, TEXT("������ ������"));
        }
        return TRUE;
    }
    }
    return FALSE;
}











//1 � 2 (��������) +
/*
#include <windows.h>
#include <tchar.h>
#include "resource.h"

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

HWND hStatic1, hStatic2;
TCHAR szCoordinates[20];
HINSTANCE hInst;
const int LEFT = 15, TOP = 110, WIDTH = 380, HEIGHT = 50;
int staticCounter = 1; // �������� ������� ��� ����������� ������ �������

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPTSTR lpszCmdLine, int nCmdShow)
{
    hInst = hInstance;
    // ������ ������� ���� ���������� �� ������ ���������� �������
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
}

BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CLOSE:
        EndDialog(hWnd, 0); // ��������� ��������� ������
        return TRUE;
        // WM_INITDIALOG - ������ ��������� �������� ����� �������� ����������� ����, �� ����� ��� ������������ �� �����
    case WM_INITDIALOG:
        hStatic1 = GetDlgItem(hWnd, IDC_STATIC1); // �������� ���������� �������, ������������ �� ����� �������
        // ������ ������ � ������� CreateWindowEx
        hStatic2 = CreateWindowEx(0, TEXT("STATIC"), 0,
            WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER | WS_EX_CLIENTEDGE,
            LEFT, TOP, WIDTH, HEIGHT, hWnd, 0, hInst, 0);
        return TRUE;
    case WM_MOUSEMOVE:
        wsprintf(szCoordinates, TEXT("X=%d  Y=%d"), LOWORD(lParam), HIWORD(lParam)); // ������� ���������� ������� ����
        SetWindowText(hStatic1, szCoordinates); // ������ ��������� �� ������
        return TRUE;
    case WM_LBUTTONDOWN:
        SetWindowText(hStatic2, TEXT("������ ����� ������ ����"));
        return TRUE;
    case WM_LBUTTONUP:
    {
        // �������� ���������� ���������� ����� ������ ����
        int xRelease = LOWORD(lParam);
        int yRelease = HIWORD(lParam);

        // ��������� ������ �������������� � ������� �������������� ��� ��������������
        if ((xRelease - LEFT) >= 10 && (yRelease - TOP) >= 10)
        {
            // ������� ����� ������ � ���������� �������
            HWND hNewStatic = CreateWindowEx(0, TEXT("STATIC"), 0,
                WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER | WS_EX_CLIENTEDGE,
                LEFT, TOP, xRelease - LEFT, yRelease - TOP, hWnd, 0, hInst, 0);

            // ������������� ����� ������� ��� ���������� �����
            TCHAR szStaticText[20];
            wsprintf(szStaticText, TEXT("%d"), staticCounter++);
            SetWindowText(hNewStatic, szStaticText);
        }
        else
        {
            MessageBox(hWnd, TEXT("������ ������� ������ ���� �� ����� 10x10"), TEXT("��������������"), MB_OK | MB_ICONWARNING);
        }

        return TRUE;
    }
    case WM_RBUTTONDOWN:
        SetWindowText(hStatic2, TEXT("������ ������ ������ ����"));
        return TRUE;
    }
    return FALSE;
}
*/