#include <winsock2.h>  // ��� ����������� ������� � Windows
#include <thread>      // ������

#include <iostream>

#include <string>
#include <sstream>  // ��� ������� ���������� �����-������

#include <vector>
#include <queue>
#include <map>

#include <chrono>  // ��� �������
#include <iomanip>

#pragma comment(lib, "ws2_32.lib")  // ������
#pragma warning(disable : 4996)


#define MAX_CLIENTS 10  // �������� ��������

SOCKET server_socket;  // ����������� ������ �������


struct Order {
    // ��������� � ����������� � ������
    int client_id;// ������������� �������
    std::string items;// ���������� ������
    std::chrono::seconds total_time;  // ����� ����� �� ���������� ������
};

std::queue<Order> orders_queue;  // ������� �������
std::map<std::string, int> item_processing_time = 
{
    // ����� ��������� �������
    {"hamburger", 5},
    {"sprite", 1},
    {"free potato", 3},
    {"pie", 4},
    {"ice cream", 2} };
std::map<std::string, float> item_prices = 
{
    // ���� �������
    {"hamburger", 8.20},
    {"sprite", 1.43},
    {"free potato", 3.80},
    {"pie", 4.65},
    {"ice cream", 2.68} };

void processOrder(const Order& order) 
{
    // ������� ��� ��������� ������
    std::cout << "Processing order for Client #" << order.client_id << std::endl;
    std::cout << "Order: " << order.items << std::endl;
    std::cout << "Total time for preparation: " << order.total_time.count() << " seconds" << std::endl;  // ����� �����
    std::this_thread::sleep_for(order.total_time);  // ����� �� ����� ����������
}

void sendOrderReadyMessage(SOCKET client_socket) 
{
    // ��� ����� �����
    std::string ready_message = "Your order is ready";
    send(client_socket, ready_message.c_str(), ready_message.size(), 0);
}

int client_counter = 0;  // ������� ��������

void handleClient(SOCKET client_socket) 
{
    // ��������� �������� �� ������� !!!!!!!!!!!
    client_counter++;  // ��� �� ��������

    int current_client_id = client_counter;  // ���������� �������� �������
    // �������������

    char client_message[4096];  // ����������� ������ ������ ��� ����� ������ ������� �������� (����� � �������� ������ �� ����)
    
    int client_message_length = recv(client_socket, client_message, sizeof(client_message), 0); // ��������� ��� �� �������
    client_message[client_message_length] = '\0';  // + ������ ����� ������

    std::string order_str = client_message;  // �������������� ��� � ������
    std::cout << std::endl;
    std::cout << "Received order from Client #" << current_client_id << std::endl;  // ���� � ���������� ������

    // iss ������ ������ istringstream
    // ��� ������ ������ �� ������ ��� ����� �����-������
    // ������ order_str - � �������� ��������� ������������
    // >> ������������ ��� ���������� ������ �� ������
    // �� ���� ��� ��� ���������� ���� �� ������ order_str � �������� ����

    std::istringstream iss(order_str);// �������� ������ ��� ������� ������ ������
    std::string order_keyword; // ���������� ��� �������� ��������� �������� ���� ������
    std::vector<std::string> order_keywords;// ������ ��� �������� �������� ����
    // ������

    while (iss >> order_keyword) 
    {
        // ������� ������ ������
        if (item_processing_time.find(order_keyword) != item_processing_time.end()) 
        {
            // �������� ������� ������ � ���� � ��������
            order_keywords.push_back(order_keyword);// ���������� ������ � ������
        }
        else if (order_keyword == "ice" && iss >> order_keyword && order_keyword == "cream") 
        {// ����� �� ���� ���� 
            order_keywords.push_back("ice cream");
        }
        else if (order_keyword == "free" && iss >> order_keyword && order_keyword == "potato") 
        {
            order_keywords.push_back("free potato");
        }
    }

    if (!order_keywords.empty()) 
    {// �������� ���� �� ���������� �����

        std::string order_list;  // ������ ��� �������� ������ ������� ������
        for (size_t i = 0; i < order_keywords.size(); ++i) 
        { // ������������ ������ ������� ������

            order_list += order_keywords[i];  // ���������� � ������
            if (i != order_keywords.size() - 1) 
            { // �������� �� ��������� ������� ������ 
                order_list += ", ";  // ��� �������
            }
        }

        // ��� �� ������
        std::string order_message = "Your order: " + order_list + "\n";  // ��� ������ �������� ����� �� ������ �������
        send(client_socket, order_message.c_str(), order_message.size(), 0);// �������� ��� �������
        // ������
        std::cout << "Order: " << order_list << std::endl;  // ���� ����� �� �� �������

         // �����!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        int total_time_seconds = 0;  // ����������
        for (const auto& keyword : order_keywords) 
        { // ������� ������ �������
           
            total_time_seconds += item_processing_time[keyword]; // ����������� ������� ������� ������
        }
        // � ����� ��� ��������
        // std::chrono::seconds �������� � ��������

        std::chrono::seconds total_time(total_time_seconds);  // �������� ������� �������
        // ��� �� ������
        std::string time_message = "Total time for preparation: " + std::to_string(total_time.count()) + " seconds\n";
        send(client_socket, time_message.c_str(), time_message.size(), 0);// �������� ��� �������
        
        // ������
        std::cout << "----------------------------" << std::endl;
        std::cout << "Total time for preparation: " << total_time.count() << " seconds" << std::endl;  // ���� ����� �� �� �������

        // ���������!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // ������� ������ ����� ��������
        std::string items_price_message;  // ��� � ������
        for (const auto& keyword : order_keywords) 
        {
            float item_cost = item_prices[keyword];  // ��������� ��� ������
            std::ostringstream cost_stream;  // ����� ��� �������������� ����� � ������
            cost_stream << std::fixed << std::setprecision(2) << item_cost;// ��������� ������� ����� � ��������� ������
            items_price_message += keyword + "\t" + cost_stream.str() + "$\n";  // ���������� ���� ������ � �����
        }

        // ��� �� ������
        send(client_socket, items_price_message.c_str(), items_price_message.size(), 0);
        std::cout << items_price_message;

        // ��������
        float total_cost = 0.0f;  // ����� ��������� ������
        for (const auto& keyword : order_keywords) 
        {
            // ����� ���������
            total_cost += item_prices[keyword];  // ����������� ������� ������
        }
        std::ostringstream total_cost_stream;  // ����� ��� �������������� ����� � ������
        total_cost_stream << std::fixed << std::setprecision(2) << total_cost;
        // ������ � ��������� ������

        // ��� �� ������
        std::string total_cost_message = "Total cost: " + total_cost_stream.str() + "$\n";
        send(client_socket, total_cost_message.c_str(), total_cost_message.size(), 0);// ���������� �� ������ �������

        // ������
        std::cout << "Total cost: $" << std::fixed << std::setprecision(2) << total_cost << std::endl;
        std::cout << "----------------------------" << std::endl;

        // �������� ���������� ���������, ��� ��������� � ��
        std::this_thread::sleep_for(total_time);

        // ��� �� ������
        std::string ready_message = "Your order is ready\n";
        send(client_socket, ready_message.c_str(), ready_message.size(), 0); // �������� ��� ��� ������, ����� ��������� �������

        // ������
        std::cout << "Order is ready" << std::endl;
    }
    else 
    {
        // ������ � ������� = ������
        std::string error_message = "Invalid order. Please include valid keywords.\n";
        send(client_socket, error_message.c_str(), error_message.size(), 0);
        std::cout << "Invalid order. Please include valid keywords." << std::endl;
    }

    closesocket(client_socket);  // �������� ������ �������
}


