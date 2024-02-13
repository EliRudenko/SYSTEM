#include "Header.h"

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{	
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, DlgProc);
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static AddNoteDialog dialog;
	switch (uMsg)
	{
	case WM_INITDIALOG:		
		dialog.OnInitDialog(hwnd);
		break;	

	case WM_COMMAND:
		dialog.OnCommand(hwnd, HIWORD(wParam), LOWORD(wParam), (HWND)lParam);
		break;

	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return TRUE;
	}

	return FALSE;
}