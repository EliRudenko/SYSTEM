#include "CodingThreadDlg.h"

CodingThreadDlg* CodingThreadDlg::ptr = NULL;
HANDLE hMutex;
std::wstring codingText;

CodingThreadDlg::CodingThreadDlg(void)
{
    ptr = this;
}

CodingThreadDlg::~CodingThreadDlg(void)
{

}

void CodingThreadDlg::Cls_OnClose(HWND hwnd)
{
    ReleaseMutex(hMutex);
    EndDialog(hwnd, 0);
}

DWORD WINAPI Coding_Thread(LPVOID lp)
{
    CodingThreadDlg* ptr = reinterpret_cast<CodingThreadDlg*>(lp);
    char buf[100];

    std::ifstream in("coding.txt", std::ios::in | std::ios::binary);

    if (!in)
    {
        MessageBox(ptr->hDialog, L"Ошибка открытия файла!", L"Мьютекс", MB_OK | MB_ICONINFORMATION);
        return 1;
    }

    hMutex = OpenMutex(MUTEX_ALL_ACCESS, false, L"{B8A2C367-10FE-494d-A869-841B2AF972E0}");
    DWORD dwAnswer = WaitForSingleObject(hMutex, INFINITE);

    std::wstringstream ss;

    while (in.getline(buf, 100))
    {
        ss << buf << L"\r\n"; 
    }

    in.close();
    ReleaseMutex(hMutex);

    codingText = ss.str();

    ptr->UpdateEditControl(codingText);

    MessageBox(ptr->hDialog, L"Чтение данных из файла coding.txt завершено!", L"Мьютекс", MB_OK | MB_ICONINFORMATION);

    return 0;
}

void CodingThreadDlg::UpdateEditControl(const std::wstring& text)
{
    HWND hEdit = GetDlgItem(hDialog, IDC_EDIT1);

    SendMessageW(hEdit, WM_SETTEXT, 0, (LPARAM)L"");

    SendMessageW(hEdit, EM_SETSEL, 0, -1); // Выделяем всё
    SendMessageW(hEdit, EM_REPLACESEL, 0, (LPARAM)text.c_str());
}

BOOL CodingThreadDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    hDialog = hwnd;

    HWND hEdit = GetDlgItem(hwnd, IDC_EDIT1);
    DWORD dwStyle = GetWindowLong(hEdit, GWL_STYLE);
    SetWindowLong(hEdit, GWL_STYLE, dwStyle | ES_MULTILINE);

    CreateThread(NULL, 0, Coding_Thread, this, 0, NULL);
    return TRUE;
}

BOOL CALLBACK CodingThreadDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CLOSE:
        ptr->Cls_OnClose(hwnd);
        break;
    case WM_INITDIALOG:
        ptr->Cls_OnInitDialog(hwnd, (HWND)wParam, lParam);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON1:
            ptr->Cls_OnButton1();
            break;
        }
        break;
    }
    return FALSE;
}

void CodingThreadDlg::Cls_OnButton1()
{
    ReleaseMutex(hMutex);
    EndDialog(hDialog, 0);
}

