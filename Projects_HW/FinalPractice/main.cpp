#include <windows.h>
#include <string>
#include <set>
#include <fstream>
#include <vector>
#include <algorithm>
#include "resource.h"


std::set<std::wstring> words;
std::vector<std::wstring> foundWords;
int curseWords = 0;
HANDLE mtx;
bool censorshipDone = false;
bool isSearching = false;
bool isPaused = false;
HANDLE searchThread;

void searchFiles() {
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW(L"./*.txt", &findFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            std::wstring filePath = findFileData.cFileName;
            std::wifstream file(filePath);
            std::wstring word;
            while (file >> word) {
                WaitForSingleObject(mtx, INFINITE);
                if (isPaused) {
                    ReleaseMutex(mtx);
                    return; // Stop the search if paused
                }
                if (!isSearching) {
                    ReleaseMutex(mtx);
                    return; // Stop the search if isSearching is false
                }
                if (words.find(word) != words.end()) {
                    foundWords.push_back(word);
                    curseWords++;
                    /*MessageBox(NULL, L"Found Curse Word", L"Info", MB_OK);*/
                    Sleep(3000); // delay
                }
                ReleaseMutex(mtx);
            }
        } while (FindNextFileW(hFind, &findFileData) != 0);
        FindClose(hFind);
    }
}

void writeStatistics() {
    if (!censorshipDone)
        return;

    std::wofstream file("Statistics.txt");
    for (const auto& word : foundWords) {
        file << word << std::endl;
    }
    file << "Total curse words: " << curseWords << std::endl;
    MessageBox(NULL, L"Statistics gathered", L"Info", MB_OK);
}

void censorFiles() {
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW(L"./*.txt", &findFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            std::wstring filePath = findFileData.cFileName;
            std::wifstream file(filePath);
            std::wstring content((std::istreambuf_iterator<wchar_t>(file)), std::istreambuf_iterator<wchar_t>());
            file.close();
            for (const auto& word : words) {
                size_t pos = content.find(word);
                while (pos != std::wstring::npos) {
                    content.replace(pos, word.length(), L"*******");
                    pos = content.find(word, pos + 7);
                }
            }
            std::wofstream outFile(filePath + L"_censored.txt");
            outFile << content;
            outFile.close();
        } while (FindNextFileW(hFind, &findFileData) != 0);
        FindClose(hFind);
    }
    censorshipDone = true;
   /* MessageBox(NULL, L"Censored all the files", L"Info", MB_OK);*/
    writeStatistics(); // Write statistics after all files are censored
    MessageBox(NULL, L"Program finished", L"Info", MB_OK);
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        mtx = CreateMutex(NULL, FALSE, NULL);
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_MAN_IMP:
        {
            TCHAR buffer[256];
            GetDlgItemText(hwnd, IDC_EDIT_MAN_IMP, buffer, 256);
            std::wstring word(buffer);
            WaitForSingleObject(mtx, INFINITE);
            words.insert(word);
            ReleaseMutex(mtx);
            SendDlgItemMessage(hwnd, IDC_LIST_CURSE, LB_ADDSTRING, 0, (LPARAM)buffer);
           /* MessageBox(hwnd, L"Added successfully", L"Success", MB_OK);*/
            return TRUE;
        }
        case IDC_BUTTON_DEL_CURSE:
        {
            int idx = SendDlgItemMessage(hwnd, IDC_LIST_CURSE, LB_GETCURSEL, 0, 0);
            if (idx != LB_ERR)
            {
                TCHAR buffer[256];
                SendDlgItemMessage(hwnd, IDC_LIST_CURSE, LB_GETTEXT, idx, (LPARAM)buffer);
                std::wstring word(buffer);
                WaitForSingleObject(mtx, INFINITE);
                words.erase(word);
                ReleaseMutex(mtx);
                SendDlgItemMessage(hwnd, IDC_LIST_CURSE, LB_DELETESTRING, idx, 0);
            }
            return TRUE;
        }
        case IDC_BUTTON_FILE_IMP:
            DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG_FILE_PATH), hwnd, DlgProc);
            return TRUE;
        case IDC_BUTTON_PATH_ENTER:
        {
            TCHAR buffer[256];
            GetDlgItemText(hwnd, IDC_EDIT_PATH, buffer, 256);
            std::wifstream file(buffer);
            if (file)
            {
                std::wstring word;
                while (file >> word)
                {
                    words.insert(word);
                    SendDlgItemMessage(hwnd, IDC_LIST_CURSE, LB_ADDSTRING, 0, (LPARAM)word.c_str());
                }
               /* MessageBox(hwnd, L"Import successful", L"Success", MB_ICONINFORMATION | MB_OK);*/
            }
            else
            {
                /*MessageBox(hwnd, L"Import failed", L"Error", MB_ICONERROR | MB_OK);*/
            }
            EndDialog(hwnd, 0);
            return TRUE;
        }
        case IDC_BUTTON_START:
            if (!isSearching) {
                isSearching = true;
                isPaused = false;
                SetDlgItemText(hwnd, IDC_BUTTON_START, L"Pause");

                MessageBox(NULL, L"Work has resumed", L"Info", MB_OK);

                HANDLE t1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)searchFiles, NULL, 0, NULL);
                HANDLE t2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)writeStatistics, NULL, 0, NULL);
                WaitForSingleObject(t1, INFINITE);
                WaitForSingleObject(t2, INFINITE);
                CloseHandle(t1);
                CloseHandle(t2);
                HANDLE t3 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)censorFiles, NULL, 0, NULL);
                WaitForSingleObject(t3, INFINITE);
                CloseHandle(t3);
            }
            else {
                isSearching = false;
                isPaused = true;
                SetDlgItemText(hwnd, IDC_BUTTON_START, L"Start");

                MessageBox(NULL, L"Work has been stopped", L"Info", MB_OK);
            }
            return TRUE;
        }
        break;
    case WM_CLOSE:
        CloseHandle(mtx);
        EndDialog(hwnd, 0);
        return TRUE;
    }
    return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_CURSE_MAIN), NULL, DlgProc);
    return 0;
}
