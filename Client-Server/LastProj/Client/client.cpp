#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main() {
    setlocale(0, "ru");
    // ������������� Winsock
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0) {
        cerr << "Can't initialize Winsock! Quitting" << endl;
        return -1;
    }

    // �������� ������
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Can't create socket! Quitting" << endl;
        WSACleanup();
        return -1;
    }

    // ��������� ������ ������� � �����
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    // ����������� � �������
    int connectResult = connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (connectResult == SOCKET_ERROR) {
        cerr << "Can't connect to server! Quitting" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }

    // ������ ����� �������
    string client_name;
    cout << "������� ���� ���: ";
    getline(cin, client_name);

    // ������ ������
    string order;
    cout << "������� ��� ����� (���������� �������� ��������): ";
    getline(cin, order);

    // �������� ������� �� ������
    string request = client_name + "|" + order;
    send(clientSocket, request.c_str(), request.size(), 0);

    // ��������� ������ �� �������
    char buffer[4096];
    ZeroMemory(buffer, 4096);
    int bytesReceived = recv(clientSocket, buffer, 4096, 0);
    if (bytesReceived > 0) {
        cout << buffer << endl;
    }

    // �������� ������ � ������� Winsock
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
