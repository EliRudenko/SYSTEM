#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <unordered_map> 
#include <queue>
#include <stdexcept>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

// �������� ��� ������� �������� � ��������
const unordered_map<string, int> preparation_times = {
    {"hamburger", 5},
    {"sprite", 1},
    {"potato", 3}
};

// ��������� ��� �������� ������ �������
struct Order {
    string client_name;
    vector<string> items;
};

// ������� �������
queue<Order> orders_queue;

// ������� � �������� ���������� ��� ������������� ������� � ������� �������
mutex mtx;
condition_variable cv;

// ������� ��� ��������� �������
void process_orders(SOCKET clientSocket) {
    while (true) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] { return !orders_queue.empty(); });

        // �������� ����� �� �������
        Order order = orders_queue.front();
        orders_queue.pop();

        // ����� ���������� � ������ �� �������
        cout << "��� �������: " << order.client_name << endl;
        cout << "�����: ";
        for (const string& item : order.items) {
            cout << item << ", ";
        }
        cout << endl;

        int total_time = 0;
        // ��������� ����� ����� �� ������� ������
        for (const string& item : order.items) {
            total_time += preparation_times.at(item);
        }
        cout << "�����: " << total_time << " ������" << endl;

        // ���������� ���������� � ������� � ������� ������ �������
        string response = "��������� " + to_string(total_time) + " ������, ���������";
        send(clientSocket, response.c_str(), response.size() + 1, 0);
        cout << "���������� �������: " << response << endl;

        // ������� �����
        for (const string& item : order.items) {
            this_thread::sleep_for(chrono::seconds(preparation_times.at(item)));
        }

        // ��������� ������� � ���������� ������
        response = "����� �����";
        send(clientSocket, response.c_str(), response.size() + 1, 0);
        cout << "���������� �������: " << response << endl;

        lock.unlock();
        cv.notify_all();

        // �������� ����� ���������� ���������� ������
        this_thread::sleep_for(chrono::seconds(9));
    }
}

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
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Can't create socket! Quitting" << endl;
        WSACleanup();
        return -1;
    }

    // �������� ������ � ������ � �����
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;
    bind(serverSocket, (sockaddr*)&hint, sizeof(hint));

    // ������������� �����
    listen(serverSocket, SOMAXCONN);

    // �������� ����������� �������
    sockaddr_in client;
    int clientSize = sizeof(client);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&client, &clientSize);

    if (clientSocket == INVALID_SOCKET) {
        cerr << "Can't accept client! Quitting" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    // �������� ���������� ������
    closesocket(serverSocket);

    // ��������� � ������������ ������
    process_orders(clientSocket);

    // �������� ����������� ������ � ������� Winsock
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
