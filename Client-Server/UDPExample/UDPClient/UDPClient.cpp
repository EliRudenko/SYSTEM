#include <iostream>
#include <string>

#include <WinSock2.h>
#include <WS2tcpip.h> 

#pragma comment(lib, "Ws2_32.lib")

int main() 
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
    {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET) 
    {
        std::cerr << "Failed to create client socket." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    inet_pton(AF_INET, "192.168.88.184", &(serverAddr.sin_addr)); 

    std::string nickname;
    int colorNumber;
    std::cout << "Enter your nickname: ";
    std::getline(std::cin, nickname);
    std::cout << "Enter the color number: ";
    std::cin >> colorNumber;
    std::cin.ignore();

    std::string message;
    std::cout << "Enter your message: ";

    std::getline(std::cin, message);
    std::string fullMessage = std::to_string(colorNumber) + " " + message;

    if (sendto(clientSocket, fullMessage.c_str(), fullMessage.length(), 0, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) 
    {
        std::cerr << "Error in sending data." << std::endl;
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
