#include <Windows.h>
#include "resource.h"

#define IDC_FIRST_MOVE_CHECKBOX 1010
#define IDC_NEW_GAME_BUTTON 1011
#define IDC_BUTTON_BASE 1001

bool firstMoveCross = true;
bool gameInProgress = false;
void ResetGame(HWND hwnd);
void EnableButtons(HWND hwnd, bool enable);
void ClearButtons(HWND hwnd);
void HandleButtonClick(HWND hwnd, int buttonId);
void MakeComputerMove(HWND hwnd);
bool IsBoardFull(HWND hwnd);
bool CheckForWinner(HWND hwnd);
void SetSymbol(HWND button);
bool IsButtonEmpty(HWND button);
bool AreButtonsEqual(HWND button1, HWND button2, HWND button3);
bool IsButtonMarked(HWND button);

LRESULT CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON10: // New Game Button
            ResetGame(hwnd);
            break;
        case IDC_CHECK1: // First Move Checkbox
            firstMoveCross = IsDlgButtonChecked(hwnd, IDC_CHECK1) == BST_CHECKED;
            if (!gameInProgress) // Only update the starting player if the game hasn't started yet
                ResetGame(hwnd);
            break;
        case IDC_BUTTON1:
        case IDC_BUTTON2:
        case IDC_BUTTON3:
        case IDC_BUTTON4:
        case IDC_BUTTON5:
        case IDC_BUTTON6:
        case IDC_BUTTON7:
        case IDC_BUTTON8:
        case IDC_BUTTON9:
            HandleButtonClick(hwnd, LOWORD(wParam));
            break;
        }
        break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    }
    return FALSE;
}

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);

    return 0;
}

void ResetGame(HWND hwnd)
{
    gameInProgress = true;
    EnableButtons(hwnd, true);
    ClearButtons(hwnd);
    if (!firstMoveCross)
        MakeComputerMove(hwnd);
}

void EnableButtons(HWND hwnd, bool enable)
{
    for (int i = 1; i <= 9; ++i)
    {
        HWND button = GetDlgItem(hwnd, IDC_BUTTON_BASE + i);
        EnableWindow(button, enable);
    }
}

void ClearButtons(HWND hwnd)
{
    for (int i = 1; i <= 9; ++i)
    {
        HWND button = GetDlgItem(hwnd, IDC_BUTTON_BASE + i);
        SetWindowText(button, L"");
    }
}

void HandleButtonClick(HWND hwnd, int buttonId)
{
    HWND button = GetDlgItem(hwnd, buttonId);

    if (IsButtonEmpty(button))
    {
        SetSymbol(button);
        EnableButtons(hwnd, false);
        if (CheckForWinner(hwnd))
        {
            gameInProgress = false;
            MessageBox(hwnd, L"Game Over! Winner!", L"Game Over", MB_OK);
        }
        else if (IsBoardFull(hwnd))
        {
            gameInProgress = false;
            MessageBox(hwnd, L"Game Over! It's a Draw!", L"Game Over", MB_OK);
        }
        else
        {
            MakeComputerMove(hwnd);
        }
    }
}

bool IsButtonEmpty(HWND button)
{
    wchar_t buttonText[10];
    GetWindowText(button, buttonText, 10);
    return (wcscmp(buttonText, L"") == 0);
}

void SetSymbol(HWND button)
{
    wchar_t symbol[2] = { 0 };
    symbol[0] = (firstMoveCross) ? L'X' : L'O';
    SetWindowText(button, symbol);
}

bool CheckForWinner(HWND hwnd)
{
    // Check rows, columns, and diagonals for a winning combination
    for (int i = 0; i < 3; ++i)
    {
        // Check rows
        if (AreButtonsEqual(GetDlgItem(hwnd, IDC_BUTTON_BASE + i * 3 + 1), GetDlgItem(hwnd, IDC_BUTTON_BASE + i * 3 + 2), GetDlgItem(hwnd, IDC_BUTTON_BASE + i * 3 + 3)))
            return true;

        // Check columns
        if (AreButtonsEqual(GetDlgItem(hwnd, IDC_BUTTON_BASE + i + 1), GetDlgItem(hwnd, IDC_BUTTON_BASE + i + 4), GetDlgItem(hwnd, IDC_BUTTON_BASE + i + 7)))
            return true;
    }

    // Check diagonals
    if (AreButtonsEqual(GetDlgItem(hwnd, IDC_BUTTON_BASE + 1), GetDlgItem(hwnd, IDC_BUTTON_BASE + 5), GetDlgItem(hwnd, IDC_BUTTON_BASE + 9)))
        return true;
    if (AreButtonsEqual(GetDlgItem(hwnd, IDC_BUTTON_BASE + 3), GetDlgItem(hwnd, IDC_BUTTON_BASE + 5), GetDlgItem(hwnd, IDC_BUTTON_BASE + 7)))
        return true;

    return false;
}

bool IsBoardFull(HWND hwnd)
{
    for (int i = 1; i <= 9; ++i)
    {
        HWND button = GetDlgItem(hwnd, IDC_BUTTON_BASE + i);
        if (IsButtonEmpty(button))
        {
            // There is at least one empty button, so the board is not full
            return false;
        }
    }
    // All buttons are filled, so the board is full
    return true;
}

void MakeComputerMove(HWND hwnd)
{
    // This is a simple implementation that selects the first available empty button for the computer's move
    for (int i = 1; i <= 9; ++i)
    {
        HWND button = GetDlgItem(hwnd, IDC_BUTTON_BASE + i);
        if (!IsButtonMarked(button))
        {
            SetSymbol(button);
            EnableButtons(hwnd, false); // Disable buttons during computer's move
            if (CheckForWinner(hwnd))
            {
                gameInProgress = false;
                MessageBox(hwnd, L"Game Over! Winner!", L"Game Over", MB_OK);
            }
            else if (IsBoardFull(hwnd))
            {
                gameInProgress = false;
                MessageBox(hwnd, L"Game Over! It's a Draw!", L"Game Over", MB_OK);
            }
            else
            {
                EnableButtons(hwnd, true); // Re-enable buttons after computer's move
            }
            return;
        }
    }
}

bool AreButtonsEqual(HWND button1, HWND button2, HWND button3)
{
    wchar_t text1[2], text2[2], text3[2];
    GetWindowText(button1, text1, 2);
    GetWindowText(button2, text2, 2);
    GetWindowText(button3, text3, 2);

    return (wcscmp(text1, text2) == 0) && (wcscmp(text2, text3) == 0) && (wcscmp(text1, L"") != 0);
}

bool IsButtonMarked(HWND button)
{
    wchar_t buttonText[2];
    GetWindowText(button, buttonText, 2);
    return (wcscmp(buttonText, L"X") == 0) || (wcscmp(buttonText, L"O") == 0);
}