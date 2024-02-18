#include <windows.h>
#include <string>
#include <vector>
#include <ctime>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInst;
std::vector<std::wstring> board(9, L"");
bool playerTurn = true;
bool playerStarts = true; //ДЛЯ ТОГО ЧТО БЫ ОПРЕДЕЛИТЬ НАЧИНАЕТ ЛИ ИГРОК

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
    hInst = hInstance;

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"TicTacToeClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) 
    {
        MessageBox(NULL, L"Вызов RegisterClassEx завершился неудачно!", L"TicTacToe", NULL);
        return 1;
    }

    HWND hWnd = CreateWindow(L"TicTacToeClass", L"TicTacToe", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 320, 240, NULL, NULL, hInstance, NULL);

    if (!hWnd) 
    {
        MessageBox(NULL, L"Вызов CreateWindow завершился неудачно!", L"TicTacToe", NULL);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

bool CheckWinner() 
{//ПРОВЕРКА ВЫИГРАЛ ЛИ КОСПЬЮТЕР ИЛИ ИГРОК

    const std::vector<std::vector<int>> winningCombinations = 
    {//СТРОКИ, СТОЛБЦЫ, ДИАЛОГИ
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, 
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, 
        {0, 4, 8}, {2, 4, 6}            
    };

    for (const auto& combination : winningCombinations) 
    {//ПРОВЕРКА НА КОМБИНАЦИИ СВЕРХУ, ПРОШЛА ЛИ 
        if (board[combination[0]] != L"" && board[combination[0]] == board[combination[1]] && board[combination[1]] == board[combination[2]]) {
            return true; 
        }
    }

    return false;
}


void ComputerMove() 
{//ДЛЯ ОПРЕДЕЛЕНИЯ ДОСТУПНЫХ ХОДОВ КОМПА
    std::vector<int> availableMoves;
    for (int i = 0; i < 9; ++i) 
    {
        if (board[i] == L"") { availableMoves.push_back(i); }
    }

    //СЛУЧАЙНЫЙ ВЫБОР
    int randomIndex = rand() % availableMoves.size();
    int computerMoveIndex = availableMoves[randomIndex];
    board[computerMoveIndex] = L"O"; //КОМП

//ДЛЯ ТЕКСТА
    HWND hwndButton = GetDlgItem(GetActiveWindow(), IDC_BUTTON1 + computerMoveIndex);
    SetWindowText(hwndButton, L"O");

//КОМП ВЫИГРАЛ ??
    if (CheckWinner()) 
    {
        MessageBox(GetActiveWindow(), L"Компьютер побеждает!", L"Конец игры", MB_OK);
        PostQuitMessage(0);
    }

//ЕСЛИ ВСЯ ДОСКА ЗАПОЛНЕНА - НИЧЬЯ!!!!!!!!!!!!
    bool isBoardFull = true;
    for (const auto& cell : board) 
    {
        if (cell == L"") 
        {
            isBoardFull = false;
            break;
        }
    }
    if (isBoardFull) 
    {
        MessageBox(GetActiveWindow(), L"Ничья!", L"Конец игры", MB_OK);
        PostQuitMessage(0);
    }

    playerTurn = true; //ИГРОК!!!!!!!!
}

void OnButtonClick(int buttonId) 
{//КЛИК!!
    if (playerTurn) 
    {
        //ИГРОК!!
        int boardIndex = buttonId - IDC_BUTTON1;
        if (board[boardIndex] == L"") 
        {
            board[boardIndex] = L"X";
            HWND hwndButton = GetDlgItem(GetActiveWindow(), buttonId);
            SetWindowText(hwndButton, L"X");

            if (CheckWinner()) 
            {//ПРОВЕРКА ВЫИГРАША
                MessageBox(GetActiveWindow(), L"Игрок побеждает!", L"Конец игры", MB_OK);
                PostQuitMessage(0);
            }

            playerTurn = false; //КОМП!!!
            ComputerMove(); //ХОД
        }
    }
}


void StartNewGame() 
{ //НАЧАЛО НОВОЙ ИГРЫ
    for (auto& cell : board) { cell = L""; }

    for (int i = IDC_BUTTON1; i <= IDC_BUTTON9; ++i) 
    {
        HWND hwndButton = GetDlgItem(GetActiveWindow(), i);
        SetWindowText(hwndButton, L"");
    }

    playerTurn = playerStarts;

    if (!playerTurn) {
        ComputerMove();
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
    case WM_CREATE:
    {
       //СМС ОБ НАЧАЛЕ ИГРЫ, КТО ПЕРВЫЙ
        int messageBoxResult = MessageBox(hWnd,
            L"Нажмите 'Начать игру' для начала игры. \n\nЕсли вы хотите, чтобы начал компьютер, нажмите 'OK'."
            L"\nЕсли вы хотите начать сами, нажмите 'Отмена'.",
            L"Выбор первого хода",
            MB_OKCANCEL | MB_ICONINFORMATION);


        CreateWindow(L"BUTTON", L"", WS_VISIBLE | WS_CHILD, 82, 120, 41, 30, hWnd, (HMENU)IDC_BUTTON1, hInst, NULL);
        CreateWindow(L"BUTTON", L"", WS_VISIBLE | WS_CHILD, 178, 120, 41, 30, hWnd, (HMENU)IDC_BUTTON2, hInst, NULL);
        CreateWindow(L"BUTTON", L"", WS_VISIBLE | WS_CHILD, 130, 120, 41, 30, hWnd, (HMENU)IDC_BUTTON3, hInst, NULL);
        CreateWindow(L"BUTTON", L"", WS_VISIBLE | WS_CHILD, 82, 48, 41, 30, hWnd, (HMENU)IDC_BUTTON4, hInst, NULL);
        CreateWindow(L"BUTTON", L"", WS_VISIBLE | WS_CHILD, 178, 48, 41, 30, hWnd, (HMENU)IDC_BUTTON5, hInst, NULL);
        CreateWindow(L"BUTTON", L"", WS_VISIBLE | WS_CHILD, 130, 48, 41, 30, hWnd, (HMENU)IDC_BUTTON6, hInst, NULL);
        CreateWindow(L"BUTTON", L"", WS_VISIBLE | WS_CHILD, 82, 84, 41, 30, hWnd, (HMENU)IDC_BUTTON7, hInst, NULL);
        CreateWindow(L"BUTTON", L"", WS_VISIBLE | WS_CHILD, 178, 84, 41, 30, hWnd, (HMENU)IDC_BUTTON8, hInst, NULL);
        CreateWindow(L"BUTTON", L"", WS_VISIBLE | WS_CHILD, 130, 84, 41, 30, hWnd, (HMENU)IDC_BUTTON9, hInst, NULL);

        CreateWindow(L"BUTTON", L"New Game", WS_VISIBLE | WS_CHILD, 110, 10, 80, 30, hWnd, (HMENU)IDC_NEW_GAME_BUTTON, hInst, NULL);

        //КТО ПЕРВЫ??????
        if (messageBoxResult == IDOK) {
            playerTurn = false; //КОМП
            ComputerMove(); //ХОД
        }
        else {
            playerTurn = true; //ИГРОК
        }
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDC_BUTTON1:
        case IDC_BUTTON2:
        case IDC_BUTTON3:
        case IDC_BUTTON4:
        case IDC_BUTTON5:
        case IDC_BUTTON6:
        case IDC_BUTTON7:
        case IDC_BUTTON8:
        case IDC_BUTTON9:
            OnButtonClick(wmId);
            break;
        case IDC_NEW_GAME_BUTTON:
            playerStarts = !playerStarts; //ПЕРЕКЛЮЧЕНИЯ ИГРОКА
            StartNewGame();
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
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
