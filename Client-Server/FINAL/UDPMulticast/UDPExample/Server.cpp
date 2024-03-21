#include <winsock2.h>  // для функционала сокетов в Windows
#include <thread>      // потоки

#include <iostream>

#include <string>
#include <sstream>  // для потоков строкового ввода-вывода

#include <vector>
#include <queue>
#include <map>

#include <chrono>  // для времени
#include <iomanip>

#pragma comment(lib, "ws2_32.lib")  // макрос
#pragma warning(disable : 4996)


#define MAX_CLIENTS 10  // максимум клиентов

SOCKET server_socket;  // Определение сокета сервера


struct Order {
    // структура с информацией о заказе
    int client_id;// идентификатор клиента
    std::string items;// заказанные товары
    std::chrono::seconds total_time;  // общее время на выполнение заказа
};

std::queue<Order> orders_queue;  // очередь заказов
std::map<std::string, int> item_processing_time = 
{
    // время обработки товаров
    {"hamburger", 5},
    {"sprite", 1},
    {"free potato", 3},
    {"pie", 4},
    {"ice cream", 2} };
std::map<std::string, float> item_prices = 
{
    // цены товаров
    {"hamburger", 8.20},
    {"sprite", 1.43},
    {"free potato", 3.80},
    {"pie", 4.65},
    {"ice cream", 2.68} };

void processOrder(const Order& order) 
{
    // функция для обработки заказа
    std::cout << "Processing order for Client #" << order.client_id << std::endl;
    std::cout << "Order: " << order.items << std::endl;
    std::cout << "Total time for preparation: " << order.total_time.count() << " seconds" << std::endl;  // общее время
    std::this_thread::sleep_for(order.total_time);  // пауза на время подготовки
}

void sendOrderReadyMessage(SOCKET client_socket) 
{
    // смс заказ готов
    std::string ready_message = "Your order is ready";
    send(client_socket, ready_message.c_str(), ready_message.size(), 0);
}

int client_counter = 0;  // счетчик клиентов

