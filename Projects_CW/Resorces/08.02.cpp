#include "Windows.h"
#include "windowsx.h"
#include "tchar.h"
#include "resource.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	WCHAR className[] = L"FirstWindow";

	WNDCLASSEX wndClass;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.hInstance = hInstance;

	wndClass.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_NODROP)); 
	wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wndClass.hIconSm = NULL;
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = className;

	wndClass.lpfnWndProc = WindowProc;

	if (!RegisterClassEx(&wndClass)) {
		return 0;
	}

	HWND hWnd = CreateWindowEx(
		0,
		className,
		L"Window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, //x,y
		CW_USEDEFAULT, CW_USEDEFAULT, //width, height
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(hWnd, nShowCmd);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_MOUSEMOVE:
	{
		int x = GET_X_LPARAM(lParam);

		RECT rect;
		GetClientRect(hwnd, &rect);

		//GetModuleHandle ���������� ���������� ���������� ����������, 
		//�� ����� ��� ������ Load �������
		HINSTANCE hInst = GetModuleHandle(0);

		int idCursor = x < rect.right / 2 ? IDC_NODROP : IDI_ICON2;

		//��������� ������ �� �������� � �������� ��� ����������
		HCURSOR hCur = LoadCursor(hInst, MAKEINTRESOURCE(idCursor));

		if (hCur != NULL) {
			//SetCursor(hCur); ����������� ������ ������

			//SetClassLong ������ ���� � ������ ����
			//� ������ ������ ������ ������
			SetClassLong(hwnd, GCLP_HCURSOR, (LONG)hCur);
		}
		else {
			//GetLastError ���������� ��� ������, 
			//������� ���������� ��������� ��������� ������� 

			//@err,c ��������� ����������� ��� ������ � ���������
			//@err,hr ��������� ����������� �������� ������ � ���������

			int errCode = GetLastError();
			TCHAR str[20];
			wsprintf(str, TEXT("Error code: %d"), errCode);
			SetWindowText(hwnd, str);
		}
		break;
	}

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}
