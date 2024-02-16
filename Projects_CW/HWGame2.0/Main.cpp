
#include "resource.h"

#include <windows.h>
#include <CommCtrl.h>

#include <algorithm>
#include <cstdlib>

#include <ctime>
#include <vector>
#include <string>


const int BUTTON_COUNT = 16;

HWND buttons[BUTTON_COUNT];
HWND progressBar;
HWND listBox;
HWND editTimer;
HWND spinTimer;

std::vector<int> numberList;
std::vector<int> clickedNumbers;

int score = 0;
int targetScore = 30;
int gameTime = 60;
int remainingTime = gameTime;
bool gameInProgress = false;
int currentNumber = 0;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
void InitializeGame(HWND hwnd);
void StartNewGame(HWND hwnd);
void UpdateScore(HWND hwnd);
void UpdateRemainingTime(HWND hwnd);
void CheckButtonClick(HWND hwnd, int buttonIndex);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
    wc.lpszClassName = L"MyWindowClass";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(L"MyWindowClass", L"Number Clicker Game", WS_OVERLAPPEDWINDOW, 100, 100, 400, 300, NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    InitializeGame(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}



UINT_PTR timerID;
bool timerRunning = false;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) 
{
    switch (message) 
    {
    case WM_CREATE:
        //ТАЙМЕР
        timerID = SetTimer(hwnd, 1, 1000, NULL);
        timerRunning = true;

        //ЗАПИСЫВАЕТСЯ В ЭДИТ!!
        editTimer = CreateWindowEx(0, L"EDIT", L"60", WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_LEFT, 320, 125, 50, 20, hwnd, (HMENU)IDC_EDIT_TIMER, NULL, NULL);
        break;

    case WM_COMMAND:
        for (int i = 0; i < BUTTON_COUNT; ++i) 
        {
            if ((HWND)lparam == buttons[i]) 
            {
                CheckButtonClick(hwnd, i);
                break;
            }
        }
        if (LOWORD(wparam) == IDC_BUTTON17) { StartNewGame(hwnd); }
        break;

    case WM_TIMER:
        if (wparam == timerID) 
        {
            if (remainingTime > 0) 
            {
                --remainingTime;
                UpdateRemainingTime(hwnd);

                //ОБНОВЛЕНИЯ ТЕКСТА В ЭДИТЕ ТАЙМЕРА
                SetDlgItemInt(hwnd, IDC_EDIT_TIMER, remainingTime, FALSE);
            }
            else 
            {
                KillTimer(hwnd, 1); // КОГДА 0 ТО ОБНОВЛЯЕМ
                timerRunning = false;
                MessageBox(hwnd, L"Время вышло! Игра окончена.", L"Поражение", MB_OK | MB_ICONERROR);
                StartNewGame(hwnd);
            }
        }
        break;

    case WM_DESTROY:
        KillTimer(hwnd, 1); //ОСТОНОВКА ТАЙМЕРА ПЕРЕД ЗАКРЫТИЕМ
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, message, wparam, lparam);
    }

    return 0;
}

void InitializeGame(HWND hwnd) 
{
    for (int i = 0; i < BUTTON_COUNT; ++i) 
    {
        buttons[i] = CreateWindow(L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 20 + (i % 4) * 80, 40 + (i / 4) * 50, 50, 30, hwnd, (HMENU)(i + 1), NULL, NULL);
    }

    progressBar = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE, 60, 240, 216, 14, hwnd, (HMENU)IDC_PROGRESS1, NULL, NULL);

    listBox = CreateWindowEx(0, L"LISTBOX", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_VSCROLL, 320, 40, 60, 90, hwnd, (HMENU)IDC_LIST1, NULL, NULL);

    spinTimer = CreateWindowEx(0, UPDOWN_CLASS, NULL, WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS, 110, 210, 16, 20, hwnd, (HMENU)IDC_SPIN_TIMER, NULL, NULL);
    SendMessage(spinTimer, UDM_SETBUDDY, (WPARAM)editTimer, 0);
    SendMessage(spinTimer, UDM_SETRANGE, 0, MAKELPARAM(300, 1));
    StartNewGame(hwnd);
}

void StartNewGame(HWND hwnd) 
{
    if (timerRunning) 
    {
        KillTimer(hwnd, 1); //ОСТОНОВКА ПРЕДЫДУЩЕГО ТАЙМЕРА
        timerRunning = false;
    }

    numberList.clear();
    clickedNumbers.clear();
    currentNumber = 0;
    gameInProgress = true;

//ВРЕЕЕЕМЯ
    remainingTime = 60;
    UpdateRemainingTime(hwnd);

    for (int i = 0; i < BUTTON_COUNT; ++i) 
    {
        int randomNumber = rand() % 100;
        numberList.push_back(randomNumber);
        SetWindowText(buttons[i], std::to_wstring(randomNumber).c_str());
    }

    std::sort(numberList.begin(), numberList.end());

    score = 0;
    targetScore = 30;
    UpdateScore(hwnd);

    SendMessage(progressBar, PBM_SETPOS, 0, 0);
    SendMessage(listBox, LB_RESETCONTENT, 0, 0);

//ИНТЕРВАЛ 1 СЕКУНДА
    timerID = SetTimer(hwnd, 1, 1000, NULL);
    timerRunning = true;
}

