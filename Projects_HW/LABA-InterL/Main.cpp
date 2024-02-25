#include <Windows.h>
#include <iostream>
#include <vector>

#include "string"

#include "resource.h"

// Глобальные переменные
HWND hList;
std::vector<LONG> globalArray(10);
LONG globalIndex = -1;

// Функция для заполнения массива рандомными числами от 1 до 10
DWORD WINAPI InputThread(LPVOID lpParam) {
    for (int i = 0; i < 10; ++i) {
        int randomNumber = rand() % 90 + 1; // Заполняем массив случайными числами от 1 до 10

        // Простое присваивание для записи в массив и индекс
        globalArray[i] = randomNumber;
        InterlockedExchange(&globalIndex, i);

        Sleep(1000); // Имитация работы
    }

    // Обнуляем индекс после записи всех 10 чисел
    InterlockedExchange(&globalIndex, 0);

    return 0;
}

// Функция для вывода элементов массива в список
DWORD WINAPI OutputThread(LPVOID lpParam) {
    while (globalIndex != 0) {
        // Ждем завершения инициализации
        Sleep(0);
    }

    // Выводим все 10 чисел из массива в список
    for (int i = 0; i < 10; ++i) {
        std::wstring valueStr = std::to_wstring(globalArray[i]);
        const TCHAR* buffer = valueStr.c_str();

        SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)buffer);
        Sleep(1000); // Имитация вывода
    }

    return 0;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Создание диалогового окна
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, [](HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) -> INT_PTR {
        switch (message) {
        case WM_INITDIALOG:
            hList = GetDlgItem(hwnd, IDC_LIST1);

            // Создание потока ввода
            CreateThread(NULL, 0, InputThread, NULL, 0, NULL);

            // Создание потока вывода
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
