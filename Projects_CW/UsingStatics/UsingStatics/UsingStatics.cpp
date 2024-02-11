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
    // создаём главное окно приложения на основе модального диалога
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
}

BOOL GetStaticInfo(HWND hStatic, TCHAR* szInfo, int maxLen)
{// Получаем информацию о статике 

    int staticID = GetDlgCtrlID(hStatic);
    RECT rc;
    GetWindowRect(hStatic, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    wsprintf(szInfo, TEXT("Статик %d, Размеры: %dx%d, Координаты: %d,%d"), staticID, width, height, rc.left, rc.top);

    return TRUE;
}

BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    { // 1 Закрыть, 2 Дескриптор статика и создание статика
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
        SetWindowText(hStatic2, TEXT("Нажата левая кнопка мыши"));
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
            MessageBox(hWnd, TEXT("Размер статика должен быть не менее 10x10"), TEXT("Предупреждение"), MB_OK | MB_ICONWARNING);
        }

        return TRUE;
    }
    case WM_RBUTTONDOWN:
    {
        // Получаем координаты щелчка правой кнопкой мыши
        int xClick = LOWORD(lParam);
        int yClick = HIWORD(lParam);

        //ПОСИК статика по коортинатам
        HWND hClickedStatic = ChildWindowFromPoint(hWnd, { xClick, yClick });

        if (hClickedStatic != NULL && IsWindow(hClickedStatic))
        {
            // Получаем информацию о статике и выводим её в заголовок окна
            TCHAR szStaticInfo[100];
            GetStaticInfo(hClickedStatic, szStaticInfo, sizeof(szStaticInfo) / sizeof(szStaticInfo[0]));
            SetWindowText(hWnd, szStaticInfo);
        }

        return TRUE;
    }
    case WM_LBUTTONDBLCLK:
    {
        // Получаем координаты двойного щелчка левой кнопкой мыши
        int xDoubleClick = LOWORD(lParam);
        int yDoubleClick = HIWORD(lParam);

        HWND hClickedStatic = ChildWindowFromPoint(hWnd, { xDoubleClick, yDoubleClick });

        if (hClickedStatic != NULL && IsWindow(hClickedStatic))
        {
            // Удаляем статик с наименьшим порядковым номером
            int staticID = GetDlgCtrlID(hClickedStatic);
            DestroyWindow(hClickedStatic);
            SetWindowText(hWnd, TEXT("Статик удален"));
        }
        return TRUE;
    }
    }
    return FALSE;
}











//1 и 2 (ПРАКТИКА) +
/*
#include <windows.h>
#include <tchar.h>
#include "resource.h"

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

HWND hStatic1, hStatic2;
TCHAR szCoordinates[20];
HINSTANCE hInst;
const int LEFT = 15, TOP = 110, WIDTH = 380, HEIGHT = 50;
int staticCounter = 1; // Добавлен счетчик для порядкового номера статика

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPTSTR lpszCmdLine, int nCmdShow)
{
    hInst = hInstance;
    // создаём главное окно приложения на основе модального диалога
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
}

BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CLOSE:
        EndDialog(hWnd, 0); // закрываем модальный диалог
        return TRUE;
        // WM_INITDIALOG - данное сообщение приходит после создания диалогового окна, но перед его отображением на экран
    case WM_INITDIALOG:
        hStatic1 = GetDlgItem(hWnd, IDC_STATIC1); // получаем дескриптор статика, размещенного на форме диалога
        // создаём статик с помощью CreateWindowEx
        hStatic2 = CreateWindowEx(0, TEXT("STATIC"), 0,
            WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER | WS_EX_CLIENTEDGE,
            LEFT, TOP, WIDTH, HEIGHT, hWnd, 0, hInst, 0);
        return TRUE;
    case WM_MOUSEMOVE:
        wsprintf(szCoordinates, TEXT("X=%d  Y=%d"), LOWORD(lParam), HIWORD(lParam)); // текущие координаты курсора мыши
        SetWindowText(hStatic1, szCoordinates); // строка выводится на статик
        return TRUE;
    case WM_LBUTTONDOWN:
        SetWindowText(hStatic2, TEXT("Нажата левая кнопка мыши"));
        return TRUE;
    case WM_LBUTTONUP:
    {
        // Получаем координаты отпускания левой кнопки мыши
        int xRelease = LOWORD(lParam);
        int yRelease = HIWORD(lParam);

        // Проверяем размер прямоугольника и выводим предупреждение при несоответствии
        if ((xRelease - LEFT) >= 10 && (yRelease - TOP) >= 10)
        {
            // Создаем новый статик с порядковым номером
            HWND hNewStatic = CreateWindowEx(0, TEXT("STATIC"), 0,
                WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER | WS_EX_CLIENTEDGE,
                LEFT, TOP, xRelease - LEFT, yRelease - TOP, hWnd, 0, hInst, 0);

            // Устанавливаем текст статика как порядковый номер
            TCHAR szStaticText[20];
            wsprintf(szStaticText, TEXT("%d"), staticCounter++);
            SetWindowText(hNewStatic, szStaticText);
        }
        else
        {
            MessageBox(hWnd, TEXT("Размер статика должен быть не менее 10x10"), TEXT("Предупреждение"), MB_OK | MB_ICONWARNING);
        }

        return TRUE;
    }
    case WM_RBUTTONDOWN:
        SetWindowText(hStatic2, TEXT("Нажата правая кнопка мыши"));
        return TRUE;
    }
    return FALSE;
}
*/