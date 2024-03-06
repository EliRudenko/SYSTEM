#include <windows.h>  
#include <string>   
#include <set>     
#include <fstream>    
#include <vector>     
#include <algorithm>  
#include <Commctrl.h> // ��� ������ � ���������� ����������
#include "resource.h"


std::wstring OpenFileDialog(HWND hwnd) 
{//��� ������� ���������� � ������ �����
    OPENFILENAME ofn; // ��������� ��� ���������� ����������� ���� ������ �����
    WCHAR szFile[MAX_PATH]; // ����� ��� �������� ���� � ���������� �����

    ZeroMemory(&ofn, sizeof(ofn)); // �������� ���������

    ofn.lStructSize = sizeof(ofn); 
    ofn.hwndOwner = hwnd; // ������������� ���������� ������������� ����
    ofn.lpstrFile = szFile;         
    ofn.lpstrFile[0] = '\0';        
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]); 
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0"; //������ ��� ����� ������
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; // ���� ������������� ����

    //�������� ���� � ������� �����, ������� ����
    if (GetOpenFileName(&ofn) == TRUE) { return ofn.lpstrFile; }
    else { return L"";}
}

class CurseDetector 
{
public:
    CurseDetector() : curseWords(0), censorshipDone(false) 
    {
        mtx = CreateMutex(NULL, FALSE, NULL); // ������� ������� ��� ������������� ������� � ��������
        hProgressBar = nullptr; //���������� �������� ���� = 0
        hListbox = nullptr; //���������� ������ = 0
    }

    ~CurseDetector() { CloseHandle(mtx); }

    void Initialize(HWND progressBar, HWND listbox) 
    {//������������� ������������ � �������� ����
        hProgressBar = progressBar; 
        hListbox = listbox;       
    }

    void AddCurseWord(const std::wstring& word) 
    {//���������� �� ���������� ����� � �������, ��������� � ������
        WaitForSingleObject(mtx, INFINITE); //������ ������� ��� ����������� �������
        words.insert(word); //���������� ����� 
        ReleaseMutex(mtx);//������������
    }

    void RemoveCurseWord(int index) 
    {//�������� �� ������� �� ������
        WaitForSingleObject(mtx, INFINITE);// ������ �������
        auto it = std::next(words.begin(), index); //�������� �� ��������
        words.erase(it);//��������
        ReleaseMutex(mtx);//�������
    }

