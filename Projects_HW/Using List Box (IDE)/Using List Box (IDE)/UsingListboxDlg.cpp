#include "UsingListboxDlg.h"

UsingListboxDlg* UsingListboxDlg::ptr = NULL;
int selectedClubIndex = -1;

UsingListboxDlg::UsingListboxDlg(void)
{
	ptr = this;
}

void UsingListboxDlg::Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
}

BOOL UsingListboxDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) 
{
	// получим дескрипторы элементов управления
	hList1 = GetDlgItem(hwnd, IDC_LIST1);
	hList2 = GetDlgItem(hwnd, IDC_LIST2);
	hEdit1 = GetDlgItem(hwnd, IDC_EDIT1);
	hEdit2 = GetDlgItem(hwnd, IDC_EDIT2);

	// добавим 8 элементов в список с единичным выбором
	SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(TEXT("Милан Италия")));
	SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(TEXT("Ливерпуль Англия")));
	SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(TEXT("Бавария Германия")));
	SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(TEXT("Барселона Испания")));
	SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(TEXT("Арсенал Англия")));
	SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(TEXT("Лион Франция")));
	SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(TEXT("Интер Италия")));
	SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(TEXT("Реал Испания")));

	// установим ширину колонки для списка с множественным выбором
	SendMessage(hList2, LB_SETCOLUMNWIDTH, 170, 0);

	// добавим 8 элементов в список с множественным выбором
	SendMessage(hList2, LB_ADDSTRING, 0, LPARAM(TEXT("Рома Италия")));
	SendMessage(hList2, LB_ADDSTRING, 0, LPARAM(TEXT("Манчестер Юнайтед Англия")));
	SendMessage(hList2, LB_ADDSTRING, 0, LPARAM(TEXT("Вердер Германия")));
	SendMessage(hList2, LB_ADDSTRING, 0, LPARAM(TEXT("Валенсия Испания")));
	SendMessage(hList2, LB_ADDSTRING, 0, LPARAM(TEXT("Челси Англия")));
	SendMessage(hList2, LB_ADDSTRING, 0, LPARAM(TEXT("Марсель Франция")));
	SendMessage(hList2, LB_ADDSTRING, 0, LPARAM(TEXT("Ювентус Италия")));
	SendMessage(hList2, LB_ADDSTRING, 0, LPARAM(TEXT("Атлетико Испания")));
	return TRUE;
}

void UsingListboxDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
	{
	case IDC_ADD: 
		{
			// определим длину текста (названия клуба), введённого в текстовое поле
			int length = SendMessage(hEdit1, WM_GETTEXTLENGTH, 0, 0);
			// выделим память необходимого размера
			TCHAR *pBuffer = new TCHAR[length + 1];
			// в выделенную память скопируем текст, введённый в тестовое поле
			GetWindowText(hEdit1, pBuffer, length + 1);
			if(lstrlen(pBuffer))
			{
				// уточним, имеется ли уже в списке введённое название клуба 
				int index = SendMessage(hList1, LB_FINDSTRINGEXACT, -1, LPARAM(pBuffer));
				if(index == LB_ERR)
					// добавим название клуба в список
					SendMessage(hList1, LB_ADDSTRING, 0, LPARAM(pBuffer));
				else
					MessageBox(hwnd, TEXT("Такой клуб уже существует!"), TEXT("Добавление клуба"), MB_OK | MB_ICONSTOP);
			}
			delete[] pBuffer;
		}
		break;
	case IDC_DEL:
		{
			// получим индекс выбранного элемента списка
			int index = SendMessage(hList1, LB_GETCURSEL, 0, 0);
			if(index != LB_ERR) // выбран ли элемент списка?
			{
				// определим длину названия выбранного клуба
				int length = SendMessage(hList1, LB_GETTEXTLEN, index, 0);
				// выделим память необходимого размера
				TCHAR *pBuffer = new TCHAR[length + 1];
				// скопируем название клуба в выделенную память
				SendMessage(hList1, LB_GETTEXT, index, LPARAM(pBuffer));
				MessageBox(hwnd, pBuffer, TEXT("Удаление клуба"), MB_OK | MB_ICONINFORMATION);
				// удалим строку из списка
				SendMessage(hList1, LB_DELETESTRING, index, 0);
				delete[] pBuffer;
			}
			else
				MessageBox(hwnd, TEXT("Клуб не выбран!"), TEXT("Удаление клуба"), MB_OK | MB_ICONSTOP);
		}
		break;
	case IDC_FIND:
		{
			// определим длину искомого названия, введённого в текстовое поле
			int length = SendMessage(hEdit2, WM_GETTEXTLENGTH, 0, 0);
			// выделим память необходимого размера
			TCHAR *pBuffer = new TCHAR[length + 1];
			// скопируем название клуба в выделенную память
			GetWindowText(hEdit2, pBuffer, length + 1);
			if(lstrlen(pBuffer))
			{
				// определим, имеется ли в списке искомое название клуба
				int index = SendMessage(hList1, LB_SELECTSTRING, -1, LPARAM(pBuffer));
				if(index == LB_ERR)
					MessageBox(hwnd, TEXT("Клуб не найден!"), TEXT("Поиск клуба"), MB_OK | MB_ICONSTOP);
			}
			delete[] pBuffer;
		}
		break;
	case IDC_DELALL:
		// очистим содержимое списка
		SendMessage(hList1, LB_RESETCONTENT, 0, 0);
		break;
	case IDC_GETSELITEMS:
		{
			// определим количество выбранных элементов в списке с множественным выбором
			int nCount = SendMessage(hList2, LB_GETSELCOUNT, 0, 0);
			// выделим память необходимого размера для хранения индексов выбранных элементов списка
			int *p = new int[nCount];
			// Заполним динамический массив индексами выделенных элементов списка 
			SendMessage(hList2, LB_GETSELITEMS, nCount, LPARAM(p));
			for(int i = 0; i < nCount; i++)
			{
				// определим размер текста элемента списка
				int length = SendMessage(hList2, LB_GETTEXTLEN, p[i], 0);
				// выделим память необходимого размера
				TCHAR *pBuffer = new TCHAR[length + 1];
				// в выделенную память скопируем текст выбранного элемента списка
				SendMessage(hList2, LB_GETTEXT, p[i], LPARAM(pBuffer));
				MessageBox(hwnd, pBuffer, TEXT("Список с множественным выбором"), MB_OK | MB_ICONINFORMATION);
				delete[] pBuffer;
			}
		}
	case IDC_LIST1:
		//ДЗ!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if (codeNotify == LBN_SELCHANGE) 
		{
			//ИНДЕХ ВЫБРАННОГО
			int index = SendMessage(hList1, LB_GETCURSEL, 0, 0);
			if (index != LB_ERR) 
			{
				//ДЛИНА ДЛЯ ПАМЯТИ
				int length = SendMessage(hList1, LB_GETTEXTLEN, index, 0);
				TCHAR* pBuffer = new TCHAR[length + 1]; //ПАМЯТЬ
			
				SendMessage(hList1, LB_GETTEXT, index, LPARAM(pBuffer)); // КОПИ ТЕКСТ
			
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT4), pBuffer); // ДЛЯ РЕДАКТА
		
				selectedClubIndex = index; // СОХРАНЯЕМ
				delete[] pBuffer;
			}
		}
		break;
	case IDC_BUTTON3:
	{//ДЗ!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//КНОМКА СОХРАНИТЬ, В РЕДАКТЕ
		 
		int length = SendMessage(GetDlgItem(hwnd, IDC_EDIT4), WM_GETTEXTLENGTH, 0, 0); //ДЛИНА ПОСЛЕ ИЗМЕНЕНИЯ
		TCHAR* pBuffer = new TCHAR[length + 1];//ПАМЯТЬ С НОВОЙ ДЛИНОЙ
		
		GetWindowText(GetDlgItem(hwnd, IDC_EDIT4), pBuffer, length + 1); // КОПИ

		if (lstrlen(pBuffer)) 
		{//НОВЫЕ ДАННЫЕ В СПИСКЕ
			
			SendMessage(hList1, LB_DELETESTRING, selectedClubIndex, 0);
			SendMessage(hList1, LB_INSERTSTRING, selectedClubIndex, LPARAM(pBuffer));
			
			SendMessage(hList1, LB_SETCURSEL, -1, 0); //СБРОСС
			
			SetWindowText(GetDlgItem(hwnd, IDC_EDIT4), L"");//ОЧИСТКА
		}
		delete[] pBuffer;
	}
	break;


	}
	//БЫЛО
	// в списке с единичным выбором текущий выбор был изменён
	if(id == IDC_LIST1 && codeNotify == LBN_SELCHANGE)
	{
		// получим индекс выбранного элемента списка
		int index = SendMessage(hList1, LB_GETCURSEL, 0, 0);
		if(index != LB_ERR) // выбран ли элемент списка?
		{
			// определим длину названия клуба
			int length = SendMessage(hList1, LB_GETTEXTLEN, index, 0);
			// выделим память необходимого размера
			TCHAR *pBuffer = new TCHAR[length + 1];
			// в выделенную память скопируем текст выбранного элемента списка
			SendMessage(hList1, LB_GETTEXT, index, LPARAM(pBuffer));
			// установим текст в заголовок главного окна
			SetWindowText(hwnd, pBuffer);
			delete[] pBuffer;
		}
	}
}

BOOL CALLBACK UsingListboxDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}
	return FALSE;
}