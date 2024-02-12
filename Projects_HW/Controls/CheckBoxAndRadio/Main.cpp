#include <Windows.h>
#include "resource.h"
#include <string>
using namespace std;

#define IDC_CHECK3 (WM_USER + 1)

HINSTANCE hInstance;

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	::hInstance = hInstance;
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hCheckBoxes[3];
	static HWND hButtonOk, hButtonCancel;

	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		hCheckBoxes[0] = GetDlgItem(hwnd, IDC_CHECK1);
		hCheckBoxes[1] = GetDlgItem(hwnd, IDC_CHECK2);
		hButtonOk = GetDlgItem(hwnd, IDOK);
		hButtonCancel = GetDlgItem(hwnd, IDCANCEL);

		hCheckBoxes[2] = CreateWindowEx(
			0,
			L"Button",
			L"Check 3",
			WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, //| BS_PUSHLIKE
			35, 70,
			80, 15,
			hwnd,
			(HMENU)IDC_CHECK3,
			hInstance,
			0
		);

		SendMessage(hCheckBoxes[2], BM_SETCHECK, BST_CHECKED, 0);

		//RadioButton
		//SendDlgItemMessage(hwnd, IDC_RADIO1, BM_SETCHECK, BST_CHECKED, 0);
		//SendDlgItemMessage(hwnd, IDC_RADIO2, BM_SETCHECK, BST_CHECKED, 0);
		
		CheckRadioButton(hwnd, IDC_RADIO1, IDC_RADIO3, IDC_RADIO2);
		
		break;
	}

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK && HIWORD(wParam) == BN_CLICKED) {
			
			wstring str;

			for (size_t i = 0; i < 3; i++)
			{
				LRESULT isCheked = SendMessage(hCheckBoxes[i], BM_GETCHECK, 0, 0);

				str += L"Check box ";
				str += to_wstring(i + 1);

				if (isCheked == BST_CHECKED) {
					
					str += L" checked\n";
				}
				else if (isCheked == BST_UNCHECKED){
					
					str += L" unchecked\n";
				}
				else {
					str += L" indeterminate\n";
				}
			}		

			MessageBox(hwnd, str.c_str(), L"", MB_OK);
		}
		else if ((HWND)lParam == hCheckBoxes[2] && HIWORD(wParam) == BN_CLICKED) {
			
			//LRESULT isCheked = SendMessage(hCheckBoxes[2], BM_GETCHECK, 0, 0);
			UINT isCheked = IsDlgButtonChecked(hwnd, IDC_CHECK3);

			if (isCheked == BST_CHECKED) {

				EnableWindow(hButtonOk, TRUE);
			}
			else if (isCheked == BST_UNCHECKED) {

				EnableWindow(hButtonOk, FALSE);
			}
		}
		break;

	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return TRUE;
	}

	return FALSE;
}