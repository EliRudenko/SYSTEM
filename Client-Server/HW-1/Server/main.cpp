#include <iostream>
#include <string>
#include "WinSock2.h"
#include <ws2tcpip.h>
#include "resource.h"

#pragma comment(lib, "Ws2_32.lib")

HWND hStartButton, hMessageEdit, hInputEdit, hSendButton, hEndButton;
SOCKET _socket, acceptSocket;
const int MAXSTRLEN = 255;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hWnd;
    MSG msg;

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(MAIN_DIALOG), NULL, (DLGPROC)WndProc);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    WSACleanup();

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static char buffer[MAXSTRLEN]; // Buffer to store user input

    switch (message)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_START:
        {
            WSADATA wsaData;
            sockaddr_in addr;

            WSAStartup(MAKEWORD(2, 2), &wsaData);
            _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

            addr.sin_family = AF_INET;
            inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
            addr.sin_port = htons(20000);

            bind(_socket, (SOCKADDR*)&addr, sizeof(addr));
            listen(_socket, 1);

            std::cout << "Server is started\n";

            acceptSocket = accept(_socket, NULL, NULL);
            break;
        }
        case IDC_END:
            closesocket(acceptSocket);
            closesocket(_socket);
            WSACleanup();
            DestroyWindow(hWnd);
            break;
        case IDC_SEND:
        {
            // Get user input from the input edit control
            GetWindowTextA(hInputEdit, buffer, MAXSTRLEN);

            // Send the user input to the client
            send(acceptSocket, buffer, strlen(buffer), 0);

            // Optionally, you can handle the sent message here (e.g., display it)
            std::cout << "Sent: " << buffer << std::endl;

            // Clear the input edit control
            SetWindowTextA(hInputEdit, "");
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

