#include "Windows.h"
#include "tchar.h"
#include <string>
using std::wstring;

// ���������� ���������� ��� ���������� ��������� ����
bool isMoving = true;       // ����, ����������� �� ��, ��������� �� ����
int windowSpeedX = 2;       // �������� ����������� ���� �� �����������
int windowSpeedY = 2;       // �������� ����������� ���� �� ���������
int windowWidth = 300;      // ������ ����
int windowHeight = 300;     // ������ ����

// ��������� ������� ��������� �������
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void KeyDownHandler(HWND hwnd, WPARAM wParam, LPARAM lParam);
void KeyUpHandler(HWND hwnd, WPARAM wParam, LPARAM lParam);
void WmCharHandler(HWND hwnd, WPARAM wParam, LPARAM lParam);
void MoveWindowIfNeeded(HWND hwnd);
void OnTimer(HWND hwnd);

// ����� ����� � ����������
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    WCHAR className[] = L"Window";

    WNDCLASSEX wndClass;
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wndClass.hInstance = hInstance;
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hIconSm = NULL;
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = className;

    wndClass.lpfnWndProc = WindowProc;

    if (!RegisterClassEx(&wndClass)) {
        return 0;
    }

    // ������� ����
    HWND hWnd = CreateWindowEx(
        0,
        className,
        L"LABA-1",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0,// ����� ����
        windowWidth, windowHeight,// ������ � ������ ����
        NULL,
        NULL,
        hInstance,
        NULL
    );

    ShowWindow(hWnd, nShowCmd);
    UpdateWindow(hWnd);

    SetTimer(hWnd, 1, 10, NULL); // ������

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0); //������� ���� 
        break;

    case WM_KEYDOWN:
        KeyDownHandler(hwnd, wParam, lParam);
        break;

    case WM_KEYUP:
        KeyUpHandler(hwnd, wParam, lParam);
        break;

    case WM_CHAR:
        WmCharHandler(hwnd, wParam, lParam);
        break;

    case WM_TIMER:
        OnTimer(hwnd);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void KeyDownHandler(HWND hwnd, WPARAM wParam, LPARAM lParam)
{//���� ����������
    int vkey = wParam;

    if (vkey == VK_ESCAPE)
    {
        isMoving = false; // ���������� ��� �������
    }
}

void KeyUpHandler(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
}

void WmCharHandler(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
}

void MoveWindowIfNeeded(HWND hwnd)
{
    if (isMoving)
    {
        RECT rect;
        GetWindowRect(hwnd, &rect); //��������

        int screenWidth = GetSystemMetrics(SM_CXSCREEN); //���������� ������ ������ �������
        int screenHeight = GetSystemMetrics(SM_CYSCREEN); //������

        //�� ������!
        if (rect.left + windowSpeedX < 0 || rect.left + windowWidth + windowSpeedX > screenWidth)
        {
            windowSpeedX = -windowSpeedX; // ��������� �������� ���� ����
        }

        //�� ������!!! ����� �� �� �������
        if (rect.top + windowSpeedY < 0 || rect.top + windowHeight + windowSpeedY > screenHeight)
        {
            windowSpeedY = -windowSpeedY; //���������� �������� ���� ����
        }

        //������������� ����, ���������� ����, ����� ���������� X ������ �������� ���� ����,����� ���������� Y ������ �������� ���� ����, ����� ��� ������ �� �������� � ������ ��������� 
        SetWindowPos(hwnd, NULL, rect.left + windowSpeedX, rect.top + windowSpeedY, 0, 0, SWP_NOSIZE | SWP_NOZORDER); // ��������
    }
}


void OnTimer(HWND hwnd)
{//�������������� ���������
    MoveWindowIfNeeded(hwnd);
}
