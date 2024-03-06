#include <windows.h>  
#include <string>   
#include <set>     
#include <fstream>    
#include <vector>     
#include <algorithm>  
#include <Commctrl.h> // для работы с элементами управления
#include "resource.h"


std::wstring OpenFileDialog(HWND hwnd) 
{//ДЛЯ ЗАПУСКА ПРОВОДНИКА И ВЫБОРА ФАЙЛА
    OPENFILENAME ofn; // Структура для параметров диалогового окна выбора файла
    WCHAR szFile[MAX_PATH]; // Буфер для хранения пути к выбранному файлу

    ZeroMemory(&ofn, sizeof(ofn)); // Обнуляем структуру

    ofn.lStructSize = sizeof(ofn); 
    ofn.hwndOwner = hwnd; // Устанавливаем дескриптор родительского окна
    ofn.lpstrFile = szFile;         
    ofn.lpstrFile[0] = '\0';        
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]); 
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0"; //ФИЛЬТР ДЛЯ ТИПОВ ФАЙЛОВ
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; // ФЛАГ СУЩЕСТВОВАНИЯ ПУТИ

    //ОТКРЫТИЕ ОКНА С ВЫБОРОМ ФАЙЛА, ВОЗВРАТ ПУТИ
    if (GetOpenFileName(&ofn) == TRUE) { return ofn.lpstrFile; }
    else { return L"";}
}

class CurseDetector 
{
public:
    CurseDetector() : curseWords(0), censorshipDone(false) 
    {
        mtx = CreateMutex(NULL, FALSE, NULL); // СОЗДАЕМ МЬЮТЕКС ДЛЯ СИНХРАНИЗАЦИИ ДОСТУПА К РЕСУРСАМ
        hProgressBar = nullptr; //ДЕСКРИПТОР ПРОГРЕСС БАРА = 0
        hListbox = nullptr; //ДЕСКРИПТОР СПИСКА = 0
    }

    ~CurseDetector() { CloseHandle(mtx); }

    void Initialize(HWND progressBar, HWND listbox) 
    {//ИНИЦИАЛИЗАЦИЯ ДЕСКРИПТОРОВ И ПРОГРЕСС БАРА
        hProgressBar = progressBar; 
        hListbox = listbox;       
    }

    void AddCurseWord(const std::wstring& word) 
    {//ДОБАВЛЕНИЕ НЕ ЦЕНЗУРНОГО СЛОВА В МЬЮТЕКС, ВЫВОДИТСЯ В СПИСОК
        WaitForSingleObject(mtx, INFINITE); //ЗАХВАТ мЬЮТЕКС ДЛЯ БЕЗОПАСНОГО ДОСТУПА
        words.insert(word); //ДОБАВЛЕНИЕ СЛОВА 
        ReleaseMutex(mtx);//ОСВОБОЖДЕНИЕ
    }

    void RemoveCurseWord(int index) 
    {//уДАЛЕНИЕ ПО ИНДЕКСУ СО СПИСКА
        WaitForSingleObject(mtx, INFINITE);// Захват мьютекс
        auto it = std::next(words.begin(), index); //ИТЕРАТОР НА УДАЛЕНИЕ
        words.erase(it);//УДАЛЕНИЕ
        ReleaseMutex(mtx);//СВОБОДА
    }

