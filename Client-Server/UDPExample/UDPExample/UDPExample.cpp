#include <iostream>
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

    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) 
    {
        std::cerr << "Failed to create server socket." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(88386); 
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) 
    {
        std::cerr << "Failed to bind server socket." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    char buffer[1024];
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    while (true) 
    {
        int bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrSize);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Error in receiving data." << std::endl;
        }
        else 
        {
            buffer[bytesReceived] = '\0';
            int colorNumber;
            std::string message;
            sscanf_s(buffer, "%d %s", &colorNumber, buffer, static_cast<unsigned>(sizeof(buffer)));


            char ipv4Address[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(clientAddr.sin_addr), ipv4Address, INET_ADDRSTRLEN);
            std::cout << "Received message from " << ipv4Address << ": " << buffer << std::endl;
        }
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
