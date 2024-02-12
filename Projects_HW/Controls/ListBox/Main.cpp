#include <Windows.h>
#include "resource.h"
#include "PlayerRegistor.h"

#include <string>
#include <algorithm>
using namespace std;

HINSTANCE hInstance;

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	::hInstance = hInstance;
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, DlgProc);
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hListBox;
	static HWND hEditInfo;
	static PlayerRegister reg;

	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		hListBox = GetDlgItem(hwnd, IDC_LIST1);
		hEditInfo = GetDlgItem(hwnd, IDC_EDIT_INFO);

		for (int i = 0; i < reg.players.size(); i++)
		{
			int pos = SendMessage(hListBox, LB_ADDSTRING, 0, LPARAM(reg.players[i].achName));
			SendMessage(hListBox, LB_SETITEMDATA, pos, i);
		}

		break;
	}

	case WM_COMMAND:

		if (LOWORD(wParam) == IDC_LIST1 && HIWORD(wParam) == LBN_SELCHANGE) {

			//int pos = SendMessage(hListBox, LB_GETCURSEL, 0, 0);		

			int selCount = SendMessage(hListBox, LB_GETSELCOUNT, 0, 0);

			int* selItems = new int[selCount];
			SendMessage(hListBox, LB_GETSELITEMS, selCount, LPARAM(selItems));

			wstring text = L"";
			
			for (size_t i = 0; i < selCount; i++)
			{
				int pos = selItems[i];

				if (pos != LB_ERR)
				{
					int i = SendMessage(hListBox, LB_GETITEMDATA, pos, 0);

					TCHAR str[MAX_PATH];

					wsprintf(
						str,
						L"Name: %s\r\nPosition: %s\r\nGamesPlayed: %d\r\nGoalsScored: %d\r\n\r\n",
						reg.players[i].achName,
						reg.players[i].achPosition,
						reg.players[i].nGamesPlayed,
						reg.players[i].nGoalsScored
					);

					text += str;					
				}
			}
			
			delete[] selItems;

			SetWindowText(hEditInfo, text.c_str());
			
		}

		else if (LOWORD(wParam) == IDC_ADD && HIWORD(wParam) == BN_CLICKED) {

			WCHAR name[MAX_PATH];
			WCHAR position[12];
			WCHAR games[10];
			WCHAR goals[10];

			GetDlgItemText(hwnd, IDC_EDIT_NAME, name, MAX_PATH);
			GetDlgItemText(hwnd, IDC_EDIT_POSITION, position, 12);
			GetDlgItemText(hwnd, IDC_EDIT_GAMES, games, 10);
			GetDlgItemText(hwnd, IDC_EDIT_GOALS, goals, 10);

			//прверку на корректность строк

			int findPos = SendMessage(hListBox, LB_FINDSTRINGEXACT, -1, LPARAM(name));

			if (findPos != LB_ERR) {
				MessageBox(hwnd, L"Exists", L"", MB_OK);
				break;
			}

			Player player;

			wcscpy_s(player.achName, MAX_PATH, name);
			wcscpy_s(player.achPosition, 12, position);
			player.nGamesPlayed = stoi(games);
			player.nGoalsScored = stoi(goals);

			reg.players.push_back(player);

			int i = reg.players.size() - 1;

			int pos = SendMessage(hListBox, LB_ADDSTRING, 0, LPARAM(name));
			SendMessage(hListBox, LB_SETITEMDATA, pos, i);
		}

		else if (LOWORD(wParam) == IDC_CLEAR && HIWORD(wParam) == BN_CLICKED) {

			SetDlgItemText(hwnd, IDC_EDIT_NAME, NULL);
			SetDlgItemText(hwnd, IDC_EDIT_POSITION, NULL);
			SetDlgItemText(hwnd, IDC_EDIT_GAMES, NULL);
			SetDlgItemText(hwnd, IDC_EDIT_GOALS, NULL);
		}

		else if (LOWORD(wParam) == IDC_DELETE && HIWORD(wParam) == BN_CLICKED) {

			int isDelete = MessageBox(hwnd, L"Delete?", L"", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);

			if (isDelete == IDYES)
			{				
				int pos = SendMessage(hListBox, LB_GETCURSEL, 0, 0);

				if (pos != LB_ERR)
				{
					int ind = SendMessage(hListBox, LB_GETITEMDATA, pos, 0);

					SendMessage(hListBox, LB_DELETESTRING, pos, 0);

					auto it = reg.players.begin() + ind;
					reg.players.erase(it);						
				}				
			}
		}
	
		break;

	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return TRUE;
	}

	return FALSE;
}