void handleClient(SOCKET client_socket) 
{
    // ОБРАТОЧКА ЗАПРОСОВ ОТ КЛИЕНТА !!!!!!!!!!!
    client_counter++;  // КОЛ ВО КЛИЕНТОВ

    int current_client_id = client_counter;  // ПРИСВОЕНИЕ ТЕКУЩЕМУ КЛИЕНТУ
    // ИДЕНТИФИКАТОР

    char client_message[4096];  // Стандартный размер буфера для ввода вывода сетевых операций (прием и отправка данных по сети)
    
    int client_message_length = recv(client_socket, client_message, sizeof(client_message), 0); // Получение смс от клиента
    client_message[client_message_length] = '\0';  // + символ конца строки

    std::string order_str = client_message;  // Преобразование смс в строку
    std::cout << std::endl;
    std::cout << "Received order from Client #" << current_client_id << std::endl;  // Инфа о полученном заказе

    // iss объект класса istringstream
    // для чтение данных из строки как поток ввода-вывода
    // Строка order_str - в качестве аргумента конструктора
    // >> используется для извлечения данных из потока
    // То есть это для извлечение слов из строки order_str в процессе чтки

    std::istringstream iss(order_str);// Создание потока для разбора строки заказа
    std::string order_keyword; // Переменная для хранения отдельных ключевых слов заказа
    std::vector<std::string> order_keywords;// Вектор для хранения ключевых слов
    // заказа

    while (iss >> order_keyword) 
    {
        // Парсинг строки заказа
        if (item_processing_time.find(order_keyword) != item_processing_time.end()) 
        {
            // Проверка наличия товара в мапе с временем
            order_keywords.push_back(order_keyword);// Добавление товара в список
        }
        else if (order_keyword == "ice" && iss >> order_keyword && order_keyword == "cream") 
        {// Товар из двух слов 
            order_keywords.push_back("ice cream");
        }
        else if (order_keyword == "free" && iss >> order_keyword && order_keyword == "potato") 
        {
            order_keywords.push_back("free potato");
        }
    }

    if (!order_keywords.empty()) 
    {// Проверка есть ли заказанные блюда

        std::string order_list;  // Строка для хранения списка товаров заказа
        for (size_t i = 0; i < order_keywords.size(); ++i) 
        { // Формирование списка товаров заказа

            order_list += order_keywords[i];  // Добавление в список
            if (i != order_keywords.size() - 1) 
            { // Проверка на последний элемент списка 
                order_list += ", ";  // Для красоты
            }
        }

        // ГОУ ТО КЛИЕНТ
        std::string order_message = "Your order: " + order_list + "\n";  // Тут только ключевые слова из строки клиента
        send(client_socket, order_message.c_str(), order_message.size(), 0);// Отправка смс клиенту
        // СЕРВЕР
        std::cout << "Order: " << order_list << std::endl;  // Тоже самое но на сервере

         // ВРЕМЯ!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        int total_time_seconds = 0;  // Изначально
        for (const auto& keyword : order_keywords) 
        { // Подсчет общего времени
           
            total_time_seconds += item_processing_time[keyword]; // Прибавление времени каждого товара
        }
        // В целом для задержки
        // std::chrono::seconds Интервал в секундах

        std::chrono::seconds total_time(total_time_seconds);  // Создание объекта времени
        // ГОУ ТО КЛИЕНТ
        std::string time_message = "Total time for preparation: " + std::to_string(total_time.count()) + " seconds\n";
        send(client_socket, time_message.c_str(), time_message.size(), 0);// Отправка смс клиенту
        
        // СЕРВЕР
        std::cout << "----------------------------" << std::endl;
        std::cout << "Total time for preparation: " << total_time.count() << " seconds" << std::endl;  // Тоже самое но на сервере

        // СТОИМОСТЬ!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // Сначала каждый товар отдельно
        std::string items_price_message;  // Смс с ценами
        for (const auto& keyword : order_keywords) 
        {
            float item_cost = item_prices[keyword];  // Получение цен товара
            std::ostringstream cost_stream;  // Поток для преобразования чисел в строку
            cost_stream << std::fixed << std::setprecision(2) << item_cost;// Установка формата числа с плавающей точкой
            items_price_message += keyword + "\t" + cost_stream.str() + "$\n";  // Добавление этой строки с ценой
        }

        // ГОУ ТО КЛИЕНТ
        send(client_socket, items_price_message.c_str(), items_price_message.size(), 0);
        std::cout << items_price_message;

        // СУММАРНО
        float total_cost = 0.0f;  // Общая стоимость заказа
        for (const auto& keyword : order_keywords) 
        {
            // Общая стоимость
            total_cost += item_prices[keyword];  // Прибавление каждого товара
        }
        std::ostringstream total_cost_stream;  // Поток для преобразования чисел в строку
        total_cost_stream << std::fixed << std::setprecision(2) << total_cost;
        // Формат с плавающей точкой

        // ГОУ ТО КЛИЕНТ
        std::string total_cost_message = "Total cost: " + total_cost_stream.str() + "$\n";
        send(client_socket, total_cost_message.c_str(), total_cost_message.size(), 0);// Отправочка по сокету клиенту

        // СЕРВЕР
        std::cout << "Total cost: $" << std::fixed << std::setprecision(2) << total_cost << std::endl;
        std::cout << "----------------------------" << std::endl;

        // ЗАДЕРЖКА ВЫПОЛНЕНИЯ ПРОГРАММЫ, ТИП ГОТОВИТСЯ И ТП
        std::this_thread::sleep_for(total_time);

        // ГОУ ТО КЛИЕНТ
        std::string ready_message = "Your order is ready\n";
        send(client_socket, ready_message.c_str(), ready_message.size(), 0); // Отправка что все готово, после истечения времени

        // СЕРВЕР
        std::cout << "Order is ready" << std::endl;
    }
    else 
    {
        // ТРАБЛЫ С ЗАКАЗАМ = ОШИБКИ
        std::string error_message = "Invalid order. Please include valid keywords.\n";
        send(client_socket, error_message.c_str(), error_message.size(), 0);
        std::cout << "Invalid order. Please include valid keywords." << std::endl;
    }

    closesocket(client_socket);  // ЗАКРЫТИЕ СОКЕТА КЛИЕНТА
}


