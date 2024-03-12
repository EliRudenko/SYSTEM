#include <iostream>
#include <string>
#include "WinSock2.h"
#include <ws2tcpip.h>
#include "resource.h"

#pragma comment(lib, "Ws2_32.lib")

const int MAXSTRLEN = 255;

SOCKET _socket;

void Connect()
{
    WSADATA wsaData;
    sockaddr_in addr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize Winsock\n";
        return;
    }

    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    addr.sin_port = htons(20000);

    connect(_socket, (SOCKADDR*)&addr, sizeof(addr));
}

void Disconnect()
{
    closesocket(_socket);
    WSACleanup();
}

void SendMessage(const char* message)
{
    send(_socket, message, strlen(message), 0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_CONNECT:
            Connect();
            break;
        case IDC_DISCONNECT:
            Disconnect();
            DestroyWindow(hWnd);
            break;
        case IDC_SEND:
        {
            char buffer[MAXSTRLEN];
            GetWindowTextA(GetDlgItem(hWnd, IDC_INPUT), buffer, MAXSTRLEN);
            SendMessage(buffer);
            SetWindowTextA(GetDlgItem(hWnd, IDC_INPUT), "");
            break;
        }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = {};
    HWND hWnd;
    MSG msg;

    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"ClientClass";

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(MAIN_DIALOG), nullptr, (DLGPROC)WndProc);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