    void ImportCurseWordsFromFile(const std::wstring& filePath) 
    {//ИМПОРТ ИЗ ФАЙЛА
        std::wifstream file(filePath);// ОТКРЫТЬ ЧТЕНИЕ
        if (file) 
        {//ОТКРЫЛСЯ
            std::wstring word; // Переменная для хранения слова
            while (file >> word) 
            {//СЧИТЫВАНИЕ
                AddCurseWord(word);// ДОБАВЛЕНИЕ СЛОВА В МНОЖЕСТВО
                SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)word.c_str()); //ВЫВОД В СПИСОК
            }
        }
    }


    void SearchFiles() 
    {//ПОИСК!!!!!!!!!!!!!!!!!!!!!!!1
        WIN32_FIND_DATAW findFileData; //СТРУКТУРА ИНФЫ О ФАЙЛЕ
        HANDLE hFind = FindFirstFileW(L"./*.txt", &findFileData); //ТЕКУЩАЯ ПАПОЧКА, ПОИСК
       
        if (hFind != INVALID_HANDLE_VALUE) 
        {// ПОИСК - ОК =>
            do 
            {//ВСЕ НАЙДЕНЫЕ ФАЙЛЫ, ИДЕТ СКАН
                std::wstring filePath = findFileData.cFileName; //ПУТЬ К ФАЙЛУ
                std::wifstream file(filePath); //ЧТЕНИЕ
                std::wstring word; // Переменная для хранения слова
                while (file >> word) 
                { //ЧИСТА СЛОВ С ФАЙЛА
                    WaitForSingleObject(mtx, INFINITE); // Захватываем мьютекс!!!!!!!!!!1
                    
                    if (words.find(word) != words.end()) 
                    {//СЛОВО НАЙДЕНО В МНОЖЕСТВЕ
                        foundWords.push_back(word);//ВЕКТОР СЛОВ
                        curseWords++; //СЧЕТЧИК
                    }
                    ReleaseMutex(mtx); //СВОБОДА
                }
            } while (FindNextFileW(hFind, &findFileData) != 0); //ПЕРЕХОД К СЛЕДУЮЩЕМУ ФАЙЛУ
            
            FindClose(hFind); //ДЕСКРИПТОР ПОИСКА СЛОВ
        }
    }

    void WriteStatistics() 
    {//ЗАПИСЬ СТАТИСТИКИ
        //ЕСЛИ НЕ ЗАВЕРШЕНО - ВЫХОД
        if (!censorshipDone) 
            return;

        std::wofstream file("Statistics.txt"); //ОТКРЫТИЕ ФАЙЛА ДЛЯ ЗАПИСИ СТАТИСТИКИ
        for (const auto& word : foundWords) 
        { //СКАН ВСЕХ НАЙДЕНЫХ СЛОВ
            file << word << std::endl; //ЗАПИСЬ СЛОВА В ФАЙЛ
        }
        file << "Total words: " << curseWords << std::endl; 
    }


    void CensorFiles() 
    {//ЦЕНЗУРА!!!!!!!!!!!!!!!!!

        WIN32_FIND_DATAW findFileData; //СТРУКТУРА, ДАННЫЕ О ФАЙЛЕ

        HANDLE hFind = FindFirstFileW(L"./*.txt", &findFileData); //ПОИСК ФАЙЛОВ В ПАПОЧКЕ
        if (hFind != INVALID_HANDLE_VALUE) 
        {
            do 
            { //СКАН ФАЙЛОВ
                std::wstring filePath = findFileData.cFileName; //ПУТЬ
                std::wifstream file(filePath); //ЧТЕНИЕ
                
                //СЧИТЫВАНИЕ ДАННЫХ С ФАЙЛА!!!
                std::wstring content((std::istreambuf_iterator<wchar_t>(file)), std::istreambuf_iterator<wchar_t>()); 
                file.close();
                int changesCount = 0; // Счетчик изменений, ИЗНАЧАЛЬНО 0

                for (const auto& word : words) 
                {//ПЕРЕБОР ВСЕХ НЕЦЕНЗУРНЫХ СЛОВ
                    size_t pos = content.find(word); //первое "вхождение" слова в содержимое файла
                    while (pos != std::wstring::npos) 
                    { // Пока слово найдено
                        content.replace(pos, word.length(), L"*******"); //ЗВЕЗДОЧКИ ПО УСЛОВИЮ
                        pos = content.find(word, pos + 7); // Ищем следующее вхождение слова после 7 символов (ПОТОМУ ЧТО ЗВЕЗДОЧЕК 7)
                        changesCount++;
                    }
                }

                size_t extensionPos = filePath.find_last_of(L"."); //ТОЧКА ИМЕНИ ФАЙЛА ПЕРЕД РАЗРЕШЕНИЕМ
                std::wstring newFileName = filePath.substr(0, extensionPos) + L"Censored.txt"; //ИМЕЧКО, НАЗВАНИЕ ИЗНАЧАЛЬНОГО ФАЙЛА + ЦЕНЗУРКА

                std::wofstream outFile(newFileName); // НОВЫЙ ФАЙЛ, ОТКРЫТИЕ ДЛЯ ЗАПИСИ ЦЕНЗУРНОГО КОНТЕНТА
                outFile << content; 
                outFile.close();

                std::wofstream resultFile("ChangesResultCensored.txt", std::ios::app); //ОТКРЫТИЕ ФАЙЛА ДЛЯ ЗАПИСИ
                resultFile << "File: " << filePath << ", Changes Count: " << changesCount << std::endl; //ЗАПИСЬ
                resultFile.close(); 
            } while (FindNextFileW(hFind, &findFileData) != 0); //ДАЛЬШЕ СКАНИРУЕМ
            
            FindClose(hFind); //ЗАКРЫТИЕ ДЕСКРИПТОРА
        }
        censorshipDone = true; //КОНЕЦ СКАНА И ЦЕНЗУРЫ
    }

    //ПРОЦЕСС БАР
    void UpdateProgressBar(int pos) { SendMessage(hProgressBar, PBM_SETPOS, pos, 0); }


    void StartSearch(HWND hwnd) 
    {//ЗАПУСК ПРОЦЕССОРА ПОИСКА, ЗАПИСЬ СТАТИСТИКИ, ЦЕНЗУРЫ В РАЗНЫХ ПОТОКАХ !!!!!!!!!!!!!!!!!!!!!1
        EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_START), FALSE); // Отключаем кнопку "Start"

        SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100)); //ДИАПОЗОН
        SendMessage(hProgressBar, PBM_SETSTEP, 1, 0);

        for (int i = 0; i <= 100; i++) 
        {//ВЕСЬ ПРОГРЕСС БАР
            UpdateProgressBar(i); //ОБНОВЛЕНИЕ
            Sleep(20); //ТИПО ПРОЦЕСС
        }

        MessageBox(hwnd, L"Search Complete", L"Info", MB_OK);

        //ТРИ ПОТОКА: ПОИСК, ЗАПИСЬ, ЦЕНЗУРИРОВАНИЕ
        HANDLE t1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CurseDetector::SearchFilesThread, this, 0, NULL);
        WaitForSingleObject(t1, INFINITE);
        CloseHandle(t1);

        HANDLE t2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CurseDetector::WriteStatisticsThread, this, 0, NULL);
        WaitForSingleObject(t2, INFINITE);
        CloseHandle(t2);

        HANDLE t3 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CurseDetector::CensorFilesThread, this, 0, NULL);
        WaitForSingleObject(t3, INFINITE);
        CloseHandle(t3);

        EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_START), TRUE); // Включаем кнопку "Start" 
    }

    static INT_PTR CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
    {//ОБРАБОТКА ДИАЛОГОВОГО ОКНА
        CurseDetector* detector;
        if (message == WM_INITDIALOG) 
        {
            detector = (CurseDetector*)lParam; //УКАЗАТЕЛЬ НА ЭКЗЕМПЛЯР КЛАССА
            SetWindowLongPtr(hwnd, GWLP_USERDATA, lParam); //указатель на экземпляр класса в пользовательские данные окна
            detector->Initialize(GetDlgItem(hwnd, IDC_PROGRESS1), GetDlgItem(hwnd, IDC_LIST_CURSE)); //дескрипторы прогресс-бара и списка
        }
        else { detector = (CurseDetector*)GetWindowLongPtr(hwnd, GWLP_USERDATA); } //указатель из пользовательских данных окна

        return detector ? detector->HandleDialogMessage(hwnd, message, wParam, lParam) : FALSE;
    }

    // Метод для обработки сообщений диалогового окна
    int HandleDialogMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
    {
        switch (message) { // Обрабатываем различные сообщения
        case WM_COMMAND: // Сообщение о команде
            switch (LOWORD(wParam)) { // Обрабатываем различные команды
            case IDC_BUTTON_MAN_IMP: // Кнопка для ручного добавления нецензурного слова
            {
                TCHAR buffer[256]; // Буфер для хранения введенного слова
                GetDlgItemText(hwnd, IDC_EDIT_MAN_IMP, buffer, 256); // Получаем текст из текстового поля
                AddCurseWord(buffer); // Добавляем слово в множество
                SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)buffer); // Отображаем слово в списке
                MessageBox(hwnd, L"Added successfully", L"Success", MB_OK); // Выводим сообщение об успешном добавлении
                return TRUE;
            }
            case IDC_BUTTON_DEL_CURSE: // Кнопка для удаления нецензурного слова
            {
                int idx = SendMessage(hListbox, LB_GETCURSEL, 0, 0); // Получаем индекс выбранного слова в списке
                if (idx != LB_ERR) { // Если индекс корректен
                    RemoveCurseWord(idx); // Удаляем слово из множества
                    SendMessage(hListbox, LB_DELETESTRING, idx, 0); // Удаляем слово из списка
                }
                return TRUE;
            }
            case IDC_BUTTON_FILE_IMP: // Кнопка для импорта нецензурных слов из файла
            {
                std::wstring filePath = OpenFileDialog(hwnd); // Открываем диалоговое окно выбора файла
                if (!filePath.empty()) { // Если путь к файлу не пустой
                    ImportCurseWordsFromFile(filePath); // Импортируем слова из файла
                    MessageBox(hwnd, L"Import successful", L"Success", MB_ICONINFORMATION | MB_OK); // Выводим сообщение об успешном импорте
                }
                return TRUE;
            }
            case IDC_BUTTON_START: // Кнопка для запуска процесса поиска, записи статистики и цензурирования
            {
                StartSearch(hwnd); // Запускаем процесс
                return TRUE;
            }
            }
            break;
        case WM_CLOSE: // Сообщение о закрытии окна
            EndDialog(hwnd, 0); // Завершаем диалоговое окно
            return TRUE;
        }
        return FALSE;
    }

