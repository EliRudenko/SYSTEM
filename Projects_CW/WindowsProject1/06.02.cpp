#include <windows.h>
#include <tchar.h>
#include <string>

//1

INT WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR lpszCmdLine, int nCmdShow)
{
    const char* resume = "�������. �� ��� �� ����������";//����� ��� "������"
    const int numWindows = 3; //��� �� ����

    //�����, ����� ���� ��� �������� �������� ������� ���� ��� �������� ��� ��
    int resumeLength = lstrlenA(resume);
    int charsPerPage = resumeLength / numWindows;

    //��� ������ ��� � ���� �� ������
    for (int i = 0; i < numWindows - 1; ++i)
    {
        char windowText[256];//�����)

        //i * charsPerPage ��� ��������� ������ ����� "������"
        //resume + i * charsPerPage ��������� �� ������ ������ ����� � ������
        strncpy_s(windowText, sizeof(windowText), resume + i * charsPerPage, charsPerPage); //����������� �����
        windowText[charsPerPage] = '\0'; //����� ������

        MessageBoxA(NULL, windowText, "������", MB_OK | MB_ICONERROR);//�����
    }

    char lastWindowText[256];//����� 

    int averageCharsPerPage = resumeLength / numWindows; //������� ��� �� ��������
  
    //��������������!!
    //������ ������������ wsprintfA, ������ ��� sprintf �� ���������(
    wsprintfA(lastWindowText, "������� ����� �������� �� ��������: %d", averageCharsPerPage);

    MessageBoxA(NULL, lastWindowText, "������", MB_OK | MB_ICONINFORMATION);//�����

    return 0;
}


//2
/*
const int maxNumber = 100;//�������� 100 �����
int secretNumbers[maxNumber];//����� ���������� ����� � ������������ ��� �������
int attempts;//�������

INT WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR lpszCmdLine, int nCmdShow)
{
    srand(GetTickCount());//���������

    do {
        //���������������
        for (int i = 0; i < maxNumber; ++i) { secretNumbers[i] = i + 1; } // ����� �� 1 �� 100 � ������

        //� ������ ������� ������ 1-100
        for (int i = maxNumber - 1; i > 0; --i)
        {
            int j = rand() % (i + 1);
            std::swap(secretNumbers[i], secretNumbers[j]);
        }


        attempts = 0;//���� ������� ���
        do
        {
            //��������
            TCHAR question[100];
            _stprintf_s(question, _T("����� %d?"), secretNumbers[attempts]);

            // �� ��� ��� �� ������������
            int guess = MessageBox(0, question, _T("���� '������ �����'"), MB_ICONQUESTION | MB_YESNO);

            if (guess == IDNO) // �� ������, ������ ������  + ������� 
            {//IDNO ���������, ��� ������ ���

                int closeResult = MessageBox(0, _T("�� ������ ��� ������ ��� ��������� ��������� ?"), _T("����������� ������"), MB_ICONQUESTION | MB_YESNO);
                if (closeResult == IDNO)
                {
                    return 0;  // ������
                }
            }
            else if (guess == IDYES) //������ = �������
            {//IDYES ���������, ��� ������ ��

                attempts++; //�� + ������� ��� �����
                break; // ����� ����������, ������ ���� �� ��������� ����
            }
            else //����� ����
            {
                MessageBox(0, _T("�������, �������))"), _T("������ �����!!!"), MB_ICONINFORMATION | MB_OK);
                return 0;
            }

        } while (true);

        //������ = ����� � �������� �����
        MessageBox(0, _T("�������, ������� ��. ��� ����� ���������� � ��������("), _T("������ �����!!!"), MB_ICONINFORMATION | MB_OK);

        //������� �������
        MessageBox(0, std::to_wstring(attempts).c_str(), _T("���������� ���������� �������"), MB_ICONINFORMATION | MB_OK);

        //���������� ���� ����
        int result = MessageBox(0, _T("������ ��� ������������ ��������� �����? ��������?"), _T("������ �����!!!"), MB_ICONQUESTION | MB_YESNO);

        return 0;
    } while (true);

    return 0;
}
*/