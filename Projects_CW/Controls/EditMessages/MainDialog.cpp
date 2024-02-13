#include "MainDialog.h"

void MainDialog::OnInitDialog(HWND hwnd)
{
	_hEdit = GetDlgItem(hwnd, IDC_EDIT);
	_hStaticInfo = GetDlgItem(hwnd, IDC_STATIC_INFO);
}

void MainDialog::OnCommand(HWND hwnd, DWORD nCode, DWORD idControl, HWND hControl)
{
	switch (idControl) {
	case IDC_BUTTON_COPY:
		SendMessage(_hEdit, WM_COPY, 0, 0);
		break;

	case IDC_BUTTON_CUT:
		SendMessage(_hEdit, WM_CUT, 0, 0);
		break;

	case IDC_BUTTON_PASTE:
		SendMessage(_hEdit, WM_PASTE, 0, 0);
		break;

	case IDC_BUTTON_CLEAR:
		SendMessage(_hEdit, WM_CLEAR, 0, 0);
		break;

	case IDC_BUTTON_SELECT_ALL:
		SendMessage(_hEdit, EM_SETSEL, 0, -1);
		break;

	case IDC_BUTTON_UNDO:
		SendMessage(_hEdit, EM_UNDO, 0, 0);
		break;

	case IDC_BUTTON_INFO:
		OnGetInfoHandler(hwnd, nCode);
		break;

	case IDC_BUTTON_LEFT:
	{
		LONG style = GetWindowLong(_hEdit, GWL_STYLE);
		style ^= ES_RIGHT;
		SetWindowLong(_hEdit, GWL_STYLE, style | ES_LEFT);
		InvalidateRect(_hEdit, NULL, TRUE);
		break;
	}

	case IDC_BUTTON_RIGHT:
	{
		LONG style = GetWindowLong(_hEdit, GWL_STYLE);
		style ^= ES_LEFT;
		SetWindowLong(_hEdit, GWL_STYLE, style | ES_RIGHT);
		InvalidateRect(_hEdit, NULL, TRUE);
		break;
	}

	case IDC_EDIT:
		OnEditHanlder(hwnd, nCode);
		break;
	}
}

void MainDialog::OnEditHanlder(HWND hwnd, DWORD nCode)
{
	if (nCode == EN_CHANGE) {
		UINT textLength = SendMessage(_hEdit, WM_GETTEXTLENGTH, 0, 0);
		UINT rows = SendMessage(_hEdit, EM_GETLINECOUNT, 0, 0);

		wstring str = L"Length: ";
		str += to_wstring(textLength);
		str += L" Rows: ";
		str += to_wstring(rows);

		SetWindowText(hwnd, str.c_str());
	}
}

void MainDialog::OnGetInfoHandler(HWND hwnd, DWORD nCode)
{
	DWORD start;
	DWORD end;

	SendMessage(_hEdit, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);

	WCHAR szText[1024];
	GetWindowText(_hEdit, szText, 1024);

	wstring text = szText;
	wstring selectedText = text.substr(start, (end - start));

	wstring str = L"Selection start: ";
	str += to_wstring(start);
	str += L" end: ";
	str += to_wstring(end);
	str += L" text: " + selectedText;

	SetWindowText(_hStaticInfo, str.c_str());
}
