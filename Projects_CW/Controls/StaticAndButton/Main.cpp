#include <Windows.h>
#include "resource.h"

HINSTANCE hInstance;

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	::hInstance = hInstance;
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, DlgProc);
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HWND hPictureStatic = GetDlgItem(hwnd, IDC_PICTURE);
		HBITMAP hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
		SendMessage(hPictureStatic, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
		
		HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
		SendDlgItemMessage(hwnd, IDC_BUTTON2, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
		
		break;
	}

	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_BUTTON1 && HIWORD(wParam) == BN_CLICKED) {
			EnableWindow(GetDlgItem(hwnd, IDC_BUTTON2), FALSE);
			MessageBox(NULL, L"Clicked", L"", MB_OK);
		}
		break;

	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return TRUE;
	}

	return FALSE;
}