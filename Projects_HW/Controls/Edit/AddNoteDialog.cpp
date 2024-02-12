#include "AddNoteDialog.h"

const UINT AddNoteDialog::TITLE_MAX_SIZE = 100;
const UINT AddNoteDialog::TEXT_MAX_SIZE = 1024;

void AddNoteDialog::OnInitDialog(HWND hwnd)
{
	hInstance = GetModuleHandle(0);

	//save button
	hSaveButton = GetDlgItem(hwnd, IDSAVE);

	//error static
	hErrorStatic = GetDlgItem(hwnd, IDC_ERROR);

	//title
	hTitleEdit = GetDlgItem(hwnd, IDC_TITLE);
	SendMessage(hTitleEdit, EM_SETLIMITTEXT, TITLE_MAX_SIZE, 0);
	SetFocus(hTitleEdit);

	//text
	hTextEdit = GetDlgItem(hwnd, IDC_TEXT);
	SendMessage(hTextEdit, EM_SETLIMITTEXT, TEXT_MAX_SIZE, 0);
}

void AddNoteDialog::OnCommand(HWND hwnd, DWORD nCode, DWORD idControl, HWND hControl)
{
	switch (idControl) {
	case IDSAVE:
		OnSaveButtonClick(hwnd, nCode);
		break;

	case IDCANCEL:
		EndDialog(hwnd, 0);
		break;

	case IDC_TITLE:
		OnTitleChange(hwnd, nCode);
		break;
	}
}

void AddNoteDialog::OnSaveButtonClick(HWND hwnd, DWORD nCode)
{
	if (nCode == BN_CLICKED)
	{
		WCHAR title[TITLE_MAX_SIZE];			
		GetWindowText(hTitleEdit, title, TITLE_MAX_SIZE);

		WCHAR text[TEXT_MAX_SIZE];
		GetWindowText(hTextEdit, text, TEXT_MAX_SIZE);

		wstring str = L"Title: ";
		str += title;
		str += L"\nText: ";
		str += text;

		SaveNote(str);
		MessageBox(hwnd, str.c_str(), L"Summary", MB_OK);

	}
}

void AddNoteDialog::OnTitleChange(HWND hwnd, DWORD nCode)
{
	if (nCode == EN_CHANGE) 
	{
		WCHAR title[100];
		GetWindowText(hTitleEdit, title, 100);

		bool isValid = TitleValidate(title);

		ShowWindow(hErrorStatic, isValid ? SW_HIDE : SW_SHOW);
		EnableWindow(hSaveButton, isValid);
	}
}

bool AddNoteDialog::TitleValidate(LPCWSTR title)
{
	wstring str = title;

	int count = count_if(str.begin(), str.end(), [](WCHAR symbol) {
		return symbol == ' ' || symbol == '\t' || symbol == '\n';
	});

	return count != str.size();;
}

void AddNoteDialog::SaveNote(wstring str)
{
	wofstream fout(path, ios::app);

	if (!fout.is_open()) {
		MessageBox(NULL, L"File is not opened", L"Error", MB_OK);
		return;
	}

	fout << str << endl;
	fout.close();
}
