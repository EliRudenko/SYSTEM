#include <iostream>
#include <string>
using namespace std;

#include <ws2tcpip.h> // ������� ������� Windows
#include <windows.h> // ������� Windows
#include <thread> // ������

#include <chrono> // ��� �������
#include <iomanip> // �������������� ������

#pragma comment (lib, "Ws2_32.lib") // ������ ��� ����������� ���������� Windows

#define SERVER_IP "127.0.0.1" // IP-����� �������
#define DEFAULT_PORT "8888" // ����

SOCKET client_socket; // ���������� ��� �������� ����������� ������ �������

DWORD WINAPI Sender(void* param)
{ // ����������� ������� �� ������ � ����� ���������� � �������, ���������, ���������� ������

    cout << "Please enter your order: ";
    string query;
    getline(cin, query);

    // �������� ������� �� ������
    send(client_socket, query.c_str(), query.size(), 0);

    // �������� � ������� ���������� � ������� �������� �� �������
    char wait_message[4096]; // ��������� ������ ������ ��������
    int result = recv(client_socket, wait_message, sizeof(wait_message), 0);
    if (result > 0)
    {
        wait_message[result] = '\0';
        cout << wait_message;

        // �������� � ������� ��������� � ���������� ������
        char response[4096]; // ��������� ������ ������ ��������
        result = recv(client_socket, response, sizeof(response), 0);
        if (result > 0)
        {
            response[result] = '\0';
            cout << response << endl;

            // �������� � ������� ���������� � ��������� ������
            char cost_message[4096]; // ��������� ������ ������ ��������
            result = recv(client_socket, cost_message, sizeof(cost_message), 0);
            if (result > 0)
            {
                cost_message[result] = '\0';
                cout << cost_message;
            }
        }
    }

    return 0;
}

DWORD WINAPI Receiver(void* param)
{ // ��������� ��������� �� ������� � �����
    while (true)
    {
        char response[4096]; // ��������� ������ ������ ��������
        int result = recv(client_socket, response, sizeof(response), 0);
        if (result > 0)
        {
            response[result] = '\0';
            cout << response << endl;
        }
    }
}

int main()
{
    system("title Client"); // ��������� ����

    cout << "Menu:\n";
    cout << "1. hamburger      8.20$\n";
    cout << "1. free potato    3.80$\n";
    cout << "2. sprite         1.43$\n";
    cout << "3. pie            4.65$\n";
    cout << "4. ice cream      2.68$\n";
    cout << endl;
    // ���������� Winsock
    WSADATA wsaData;// ��������� ��� �������� ���������� � ������ ������������ Winsock API
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);// ������ ������ Winsock 2.2.
    if (iResult != 0)
    {//�������� ������������� Winsock
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    // ��������� ���������� � �������
    addrinfo* result = nullptr; //��������� ��� �������� ���� � ���������� getaddrinfo()
    iResult = getaddrinfo(SERVER_IP, DEFAULT_PORT, NULL, &result); //���������� ���� �� ������ �������
    if (iResult != 0)
    {//�������� ������� �� �������� ���� �� ������ �������
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();//������������ �������� Winsock API
        return 2;
    }

    // ����������� � �������
    for (addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {// ���� �� ���� ��������� �������, ���������� ����� getaddrinfo()
        client_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);//�������� ������!
        
        if (client_socket == INVALID_SOCKET)
        {//�������� ��� �� �� ���������
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();// ������������ �������� Winsock API
            return 3;
        }

        iResult = connect(client_socket, ptr->ai_addr, (int)ptr->ai_addrlen); //����������� � �������
        if (iResult == SOCKET_ERROR)
        {//��� �� ��
            closesocket(client_socket);//�������� ������
            client_socket = INVALID_SOCKET;
            continue; // ��������� ������
        }
        break;
    }

    freeaddrinfo(result); // ����������� ������, ���������� ��� ���������� getaddrinfo

    if (client_socket == INVALID_SOCKET)
    { // ��������� ���������� �����������
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 5;
    }

    // ������� ������ ��� �������� � ������ ������
    CreateThread(NULL, 0, Sender, NULL, 0, NULL);
    CreateThread(NULL, 0, Receiver, NULL, 0, NULL);

    Sleep(INFINITE);
}