    void ImportCurseWordsFromFile(const std::wstring& filePath) 
    {//������ �� �����
        std::wifstream file(filePath);// ������� ������
        if (file) 
        {//��������
            std::wstring word; // ���������� ��� �������� �����
            while (file >> word) 
            {//����������
                AddCurseWord(word);// ���������� ����� � ���������
                SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)word.c_str()); //����� � ������
            }
        }
    }


    void SearchFiles() 
    {//�����!!!!!!!!!!!!!!!!!!!!!!!1
        WIN32_FIND_DATAW findFileData; //��������� ���� � �����
        HANDLE hFind = FindFirstFileW(L"./*.txt", &findFileData); //������� �������, �����
       
        if (hFind != INVALID_HANDLE_VALUE) 
        {// ����� - �� =>
            do 
            {//��� �������� �����, ���� ����
                std::wstring filePath = findFileData.cFileName; //���� � �����
                std::wifstream file(filePath); //������
                std::wstring word; // ���������� ��� �������� �����
                while (file >> word) 
                { //����� ���� � �����
                    WaitForSingleObject(mtx, INFINITE); // ����������� �������!!!!!!!!!!1
                    
                    if (words.find(word) != words.end()) 
                    {//����� ������� � ���������
                        foundWords.push_back(word);//������ ����
                        curseWords++; //�������
                    }
                    ReleaseMutex(mtx); //�������
                }
            } while (FindNextFileW(hFind, &findFileData) != 0); //������� � ���������� �����
            
            FindClose(hFind); //���������� ������ ����
        }
    }

    void WriteStatistics() 
    {//������ ����������
        //���� �� ��������� - �����
        if (!censorshipDone) 
            return;

        std::wofstream file("Statistics.txt"); //�������� ����� ��� ������ ����������
        for (const auto& word : foundWords) 
        { //���� ���� �������� ����
            file << word << std::endl; //������ ����� � ����
        }
        file << "Total words: " << curseWords << std::endl; 
    }


    void CensorFiles() 
    {//�������!!!!!!!!!!!!!!!!!

        WIN32_FIND_DATAW findFileData; //���������, ������ � �����

        HANDLE hFind = FindFirstFileW(L"./*.txt", &findFileData); //����� ������ � �������
        if (hFind != INVALID_HANDLE_VALUE) 
        {
            do 
            { //���� ������
                std::wstring filePath = findFileData.cFileName; //����
                std::wifstream file(filePath); //������
                
                //���������� ������ � �����!!!
                std::wstring content((std::istreambuf_iterator<wchar_t>(file)), std::istreambuf_iterator<wchar_t>()); 
                file.close();
                int changesCount = 0; // ������� ���������, ���������� 0

                for (const auto& word : words) 
                {//������� ���� ����������� ����
                    size_t pos = content.find(word); //������ "���������" ����� � ���������� �����
                    while (pos != std::wstring::npos) 
                    { // ���� ����� �������
                        content.replace(pos, word.length(), L"*******"); //��������� �� �������
                        pos = content.find(word, pos + 7); // ���� ��������� ��������� ����� ����� 7 �������� (������ ��� ��������� 7)
                        changesCount++;
                    }
                }

                size_t extensionPos = filePath.find_last_of(L"."); //����� ����� ����� ����� �����������
                std::wstring newFileName = filePath.substr(0, extensionPos) + L"Censored.txt"; //������, �������� ������������ ����� + ��������

                std::wofstream outFile(newFileName); // ����� ����, �������� ��� ������ ���������� ��������
                outFile << content; 
                outFile.close();

                std::wofstream resultFile("ChangesResultCensored.txt", std::ios::app); //�������� ����� ��� ������
                resultFile << "File: " << filePath << ", Changes Count: " << changesCount << std::endl; //������
                resultFile.close(); 
            } while (FindNextFileW(hFind, &findFileData) != 0); //������ ���������
            
            FindClose(hFind); //�������� �����������
        }
        censorshipDone = true; //����� ����� � �������
    }

    //������� ���
    void UpdateProgressBar(int pos) { SendMessage(hProgressBar, PBM_SETPOS, pos, 0); }


    void StartSearch(HWND hwnd) 
    {//������ ���������� ������, ������ ����������, ������� � ������ ������� !!!!!!!!!!!!!!!!!!!!!1
        EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_START), FALSE); // ��������� ������ "Start"

        SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100)); //��������
        SendMessage(hProgressBar, PBM_SETSTEP, 1, 0);

        for (int i = 0; i <= 100; i++) 
        {//���� �������� ���
            UpdateProgressBar(i); //����������
            Sleep(20); //���� �������
        }

        MessageBox(hwnd, L"Search Complete", L"Info", MB_OK);

        //��� ������: �����, ������, ��������������
        HANDLE t1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CurseDetector::SearchFilesThread, this, 0, NULL);
        WaitForSingleObject(t1, INFINITE);
        CloseHandle(t1);

        HANDLE t2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CurseDetector::WriteStatisticsThread, this, 0, NULL);
        WaitForSingleObject(t2, INFINITE);
        CloseHandle(t2);

        HANDLE t3 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CurseDetector::CensorFilesThread, this, 0, NULL);
        WaitForSingleObject(t3, INFINITE);
        CloseHandle(t3);

        EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_START), TRUE); // �������� ������ "Start" 
    }

    static INT_PTR CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
    {//��������� ����������� ����
        CurseDetector* detector;
        if (message == WM_INITDIALOG) 
        {
            detector = (CurseDetector*)lParam; //��������� �� ��������� ������
            SetWindowLongPtr(hwnd, GWLP_USERDATA, lParam); //��������� �� ��������� ������ � ���������������� ������ ����
            detector->Initialize(GetDlgItem(hwnd, IDC_PROGRESS1), GetDlgItem(hwnd, IDC_LIST_CURSE)); //����������� ��������-���� � ������
        }
        else { detector = (CurseDetector*)GetWindowLongPtr(hwnd, GWLP_USERDATA); } //��������� �� ���������������� ������ ����

        return detector ? detector->HandleDialogMessage(hwnd, message, wParam, lParam) : FALSE;
    }

    // ����� ��� ��������� ��������� ����������� ����
    int HandleDialogMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
    {
        switch (message) { // ������������ ��������� ���������
        case WM_COMMAND: // ��������� � �������
            switch (LOWORD(wParam)) { // ������������ ��������� �������
            case IDC_BUTTON_MAN_IMP: // ������ ��� ������� ���������� ������������ �����
            {
                TCHAR buffer[256]; // ����� ��� �������� ���������� �����
                GetDlgItemText(hwnd, IDC_EDIT_MAN_IMP, buffer, 256); // �������� ����� �� ���������� ����
                AddCurseWord(buffer); // ��������� ����� � ���������
                SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)buffer); // ���������� ����� � ������
                MessageBox(hwnd, L"Added successfully", L"Success", MB_OK); // ������� ��������� �� �������� ����������
                return TRUE;
            }
            case IDC_BUTTON_DEL_CURSE: // ������ ��� �������� ������������ �����
            {
                int idx = SendMessage(hListbox, LB_GETCURSEL, 0, 0); // �������� ������ ���������� ����� � ������
                if (idx != LB_ERR) { // ���� ������ ���������
                    RemoveCurseWord(idx); // ������� ����� �� ���������
                    SendMessage(hListbox, LB_DELETESTRING, idx, 0); // ������� ����� �� ������
                }
                return TRUE;
            }
            case IDC_BUTTON_FILE_IMP: // ������ ��� ������� ����������� ���� �� �����
            {
                std::wstring filePath = OpenFileDialog(hwnd); // ��������� ���������� ���� ������ �����
                if (!filePath.empty()) { // ���� ���� � ����� �� ������
                    ImportCurseWordsFromFile(filePath); // ����������� ����� �� �����
                    MessageBox(hwnd, L"Import successful", L"Success", MB_ICONINFORMATION | MB_OK); // ������� ��������� �� �������� �������
                }
                return TRUE;
            }
            case IDC_BUTTON_START: // ������ ��� ������� �������� ������, ������ ���������� � ��������������
            {
                StartSearch(hwnd); // ��������� �������
                return TRUE;
            }
            }
            break;
        case WM_CLOSE: // ��������� � �������� ����
            EndDialog(hwnd, 0); // ��������� ���������� ����
            return TRUE;
        }
        return FALSE;
    }

