#include <Windows.h>
#include <iostream>
#include <vector>

#include "string"

#include "resource.h"

// ���������� ����������
HWND hList;
std::vector<LONG> globalArray(10);
LONG globalIndex = -1;

// ������� ��� ���������� ������� ���������� ������� �� 1 �� 10
DWORD WINAPI InputThread(LPVOID lpParam) {
    for (int i = 0; i < 10; ++i) {
        int randomNumber = rand() % 90 + 1; // ��������� ������ ���������� ������� �� 1 �� 10

        // ������� ������������ ��� ������ � ������ � ������
        globalArray[i] = randomNumber;
        InterlockedExchange(&globalIndex, i);

        Sleep(1000); // �������� ������
    }

    // �������� ������ ����� ������ ���� 10 �����
    InterlockedExchange(&globalIndex, 0);

    return 0;
}

// ������� ��� ������ ��������� ������� � ������
DWORD WINAPI OutputThread(LPVOID lpParam) {
    while (globalIndex != 0) {
        // ���� ���������� �������������
        Sleep(0);
    }

    // ������� ��� 10 ����� �� ������� � ������
    for (int i = 0; i < 10; ++i) {
        std::wstring valueStr = std::to_wstring(globalArray[i]);
        const TCHAR* buffer = valueStr.c_str();

        SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)buffer);
        Sleep(1000); // �������� ������
    }

    return 0;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // �������� ����������� ����
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, [](HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) -> INT_PTR {
        switch (message) {
        case WM_INITDIALOG:
            hList = GetDlgItem(hwnd, IDC_LIST1);

            // �������� ������ �����
            CreateThread(NULL, 0, InputThread, NULL, 0, NULL);

            // �������� ������ ������
            CreateThread(NULL, 0, OutputThread, NULL, 0, NULL);
            return TRUE;

        case WM_CLOSE:
            EndDialog(hwnd, 0);
            return TRUE;
        }
        return FALSE;
        });

    return 0;
}
