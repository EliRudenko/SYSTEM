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

// Задержка для каждого продукта в секундах
const unordered_map<string, int> preparation_times = {
    {"hamburger", 5},
    {"sprite", 1},
    {"potato", 3}
};

// Структура для хранения заказа клиента
struct Order {
    string client_name;
    vector<string> items;
};

// Очередь заказов
queue<Order> orders_queue;

// Мьютекс и условная переменная для синхронизации доступа к очереди заказов
mutex mtx;
condition_variable cv;

// Функция для обработки заказов
void process_orders(SOCKET clientSocket) {
    while (true) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] { return !orders_queue.empty(); });

        // Получаем заказ из очереди
        Order order = orders_queue.front();
        orders_queue.pop();

        // Вывод информации о заказе на сервере
        cout << "Имя клиента: " << order.client_name << endl;
        cout << "Заказ: ";
        for (const string& item : order.items) {
            cout << item << ", ";
        }
        cout << endl;

        int total_time = 0;
        // Вычисляем общее время на готовку заказа
        for (const string& item : order.items) {
            total_time += preparation_times.at(item);
        }
        cout << "Время: " << total_time << " секунд" << endl;

        // Отправляем информацию о времени и статусе заказа клиенту
        string response = "Готовится " + to_string(total_time) + " секунд, подождите";
        send(clientSocket, response.c_str(), response.size() + 1, 0);
        cout << "Отправлено клиенту: " << response << endl;

        // Готовим заказ
        for (const string& item : order.items) {
            this_thread::sleep_for(chrono::seconds(preparation_times.at(item)));
        }

        // Оповещаем клиента о готовности заказа
        response = "Заказ готов";
        send(clientSocket, response.c_str(), response.size() + 1, 0);
        cout << "Отправлено клиенту: " << response << endl;

        lock.unlock();
        cv.notify_all();

        // Задержка перед обработкой следующего заказа
        this_thread::sleep_for(chrono::seconds(9));
    }
}

int main() {
    setlocale(0, "ru");
    // Инициализация Winsock
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0) {
        cerr << "Can't initialize Winsock! Quitting" << endl;
        return -1;
    }

    // Создание сокета
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Can't create socket! Quitting" << endl;
        WSACleanup();
        return -1;
    }

    // Привязка сокета к адресу и порту
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;
    bind(serverSocket, (sockaddr*)&hint, sizeof(hint));

    // Прослушивание порта
    listen(serverSocket, SOMAXCONN);

    // Ожидание подключения клиента
    sockaddr_in client;
    int clientSize = sizeof(client);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&client, &clientSize);

    if (clientSocket == INVALID_SOCKET) {
        cerr << "Can't accept client! Quitting" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    // Закрытие слушающего сокета
    closesocket(serverSocket);

    // Принимаем и обрабатываем заказы
    process_orders(clientSocket);

    // Закрытие клиентского сокета и очистка Winsock
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