void UpdateScore(HWND hwnd) 
{
    SetDlgItemInt(hwnd, IDC_EDIT1, score, FALSE);
}

void UpdateRemainingTime(HWND hwnd) 
{
    SetDlgItemInt(hwnd, IDC_EDIT2, remainingTime, FALSE);
}

void CheckButtonClick(HWND hwnd, int buttonIndex) 
{
    wchar_t buttonText[10];
    GetWindowText(buttons[buttonIndex], buttonText, 10);
    int buttonNumber = _wtoi(buttonText);

    if (gameInProgress && buttonNumber == numberList[currentNumber]) 
    {
        ++score;
        ++currentNumber;

        if (score == targetScore) 
        {
            targetScore += 5;
            remainingTime += 5;
        }

        clickedNumbers.push_back(buttonNumber);

        UpdateScore(hwnd);

        int progressValue = static_cast<int>((static_cast<double>(score) / BUTTON_COUNT) * 100.0);
        SendMessage(progressBar, PBM_SETPOS, progressValue, 0);

        wchar_t numStr[10];
        _itow_s(buttonNumber, numStr, 10);
        SendMessage(listBox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(numStr));

        if (score == BUTTON_COUNT) 
        {
            MessageBox(hwnd, L"Поздравляем! Вы выиграли!", L"Победа", MB_OK | MB_ICONINFORMATION);
            StartNewGame(hwnd);
        }
    }
    else 
    {
        MessageBox(hwnd, L"Вы ошиблись. Игра окончена.", L"Поражение", MB_OK | MB_ICONERROR);
        StartNewGame(hwnd);
    }
}





















/*
HWND hDlg;
HWND buttons[16];
int sequence[16];
int currentNumber = 0;
int score = 0;
int timeLimit = 30; // Время в секундах
bool gameInProgress = false;

// Прототипы функций
void InitializeGame();
void StartNewGame();
void UpdateScore();
void CheckButtonPress(int buttonID);

// Функция обработки сообщений
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        // Инициализация игры
        InitializeGame();
        return TRUE;

    case WM_COMMAND:
        // Обработка нажатий на кнопки
        for (int i = 0; i < 16; ++i) {
            if (LOWORD(wParam) == IDC_BUTTON1 + i) {
                CheckButtonPress(i);
                break;
            }
        }

        // Новая игра
        if (LOWORD(wParam) == IDC_BUTTON17) {
            StartNewGame();
        }
        return TRUE;

    case WM_CLOSE:
        EndDialog(hDlg, 0);
        return TRUE;
    }

    return FALSE;
}

// Инициализация игры
void InitializeGame() {
    srand(time(NULL));

    // Получение дескриптора диалогового окна и кнопок
    hDlg = GetDlgItem(hDlg, IDD_DIALOG1);
    for (int i = 0; i < 16; ++i) {
        buttons[i] = GetDlgItem(hDlg, IDC_BUTTON1 + i);
    }

    // Установка случайных чисел на кнопки
    for (int i = 0; i < 16; ++i) {
        sequence[i] = rand() % 101;
        SetWindowText(buttons[i], std::to_wstring(sequence[i]).c_str());
    }

    // Начало новой игры
    StartNewGame();
}

// Начало новой игры
void StartNewGame() {
    gameInProgress = true;
    currentNumber = 0;
    score = 0;
    UpdateScore();

    // Сброс стилей кнопок
    for (int i = 0; i < 16; ++i) {
        EnableWindow(buttons[i], TRUE);
    }

    // Запуск таймера
    SetTimer(hDlg, 1, 1000, NULL);
}

// Обновление счета на форме
void UpdateScore() {
    SetDlgItemInt(hDlg, IDC_EDIT1, score, FALSE);
}

// Проверка нажатия на кнопку
void CheckButtonPress(int buttonID) {
    if (gameInProgress && sequence[buttonID] == currentNumber) {
        // Верное нажатие
        currentNumber++;
        score++;

        // Отключение кнопки и проверка завершения игры
        if (currentNumber == 16) {
            MessageBox(hDlg, L"Вы выиграли!", L"Поздравляем!", MB_OK | MB_ICONINFORMATION);
            gameInProgress = false;
            KillTimer(hDlg, 1);
        }
        else {
            EnableWindow(buttons[buttonID], FALSE);
        }

        UpdateScore();
    }
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    INT_PTR result = DialogBoxParamW(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc, 0);
    return 0;
}

*/