int main()
{
    system("title Server"); // ЗАГОЛОВОК ОКНА

    WSADATA wsa; //ИНИЦИАЛИЗАУИЯ БИБЛИОТЕКИ Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {//ИНИЦИАЛИЗАЦИЯ Winsock
        printf("Failed. Error Code: %d", WSAGetLastError()); // ТРАБЛЫ
        return 1;
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {//СОЗДАНИЕ СОКЕТА СЕРВЕРА
        printf("Could not create socket: %d", WSAGetLastError());
        return 2;
    }

    sockaddr_in server;//СТРУКТУРА ДЛЯ ХРАНЕНИЯ ИНФЫ О СЕРВЕРЕ
    server.sin_family = AF_INET; //СЕМЕЙСТВО АДРЕСОВ
    server.sin_addr.s_addr = INADDR_ANY; // Указываем IP-адрес сервера
    server.sin_port = htons(8888); //ПОРТ СЕРВЕРА

    if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {//ПРИВЯЗКА СОКЕТА К АДРЕСУ И ПОРТУ СЕРВЕРА
        printf("Bind failed with error code: %d", WSAGetLastError());
        return 3;
    }

    listen(server_socket, MAX_CLIENTS); // РЕДИМ ПРОСЛУШИВАНИЯ
    puts("Server is waiting for incoming connections...");

    fd_set readfds; //МНОЖЕСТВО СОКЕТОВ ДЛЯ ОЖИДАНИЯ СОБЫТИЙ
    SOCKET client_socket[MAX_CLIENTS] = {}; //МАССИВ ДЛЯ ХРАНЕНИЯ КЛИЕНТСКИХ СОКЕТОВ

    while (true)
    {//ОБРАБОТКА ПОДКЛЮЧЕНИЙ
        FD_ZERO(&readfds); // ОЧИСТКА МНОЖЕСТВО СОКЕТОВ
        FD_SET(server_socket, &readfds);//ДОБАВЛЕНИЕ СЕРВЕРНОГО СОКЕТА В МНОЖЕСТВО
        for (int i = 0; i < MAX_CLIENTS; i++) 
        {//ДОБАВЛЕНИЕ КЛИЕНСКИХ СОКЕТОВ В МНОЖЕСТВО
            SOCKET s = client_socket[i];
            if (s > 0) { FD_SET(s, &readfds);}
        }

        if (select(0, &readfds, NULL, NULL, NULL) == SOCKET_ERROR) 
        {//ОЖИДАНИЕ СОБЫТИЙ НА СОКЕТАХ
            printf("select function call failed with error code : %d", WSAGetLastError());
            return 4;
        }

        SOCKET new_socket; //ПЕРЕМЕННАЯ ДЛЯ НОВОГО КЛИЕНТА
        sockaddr_in address;//СТРУКТУРА ДЛЯ ХРАНЕНИЯ ИНФОРМАЦИИ О КЛИЕНТСКОМ АДРЕСЕ
        int addrlen = sizeof(sockaddr_in);//РАЗМЕР СТРУКТУРЫ АДРЕСА

        if (FD_ISSET(server_socket, &readfds)) 
        {//ЕСТЬ ЛИ НАЛИЧИЕ СОБЫТИЯ НА СЕРВЕРНОМ СОКЕТЕ
            if ((new_socket = accept(server_socket, (sockaddr*)&address, &addrlen)) < 0) 
            {//ПРИНИМАЕМ НОВОЕ ПОДКЛЮЧЕНИЕ
                perror("accept function error");
                return 5;
            }

            for (int i = 0; i < MAX_CLIENTS; i++) 
            {//ПЕРЕБИРАЕМ КЛИЕНТСКИЕ СОКЕТЫ
                if (client_socket[i] == 0) 
                {//КЛИЕНТСКИЙ СОКЕТ ПУСТОЙ
                    client_socket[i] = new_socket;//ПРИСВАЕВАЕМ НОВЫЙ КЛИЕНТСКИЙ СОКЕТ
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) 
        {
            SOCKET s = client_socket[i];//ПРИСВАИВАЕМ ТЕКУЩЕМУ КЛИЕНТСКОМУ СОКЕТУ ЗНАЧЕНИЕ ИЗ МАССИВА
            if (FD_ISSET(s, &readfds)) 
            {//ЕСТЬ ЛИ СОБЫТИЯ НА ЭТОМ КЛИЕНТСКОМ СОКЕТЕ
                handleClient(s);//СОБСТВЕННО ВСЕ ДЕЙСТВИЯ
                client_socket[i] = 0;//ОБНУЛЕНИЕ ТЕКУЩЕГО КЛИЕНСТКОГО СОКЕТА
            }
        }
    }

    WSACleanup();
}
