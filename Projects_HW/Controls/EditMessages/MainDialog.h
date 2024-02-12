#pragma once
#include "Header.h"

class MainDialog
{
public:
	void OnInitDialog(HWND hwnd);
	void OnCommand(HWND hwnd, DWORD nCode, DWORD idControl, HWND hControl);

private:

	void OnEditHanlder(HWND hwnd, DWORD nCode);
	void OnGetInfoHandler(HWND hwnd, DWORD nCode);

	HWND _hEdit;
	HWND _hStaticInfo;
};

