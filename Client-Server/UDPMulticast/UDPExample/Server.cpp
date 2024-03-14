#include <winsock2.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <WS2tcpip.h>
#include <sstream>
#include <iomanip>

using namespace std;

#define MAX_CLIENTS 10
#define DEFAULT_BUFLEN 4096

#pragma comment(lib, "ws2_32.lib")

SOCKET server_socket;

vector<string> history;

map<SOCKET, pair<string, int>> client_info;

enum Colors 
{
    DEFAULT_COLOR,
    RED,
    GREEN,
    BLUE
};

void set_color(int color_code) 
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    switch (color_code) 
    {
    case RED:
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
        break;
    case GREEN:
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
        break;
    case BLUE:
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
        break;
    default:
        SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
        break;
    }
}

void reset_color() 
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
}

int main() 
{
    system("title Server");

    puts("Start server... DONE.");
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
    {
        printf("Failed. Error Code: %d", WSAGetLastError());
        return 1;
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) 
    {
        printf("Could not create socket: %d", WSAGetLastError());
        return 2;
    }

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) 
    {
        printf("Bind failed with error code: %d", WSAGetLastError());
        return 3;
    }

    listen(server_socket, MAX_CLIENTS);

    fd_set readfds;
    SOCKET client_socket[MAX_CLIENTS] = {};

    while (true) 
    {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);
        for (int i = 0; i < MAX_CLIENTS; i++) 
        {
            SOCKET s = client_socket[i];
            if (s > 0) { FD_SET(s, &readfds); }
        }

        if (select(0, &readfds, NULL, NULL, NULL) == SOCKET_ERROR) 
        {
            printf("select function call failed with error code : %d", WSAGetLastError());
            return 4;
        }

        SOCKET new_socket;
        sockaddr_in address;
        int addrlen = sizeof(sockaddr_in);
        if (FD_ISSET(server_socket, &readfds)) 
        {
            if ((new_socket = accept(server_socket, (sockaddr*)&address, &addrlen)) < 0) 
            {
                perror("accept function error");
                return 5;
            }

            char client_message[DEFAULT_BUFLEN];
            int client_message_length = recv(new_socket, client_message, DEFAULT_BUFLEN, 0);
            client_message[client_message_length] = '\0';

            string client_name, color_code_str;
            stringstream ss(client_message);
            getline(ss, client_name, '|');
            getline(ss, color_code_str);

            int color_code = stoi(color_code_str);

            client_info[new_socket] = make_pair(client_name, color_code);

            char ip_buffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(address.sin_addr), ip_buffer, INET_ADDRSTRLEN);

            set_color(color_code);
            printf("New connection, socket fd is %d, ip is: %s, port: %d, nickname: %s\n", new_socket, ip_buffer, ntohs(address.sin_port), client_name.c_str());
            reset_color();

            for (int i = 0; i < MAX_CLIENTS; i++) 
            {
                if (client_socket[i] != 0) 
                {
                    set_color(client_info[new_socket].second);
                    send(client_socket[i], "New client connected.", 21, 0);
                    reset_color();
                }
            }

            for (int i = 0; i < history.size(); i++) 
            {
                cout << history[i] << "\n";
                set_color(client_info[new_socket].second);
                send(new_socket, history[i].c_str(), history[i].size(), 0);
                reset_color();
            }

            for (int i = 0; i < MAX_CLIENTS; i++) 
            {
                if (client_socket[i] == 0) 
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets at index %d\n", i);
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) 
        {
            SOCKET s = client_socket[i];
            if (FD_ISSET(s, &readfds)) 
            {
                sockaddr_in address;
                int addrlen = sizeof(address);
                getpeername(s, (sockaddr*)&address, &addrlen);

                char client_message[DEFAULT_BUFLEN];
                int client_message_length = recv(s, client_message, DEFAULT_BUFLEN, 0);
                client_message[client_message_length] = '\0';

                string check_exit = client_message;
                if (check_exit == "off") 
                {
                    cout << "Client #" << i << " is off\n";
                    client_socket[i] = 0;
                }

                string temp = client_message;
                history.push_back(temp);

                for (int i = 0; i < MAX_CLIENTS; i++) 
                {
                    if (client_socket[i] != 0) 
                    {
                        set_color(client_info[s].second);
                        send(client_socket[i], client_message, client_message_length, 0);
                        reset_color();
                    }
                }
            }
        }
    }

    WSACleanup();
    return 0;
}
