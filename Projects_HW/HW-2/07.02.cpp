#include "Windows.h"
#include "tchar.h"

//1
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void UpdateClickCount(HWND hwnd);

int leftClickCount = 0;
int rightClickCount = 0;
int middleClickCount = 0;

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
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
    wndClass.lpszClassName = L"Click Counter";
    wndClass.lpfnWndProc = WindowProc;

    if (!RegisterClassEx(&wndClass)) { return 0; }

    HWND hWnd = CreateWindowEx(
        0,
        L"MouseClickCounter",
        L"Mouse Click Counter",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 200,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    ShowWindow(hWnd, nShowCmd);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) { DispatchMessage(&msg); }

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //—◊≈“◊» »
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_LBUTTONDOWN:
        leftClickCount++;
        UpdateClickCount(hwnd);
        break;

    case WM_RBUTTONDOWN:
        rightClickCount++;
        UpdateClickCount(hwnd);
        break;

    case WM_MBUTTONDOWN:
        middleClickCount++;
        UpdateClickCount(hwnd);
        break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void UpdateClickCount(HWND hwnd)
{
    TCHAR title[100];
    _stprintf_s(title, _countof(title), _T("Left: %d , Right: %d , Middle: %d"), leftClickCount, rightClickCount, middleClickCount);
    SetWindowText(hwnd, title);
}


/*
//2
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    WNDCLASSEX wndClass;
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.hInstance = hInstance;
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hIconSm = NULL;
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = L"Window";
    wndClass.lpfnWndProc = WindowProc;

    if (!RegisterClassEx(&wndClass)) { return 0; }

    HWND hWnd = CreateWindowEx(
        0,
        L"CalculatorWindow",
        L"Calculator",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 300,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    ShowWindow(hWnd, nShowCmd);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) { DispatchMessage(&msg); }

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_LBUTTONDOWN:
        SetWindowText(hwnd, L"Calculator");//—Ã≈Õ¿ Õ¿«¬¿Õ»ﬂ
        break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
*/



/*
//3
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int GetWindowX(HWND hwnd);
int GetWindowY(HWND hwnd);
int GetScreenWidth();
int GetScreenHeight();

int windowWidth = 300;
int windowHeight = 300;

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    WNDCLASSEX wndClass;
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.hInstance = hInstance;
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hIconSm = NULL;
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = L"MyWindow";
    wndClass.lpfnWndProc = WindowProc;

    if (!RegisterClassEx(&wndClass)) { return 0; }

    HWND hWnd = CreateWindowEx(
        0,
        L"MyWindow",
        L"Resizable Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowWidth, windowHeight,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    ShowWindow(hWnd, nShowCmd);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) { DispatchMessage(&msg); }

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_RETURN:
            //ËÁÌ‡˜‡Î¸ÌÓ ‚‚ÂıÛ ÒÎÂ‚‡
            SetWindowPos(hwnd, HWND_TOP, 0, 0, windowWidth, windowHeight, SWP_SHOWWINDOW);
            break;

        case VK_LEFT:
            // œ≈–≈ƒ¬»Õ”“‹ ¬À≈¬Œ GetWindowX - 10
            SetWindowPos(hwnd, NULL, max(0, GetWindowX(hwnd) - 10), GetWindowY(hwnd), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            break;

        case VK_RIGHT:
            //¬œ–¿¬Œ
            SetWindowPos(hwnd, NULL, min(GetScreenWidth() - windowWidth, GetWindowX(hwnd) + 10), GetWindowY(hwnd), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            break;

        case VK_UP:
            //¬¬≈–’
            SetWindowPos(hwnd, NULL, GetWindowX(hwnd), max(0, GetWindowY(hwnd) - 10), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            break;

        case VK_DOWN:
            //¬Õ»«
            SetWindowPos(hwnd, NULL, GetWindowX(hwnd), min(GetScreenHeight() - windowHeight, GetWindowY(hwnd) + 10), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            break;
        }
        break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int GetWindowX(HWND hwnd) 
{// ŒŒ–ƒ»Õ¿“€ ƒÀﬂ »«Ã≈Õ≈Õ»ﬂ œŒ √Œ–»«ŒÕ“¿À»
    RECT rect;
    GetWindowRect(hwnd, &rect);
    return rect.left;
}

int GetWindowY(HWND hwnd) 
{//œŒ ¬≈–“» ¿À.
    RECT rect;
    GetWindowRect(hwnd, &rect);
    return rect.top;
}

//ƒÀﬂ √–¿Õ»÷
int GetScreenWidth() { return GetSystemMetrics(SM_CXSCREEN); }

int GetScreenHeight(){ return GetSystemMetrics(SM_CYSCREEN); }
*/