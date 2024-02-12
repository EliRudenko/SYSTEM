#include <Windows.h>
#include "resource.h"

HINSTANCE hInstance;

const int STATIC_WIDTH = 50;
const int STATIC_HEIGHT = 50;
const int MOVEMENT_SPEED = 2;

HWND hStatic;
RECT rcDialog;

int targetX, targetY;

void StartMoving(HWND hWnd);
bool IsMouseOver(HWND hWn);
void MoveStatic(HWND hWnd);

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//cvc
#define WM_MOVE_STATIC (WM_USER + 1)

void MoveStatic(HWND hWnd) 
{
    // Получаем текущие координаты статика
    RECT rcStatic;
    GetWindowRect(hStatic, &rcStatic);
    ScreenToClient(GetParent(hStatic), reinterpret_cast<LPPOINT>(&rcStatic.left));
    ScreenToClient(GetParent(hStatic), reinterpret_cast<LPPOINT>(&rcStatic.right));

    //ПО ИДЕЕ ТО ЧТО В ПРОШЛОМ ДЗ С ДВИЖЕНИЕМ БЫЛО
    if (IsMouseOver(hStatic)) 
    {
    //ВЕКТОР ДВИЖЕНИЯ
        int deltaX = targetX - rcStatic.left;
        int deltaY = targetY - rcStatic.top;

        //НОВЫЕ КООРДИНАТЫ И СКОРОСТЬ
        int newX = rcStatic.left + (deltaX > 0 ? min(MOVEMENT_SPEED, deltaX) : max(-MOVEMENT_SPEED, deltaX));
        int newY = rcStatic.top + (deltaY > 0 ? min(MOVEMENT_SPEED, deltaY) : max(-MOVEMENT_SPEED, deltaY));

        //ДВИЖ
        SetWindowPos(hStatic, NULL, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        
        //ПРОВЕРОЧКИ
        if (abs(newX - targetX) <= MOVEMENT_SPEED && abs(newY - targetY) <= MOVEMENT_SPEED) 
        {
            //НОВЫЕ КООРДИНАТЫ
            StartMoving(hWnd);

            // Отправляем пользовательское сообщение WM_MOVE_STATIC для обновления статика
            PostMessage(hWnd, WM_MOVE_STATIC, 0, 0);
        }
    }
}

void StartMoving(HWND hWnd) 
{
    //НОВЫЕ КООРДИНАТЫ
    targetX = rand() % (rcDialog.right - STATIC_WIDTH - rcDialog.left);
    targetY = rand() % (rcDialog.bottom - STATIC_HEIGHT - rcDialog.top);
}

bool IsMouseOver(HWND hWnd) 
{
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(hWnd, &cursorPos);

    RECT rc;
    GetClientRect(hWnd, &rc);

    return PtInRect(&rc, cursorPos);
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) 
{
    ::hInstance = hInstance;
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, DlgProc);
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    switch (uMsg) 
    {
    case WM_INITDIALOG: 
    {
        hStatic = GetDlgItem(hwnd, IDC_PICTURE);
        GetClientRect(hwnd, &rcDialog);

        HBITMAP hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
        SendMessage(hStatic, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);

        HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
        SendDlgItemMessage(hwnd, IDC_BUTTON2, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);

        SetTimer(hwnd, 1, 50, NULL); // Таймер с интервалом 50 миллисекунд
        StartMoving(hwnd);  // Начинаем движение сразу после инициализации
        break;
    }

    case WM_TIMER: 
    {
        MoveStatic(hwnd);
        break;
    }

    case WM_MOVE_STATIC: 
    {
        MoveStatic(hwnd);
        break;
    }

    case WM_COMMAND: 
    {
        if (LOWORD(wParam) == IDC_BUTTON1 && HIWORD(wParam) == BN_CLICKED) 
        {
            EnableWindow(GetDlgItem(hwnd, IDC_BUTTON2), FALSE);
            MessageBox(NULL, L"Clicked", L"", MB_OK);
        }
        break;
    }

    case WM_CLOSE: 
    {
        EndDialog(hwnd, 0);
        return TRUE;
    }
    }

    return FALSE;
}
