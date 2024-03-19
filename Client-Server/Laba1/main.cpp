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
    cout << "������� ���� (� ������� ��.��.����): ";
    getline(cin, date); // ������ 

    string day = date.substr(0, 2);
    string month = date.substr(3, 2);
    string year = date.substr(6, 4);

    string formattedDate = year + month + day;


    cout << "������� ��� ������ (��������, USD): ";
    cin >> currency;


    string srcURL = "https://bank.gov.ua/NBUStatService/v1/statdirectory/exchange?valcode=" + currency + "&date=" + formattedDate + "&json";


    string destFile = "file.txt";


    if (S_OK == URLDownloadToFileA(NULL, srcURL.c_str(), destFile.c_str(), 0, NULL))
    {
        cout << "��������� � " << destFile << "\n";

        // ������ ������ �� �����
        ifstream file(destFile);
        if (file.is_open())
        {
            string line;
            while (getline(file, line))
            {
                // ����� ����� ������ � ���������� ������
                if (line.find("\"rate\":") != string::npos)
                {
                    // ���������� ���������� ��������� ��� ������ ����� � ��������� ������
                    regex rate_regex(R"("rate":\s*(\d+\.\d+))");
                    smatch rate_match;
                    if (regex_search(line, rate_match, rate_regex))
                    {
                        double exchangeRate = stod(rate_match[1]);

                        cout << "���� " << day << "." << month << "." << year << "�." << endl;
                        cout << "������ - " << currency << endl;
                        cout << currency << " = " << fixed << setprecision(2) << exchangeRate << " ���" << endl;

                        break;
                    }
                }
            }
            file.close();
        }
    }
    else
    {
        cout << "������ �������� ������!\n";
    }

    return 0;
}