private:
    std::set<std::wstring> words; // ��������� ��� �������� ����������� ����
    std::vector<std::wstring> foundWords; // ������ ��� �������� ��������� ����������� ����
    int curseWords; // ������� ����������� ����
    HANDLE mtx; // ���������� ��������
    bool censorshipDone; // ���� ���������� ��������������
    HWND hProgressBar; // ���������� ��������-����
    HWND hListbox; // ���������� ������

    // ����������� ������ ��� ������� ������� � ��������� �������
    static DWORD WINAPI SearchFilesThread(LPVOID lpParameter) {
        CurseDetector* detector = (CurseDetector*)lpParameter; // �������� ��������� �� ��������� ������
        detector->SearchFiles(); // ��������� ����� ����������� ���� � ������
        return 0;
    }

    static DWORD WINAPI WriteStatisticsThread(LPVOID lpParameter) {
        CurseDetector* detector = (CurseDetector*)lpParameter; // �������� ��������� �� ��������� ������
        detector->WriteStatistics(); // ���������� ���������� � ����
        return 0;
    }

    static DWORD WINAPI CensorFilesThread(LPVOID lpParameter) {
        CurseDetector* detector = (CurseDetector*)lpParameter; // �������� ��������� �� ��������� ������
        detector->CensorFiles(); // ����������� �����
        return 0;
    }
};

// �������� ������� WinMain ��� ������� ����������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    CurseDetector detector; // ������� ��������� ������ CurseDetector
    DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_CURSE_MAIN), NULL, CurseDetector::DlgProc, (LPARAM)&detector); // ��������� ���������� ����
    return 0; // ���������� ��� ���������� ����������
}
