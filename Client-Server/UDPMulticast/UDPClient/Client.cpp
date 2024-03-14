#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <WS2tcpip.h>

using namespace std;

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 4096

#define SERVER_IP "127.0.0.1"
#define DEFAULT_PORT "8888"

SOCKET client_socket;

DWORD WINAPI Sender(void* param) 
{
    while (true) 
    {
        string message;
        cout << "Enter message: ";
        getline(cin, message);
        if (message == "exit")
            break;

        send(client_socket, message.c_str(), message.size(), 0);
    }
    return 0;
}

DWORD WINAPI Receiver(void* param) 
{
    while (true) 
    {
        char response[DEFAULT_BUFLEN];
        int result = recv(client_socket, response, DEFAULT_BUFLEN, 0);
        if (result > 0) 
        {
            response[result] = '\0';
            cout << response << endl;
        }
    }
    return 0;
}

int main() 
{
    system("title Client");

    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) 
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    addrinfo* result = nullptr;
    iResult = getaddrinfo(SERVER_IP, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) 
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 2;
    }

    addrinfo* ptr = nullptr;
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) 
    {
        client_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (client_socket == INVALID_SOCKET) 
        {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 3;
        }

        iResult = connect(client_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) 
        {
            closesocket(client_socket);
            client_socket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (client_socket == INVALID_SOCKET) 
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 5;
    }

    string nickname, color;
    cout << "Enter your nickname: ";
    getline(cin, nickname);
    cout << "Choose your color (red, green, blue): ";
    getline(cin, color);

    int color_code;
    if (color == "red") color_code = 4; // RED
    else if (color == "green") color_code = 2; // GREEN
    else if (color == "blue") color_code = 1; // BLUE
    else color_code = 15; // DEFAULT_COLOR

    string init_message = nickname + "|" + to_string(color_code);
    send(client_socket, init_message.c_str(), init_message.size(), 0);

    CreateThread(0, 0, Sender, 0, 0, 0);
    CreateThread(0, 0, Receiver, 0, 0, 0);

    Sleep(INFINITE);
    return 0;
}
