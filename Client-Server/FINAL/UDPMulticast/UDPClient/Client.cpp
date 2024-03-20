#include <iostream>
#include <string>
using namespace std;

#include <ws2tcpip.h> // Сетевые функции Windows
#include <windows.h> // Функции Windows
#include <thread> // Потоки

#include <chrono> // Для времени
#include <iomanip> // Форматирование вывода

#pragma comment (lib, "Ws2_32.lib") // Макрос для подключения библиотеки Windows

#define SERVER_IP "127.0.0.1" // IP-адрес сервера
#define DEFAULT_PORT "8888" // Порт

SOCKET client_socket; // Переменная для хранения дескриптора сокета клиента

DWORD WINAPI Sender(void* param)
{ // Отправление запроса на сервер и вывод информации о времени, стоимости, готовности заказа

    cout << "Please enter your order: ";
    string query;
    getline(cin, query);

    // Отправка запроса на сервер
    send(client_socket, query.c_str(), query.size(), 0);

    // Получаем и выводим информацию о времени ожидания от сервера
    char wait_message[4096]; // Указываем размер буфера напрямую
    int result = recv(client_socket, wait_message, sizeof(wait_message), 0);
    if (result > 0)
    {
        wait_message[result] = '\0';
        cout << wait_message;

        // Получаем и выводим сообщение о готовности заказа
        char response[4096]; // Указываем размер буфера напрямую
        result = recv(client_socket, response, sizeof(response), 0);
        if (result > 0)
        {
            response[result] = '\0';
            cout << response << endl;

            // Получаем и выводим информацию о стоимости заказа
            char cost_message[4096]; // Указываем размер буфера напрямую
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
{ // Получение сообщения от сервера и вывод
    while (true)
    {
        char response[4096]; // Указываем размер буфера напрямую
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
    system("title Client"); // Заголовок окна

    cout << "Menu:\n";
    cout << "1. hamburger      8.20$\n";
    cout << "1. free potato    3.80$\n";
    cout << "2. sprite         1.43$\n";
    cout << "3. pie            4.65$\n";
    cout << "4. ice cream      2.68$\n";
    cout << endl;
    // Библиотека Winsock
    WSADATA wsaData;// Структура для хранения информации о версии используемой Winsock API
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);// версия версию Winsock 2.2.
    if (iResult != 0)
    {//проверка инициализации Winsock
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    // Получение информации о сервере
    addrinfo* result = nullptr; //указатель для хранения инфы о результате getaddrinfo()
    iResult = getaddrinfo(SERVER_IP, DEFAULT_PORT, NULL, &result); //получчение инфы об адресе сервера
    if (iResult != 0)
    {//проверка успешло ли получена инфа об адресе сервера
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();//освобождение ресурсов Winsock API
        return 2;
    }

    // Подключение к серверу
    for (addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {// Цикл по всем доступным адресам, полученным через getaddrinfo()
        client_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);//Создание сокета!
        
        if (client_socket == INVALID_SOCKET)
        {//проверка все ли ок создалось
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();// Освобождение ресурсов Winsock API
            return 3;
        }

        iResult = connect(client_socket, ptr->ai_addr, (int)ptr->ai_addrlen); //Подключение к серверу
        if (iResult == SOCKET_ERROR)
        {//все ли ок
            closesocket(client_socket);//Закрытие сокета
            client_socket = INVALID_SOCKET;
            continue; // следующий адресс
        }
        break;
    }

    freeaddrinfo(result); // Освобождаем память, выделенную под результаты getaddrinfo

    if (client_socket == INVALID_SOCKET)
    { // Проверяем успешность подключения
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 5;
    }

    // Создаем потоки для отправки и приема данных
    CreateThread(NULL, 0, Sender, NULL, 0, NULL);
    CreateThread(NULL, 0, Receiver, NULL, 0, NULL);

    Sleep(INFINITE);
}
