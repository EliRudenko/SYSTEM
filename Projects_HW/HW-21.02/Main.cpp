#include <windows.h>
#include <vector>
#include <string>
#include <tchar.h>
#include <fstream>
#include "resource.h"

BOOL CALLBACK MenuDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SignInDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SignUpDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


std::vector<std::pair<std::wstring, std::wstring>> userDatabase; //—Œ’–¿Õ≈Õ»≈ ÀŒ√»Õ¿ » œ¿–ŒÀﬂ


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MENU_DIALOG), NULL, (DLGPROC)MenuDialogProc);
}

BOOL CALLBACK MenuDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) 
        {
        case IDC_SIGNIN_BUTTON:
            EndDialog(hwnd, 0); //«¿ –≤“‹ Ã≈Õﬁ
            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SIGNIN_DIALOG), NULL, (DLGPROC)SignInDialogProc);
            return TRUE;

        case IDC_SIGNUP_BUTTON:
            EndDialog(hwnd, 0); //«¿ –≤“‹ Ã≈Õﬁ
            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SIGNUP_DIALOG), NULL, (DLGPROC)SignUpDialogProc);
            return TRUE;
        }
        break;

    case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
    }
    return FALSE;
}

// SignIn
BOOL CALLBACK SignInDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) 
        {
        case IDOK: 
        {
            //ÀŒ√»Õ » œ¿–ŒÀ‹, ¬¬Œƒ
            TCHAR szLogin[256], szPassword[256];
            GetDlgItemText(hwnd, IDC_LOGIN_EDIT, szLogin, 256);
            GetDlgItemText(hwnd, IDC_PASSWORD_EDIT, szPassword, 256);

            //œ–Œ¬≈– ¿
            bool bFound = false;
            for (const auto& user : userDatabase) 
            {
                if (user.first == szLogin && user.second == szPassword) 
                {
                    bFound = true;
                    break;
                }
            }

            if (bFound) 
            {
                MessageBox(hwnd, L"Login Successful", L"Success", MB_OK | MB_ICONINFORMATION);
                EndDialog(hwnd, 0); //«¿ –≤“‹ ¿¬“Œ–»«¿÷»ﬁ
                DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MENU_DIALOG), NULL, (DLGPROC)MenuDialogProc);
            }
            else 
            {
                MessageBox(hwnd, L"Login Failed", L"Error", MB_OK | MB_ICONERROR);
                EndDialog(hwnd, 0); // Œ“ –≤“‹ SignUp
                DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SIGNUP_DIALOG), NULL, (DLGPROC)SignUpDialogProc);
            }
            return TRUE;
        }
        }
        break;

    case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
    }
    return FALSE;
}

// SignUp
BOOL CALLBACK SignUpDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) 
        {
        case IDOK: 
        {
            // ÀŒ√»Õ »œ¿–ŒÀ‹
            TCHAR szLogin[256], szPassword[256], szEmail[256];
            GetDlgItemText(hwnd, IDC_LOGIN_EDIT, szLogin, 256);
            GetDlgItemText(hwnd, IDC_PASSWORD_EDIT, szPassword, 256);
            GetDlgItemText(hwnd, IDC_EMAIL_EDIT, szEmail, 256);

            userDatabase.push_back(std::make_pair(szLogin, szPassword));

            std::wofstream outputFile("users.txt", std::ios_base::app);
            if (outputFile.is_open()) 
            {
                outputFile << "Login: " << szLogin << ", Password: " << szPassword << ", Email: " << szEmail << std::endl;
                outputFile.close();
            }
            else 
            {
                MessageBox(hwnd, L"Failed to open file for writing", L"Error", MB_OK | MB_ICONERROR);
            }

            MessageBox(hwnd, L"Sign-Up Successful", L"Success", MB_OK | MB_ICONINFORMATION);
            EndDialog(hwnd, 0);
            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MENU_DIALOG), NULL, (DLGPROC)MenuDialogProc);
            return TRUE;
        }
        }
        break;

    case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
    }
    return FALSE;
}
