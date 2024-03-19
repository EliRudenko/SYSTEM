#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>
#include <iomanip> 
#include <regex>
#pragma comment(lib, "urlmon.lib")
using namespace std;

int main()
{
    setlocale(0, "ru");
 
    string date, currency;
    cout << "Введите дату (в формате дд.мм.гггг): ";
    getline(cin, date); // Чтение 

    string day = date.substr(0, 2);
    string month = date.substr(3, 2);
    string year = date.substr(6, 4);

    string formattedDate = year + month + day;


    cout << "Введите код валюты (например, USD): ";
    cin >> currency;


    string srcURL = "https://bank.gov.ua/NBUStatService/v1/statdirectory/exchange?valcode=" + currency + "&date=" + formattedDate + "&json";


    string destFile = "file.txt";


    if (S_OK == URLDownloadToFileA(NULL, srcURL.c_str(), destFile.c_str(), 0, NULL))
    {
        cout << "Сохранено в " << destFile << "\n";

        // Чтение данных из файла
        ifstream file(destFile);
        if (file.is_open())
        {
            string line;
            while (getline(file, line))
            {
                // Поиск курса валюты в полученных данных
                if (line.find("\"rate\":") != string::npos)
                {
                    // Используем регулярное выражение для поиска числа с плавающей точкой
                    regex rate_regex(R"("rate":\s*(\d+\.\d+))");
                    smatch rate_match;
                    if (regex_search(line, rate_match, rate_regex))
                    {
                        double exchangeRate = stod(rate_match[1]);

                        cout << "Дата " << day << "." << month << "." << year << "г." << endl;
                        cout << "Валюта - " << currency << endl;
                        cout << currency << " = " << fixed << setprecision(2) << exchangeRate << " грн" << endl;

                        break;
                    }
                }
            }
            file.close();
        }
    }
    else
    {
        cout << "Ошибка загрузки данных!\n";
    }

    return 0;
}