int main()
{
    system("title Server"); // ��������� ����

    WSADATA wsa; //������������� ���������� Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {//������������� Winsock
        printf("Failed. Error Code: %d", WSAGetLastError()); // ������
        return 1;
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {//�������� ������ �������
        printf("Could not create socket: %d", WSAGetLastError());
        return 2;
    }

    sockaddr_in server;//��������� ��� �������� ���� � �������
    server.sin_family = AF_INET; //��������� �������
    server.sin_addr.s_addr = INADDR_ANY; // ��������� IP-����� �������
    server.sin_port = htons(8888); //���� �������

    if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {//�������� ������ � ������ � ����� �������
        printf("Bind failed with error code: %d", WSAGetLastError());
        return 3;
    }

    listen(server_socket, MAX_CLIENTS); // ����� �������������
    puts("Server is waiting for incoming connections...");

    fd_set readfds; //��������� ������� ��� �������� �������
    SOCKET client_socket[MAX_CLIENTS] = {}; //������ ��� �������� ���������� �������

    while (true)
    {//��������� �����������
        FD_ZERO(&readfds); // ������� ��������� �������
        FD_SET(server_socket, &readfds);//���������� ���������� ������ � ���������
        for (int i = 0; i < MAX_CLIENTS; i++) 
        {//���������� ��������� ������� � ���������
            SOCKET s = client_socket[i];
            if (s > 0) { FD_SET(s, &readfds);}
        }

        if (select(0, &readfds, NULL, NULL, NULL) == SOCKET_ERROR) 
        {//�������� ������� �� �������
            printf("select function call failed with error code : %d", WSAGetLastError());
            return 4;
        }

        SOCKET new_socket; //���������� ��� ������ �������
        sockaddr_in address;//��������� ��� �������� ���������� � ���������� ������
        int addrlen = sizeof(sockaddr_in);//������ ��������� ������

        if (FD_ISSET(server_socket, &readfds)) 
        {//���� �� ������� ������� �� ��������� ������
            if ((new_socket = accept(server_socket, (sockaddr*)&address, &addrlen)) < 0) 
            {//��������� ����� �����������
                perror("accept function error");
                return 5;
            }

            for (int i = 0; i < MAX_CLIENTS; i++) 
            {//���������� ���������� ������
                if (client_socket[i] == 0) 
                {//���������� ����� ������
                    client_socket[i] = new_socket;//����������� ����� ���������� �����
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) 
        {
            SOCKET s = client_socket[i];//����������� �������� ����������� ������ �������� �� �������
            if (FD_ISSET(s, &readfds)) 
            {//���� �� ������� �� ���� ���������� ������
                handleClient(s);//���������� ��� ��������
                client_socket[i] = 0;//��������� �������� ����������� ������
            }
        }
    }

    WSACleanup();
}