private:
    std::set<std::wstring> words; // Множество для хранения нецензурных слов
    std::vector<std::wstring> foundWords; // Вектор для хранения найденных нецензурных слов
    int curseWords; // Счетчик нецензурных слов
    HANDLE mtx; // Дескриптор мьютекса
    bool censorshipDone; // Флаг завершения цензурирования
    HWND hProgressBar; // Дескриптор прогресс-бара
    HWND hListbox; // Дескриптор списка

    // Статические методы для запуска функций в отдельных потоках
    static DWORD WINAPI SearchFilesThread(LPVOID lpParameter) {
        CurseDetector* detector = (CurseDetector*)lpParameter; // Получаем указатель на экземпляр класса
        detector->SearchFiles(); // Выполняем поиск нецензурных слов в файлах
        return 0;
    }

    static DWORD WINAPI WriteStatisticsThread(LPVOID lpParameter) {
        CurseDetector* detector = (CurseDetector*)lpParameter; // Получаем указатель на экземпляр класса
        detector->WriteStatistics(); // Записываем статистику в файл
        return 0;
    }

    static DWORD WINAPI CensorFilesThread(LPVOID lpParameter) {
        CurseDetector* detector = (CurseDetector*)lpParameter; // Получаем указатель на экземпляр класса
        detector->CensorFiles(); // Цензурируем файлы
        return 0;
    }
};

// Основная функция WinMain для запуска приложения
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    CurseDetector detector; // Создаем экземпляр класса CurseDetector
    DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_CURSE_MAIN), NULL, CurseDetector::DlgProc, (LPARAM)&detector); // Открываем диалоговое окно
    return 0; // Возвращаем код завершения приложения
}
