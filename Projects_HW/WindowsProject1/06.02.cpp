#include <windows.h>
#include <tchar.h>
#include <string>

//1

INT WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR lpszCmdLine, int nCmdShow)
{
    const char* resume = "Верните. На ОПП пж Пожалуйста";//текст для "Резюме"
    const int numWindows = 3; //кол во окон

    //ДЛИНА, потом надо для подсчета символов каждого окна для среднего кол во
    int resumeLength = lstrlenA(resume);
    int charsPerPage = resumeLength / numWindows;

    //для вывода смс в окно по частям
    for (int i = 0; i < numWindows - 1; ++i)
    {
        char windowText[256];//бефер)

        //i * charsPerPage для начальную каждой части "резюме"
        //resume + i * charsPerPage указателя на начало каждой части в строке
        strncpy_s(windowText, sizeof(windowText), resume + i * charsPerPage, charsPerPage); //КОПИРОВАНИЕ части
        windowText[charsPerPage] = '\0'; //конец строки

        MessageBoxA(NULL, windowText, "Резюме", MB_OK | MB_ICONERROR);//ВЫВОД
    }

    char lastWindowText[256];//буфер 

    int averageCharsPerPage = resumeLength / numWindows; //СРЕДНЕЕ КОЛ ВО СИМВОЛОВ
  
    //ФОРМАТИРОВАНИЕ!!
    //почему использовала wsprintfA, потому что sprintf не сработало(
    wsprintfA(lastWindowText, "Среднее число символов на странице: %d", averageCharsPerPage);

    MessageBoxA(NULL, lastWindowText, "Резюме", MB_OK | MB_ICONINFORMATION);//вывод

    return 0;
}


//2
/*
const int maxNumber = 100;//максимум 100 чисел
int secretNumbers[maxNumber];//будем записывать числа и перемешивать для рандома
int attempts;//попытки

INT WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR lpszCmdLine, int nCmdShow)
{
    srand(GetTickCount());//рандомчик

    do {
        //ПОСЛЕДОВАТЕЛЬНО
        for (int i = 0; i < maxNumber; ++i) { secretNumbers[i] = i + 1; } // числа от 1 до 100 в массив

        //В РАНДОМ ПРОШЛЫЙ МАССИВ 1-100
        for (int i = maxNumber - 1; i > 0; --i)
        {
            int j = rand() % (i + 1);
            std::swap(secretNumbers[i], secretNumbers[j]);
        }


        attempts = 0;//пока попыток нет
        do
        {
            //вопросик
            TCHAR question[100];
            _stprintf_s(question, _T("Число %d?"), secretNumbers[attempts]);

            // Да или Нет от пользователя
            int guess = MessageBox(0, question, _T("Игра 'Угадай число'"), MB_ICONQUESTION | MB_YESNO);

            if (guess == IDNO) // не угадал, значит просто  + попытка 
            {//IDNO константа, это кнопка НЕТ

                int closeResult = MessageBox(0, _T("Ты уверен что хочешь еще пробовать угадывать ?"), _T("Предложение пощады"), MB_ICONQUESTION | MB_YESNO);
                if (closeResult == IDNO)
                {
                    return 0;  // смерть
                }
            }
            else if (guess == IDYES) //УГАДАЛ = красава
            {//IDYES константа, это кнопка ДА

                attempts++; //но + попытка все равно
                break; // конец угадыванию, двльше вопр об окончании игры
            }
            else //КОНЕЦ ИГРЫ
            {
                MessageBox(0, _T("Разумно, одобряю))"), _T("Угадай число!!!"), MB_ICONINFORMATION | MB_OK);
                return 0;
            }

        } while (true);

        //Угадал = вышел с прошлого цыкла
        MessageBox(0, _T("Шикарно, наконец то. Иди лучше разбирайся в Макросах("), _T("Угадай число!!!"), MB_ICONINFORMATION | MB_OK);

        //сколько попыток
        MessageBox(0, std::to_wstring(attempts).c_str(), _T("Количество болезненых попыток"), MB_ICONINFORMATION | MB_OK);

        //зацыклимть этот ужас
        int result = MessageBox(0, _T("Хотите еще бессмысленно потратить время? Посторим?"), _T("Угадай число!!!"), MB_ICONQUESTION | MB_YESNO);

        return 0;
    } while (true);

    return 0;
}
*/