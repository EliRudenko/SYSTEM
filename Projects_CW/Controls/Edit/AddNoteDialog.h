#pragma once

#include "Header.h"

class AddNoteDialog
{
public:
	void OnInitDialog(HWND hwnd);
	void OnCommand(HWND hwnd, DWORD nCode, DWORD idControl, HWND hControl);

private:
	void OnSaveButtonClick(HWND hwnd, DWORD nCode);
	void OnTitleChange(HWND hwnd, DWORD nCode);
	bool TitleValidate(LPCWSTR title);

	void SaveNote(wstring str);

	HINSTANCE hInstance;

	HWND hSaveButton;
	HWND hTitleEdit;
	HWND hTextEdit;
	HWND hErrorStatic;

	wstring path = L"notes.txt";

	static const UINT TITLE_MAX_SIZE;
	static const UINT TEXT_MAX_SIZE;
};

