#include <iostream>
#include <string>
#include <chrono>
#include <thread>

using namespace std;

int main() 
{
    setlocale(0, "ru");
    string server_response;
    cout << "����� ���������� � ���� ����������!" << endl;

    while (true) {
        cout << "������� ��� �����: ";
        string order;
        getline(cin, order);

        // �������� ������ �� ������ � �������� ������
        cout << "����� ��������������..." << endl;
        this_thread::sleep_for(chrono::seconds(2)); // �������� ������� ��������� ��������

        // ��������� ������ �� �������
        cout << "��� ����� �����!" << endl;
        cout << "�����: " << server_response << endl;

        // �������� �� ������� ������ ��������
        cout << "����������, ��������..." << endl;
        this_thread::sleep_for(chrono::seconds(9)); // �������� ����� ��������� �������
        cout << "�������� ���� �����" << endl;
    }

    return 0;
}
