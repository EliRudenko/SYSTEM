#include "ParentProcessDlg.h"

ParentProcessDlg* ParentProcessDlg::ptr = NULL;

ParentProcessDlg::ParentProcessDlg(void)
{
	ptr = this;
}

ParentProcessDlg::~ParentProcessDlg(void)
{

}

void ParentProcessDlg::Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
}

void MessageAboutError(DWORD dwError)
{
	LPVOID lpMsgBuf = NULL;
	TCHAR szBuf[300];

	BOOL fOK = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	if (lpMsgBuf != NULL)
	{
		wsprintf(szBuf, TEXT("Ошибка %d: %s"), dwError, lpMsgBuf);
		MessageBox(0, szBuf, TEXT("Сообщение об ошибке"), MB_OK | MB_ICONSTOP);
		LocalFree(lpMsgBuf);
	}
}

void ParentProcessDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if (IDC_BUTTON1 == id)
	{
		CreateMutex(0, FALSE, TEXT("{B8A2C367-10FE-494d-A869-841B2AF972E0}"));
		//                    TEXT("{B8A2C367-10FE-494d-A869-841B2AF972E0}")
		STARTUPINFO st1 = { sizeof(st1) };
		STARTUPINFO st2 = { sizeof(st2) };
		PROCESS_INFORMATION pr1;
		PROCESS_INFORMATION pr2;
		TCHAR filename1[50];
		TCHAR filename2[20];
		wsprintf(filename1, TEXT("%s"), TEXT("WriteToFileThread.exe"));
		wsprintf(filename2, TEXT("%s"), TEXT("CodingThread.exe"));
		if (!CreateProcess(NULL, filename1, NULL, NULL, 0, 0, NULL, NULL, &st1, &pr1))
		{
			MessageAboutError(GetLastError());
			return;
		}
		if (!CreateProcess(NULL, filename2, NULL, NULL, 0, 0, NULL, NULL, &st2, &pr2))
		{
			MessageAboutError(GetLastError());
			return;
		}
		CloseHandle(pr1.hThread);
		CloseHandle(pr1.hProcess);
		CloseHandle(pr2.hThread);
		CloseHandle(pr2.hProcess);
	}
}

BOOL CALLBACK ParentProcessDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}
	return FALSE;
}
