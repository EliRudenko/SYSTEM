#include <iostream>
#include <string>
#include <ctime>
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <cmath> 

using namespace std;

string unixTimeToString(time_t unixTime) 
{
    struct tm timeinfo;
    localtime_s(&timeinfo, &unixTime);
    char buffer[80];
    strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S", &timeinfo);
    return string(buffer);
}

int main()
{
    setlocale(0, "ru");

    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);

    int err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) 
    {
        cout << "WSAStartup failed with error: " << err << endl;
        return 1;
    }

    char hostname[255] = "api.openweathermap.org";

    addrinfo* result = NULL;

    addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int iResult = getaddrinfo(hostname, "http", &hints, &result);
    if (iResult != 0) 
    {
        cout << "getaddrinfo failed with error: " << iResult << endl;
        WSACleanup();
        return 3;
    }

    SOCKET connectSocket = INVALID_SOCKET;
    addrinfo* ptr = NULL;

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) 
    {
        connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (connectSocket == INVALID_SOCKET) 
        {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) 
        {
            closesocket(connectSocket);
            connectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    // HTTP Request
    string uri = "/data/2.5/weather?q=Odessa&appid=75f6e64d49db78658d09cb5ab201e483&mode=JSON";
    string request = "GET " + uri + " HTTP/1.1\n";
    request += "Host: " + string(hostname) + "\n";
    request += "Accept: */*\n";
    request += "Accept-Encoding: gzip, deflate, br\n";
    request += "Connection: close\n";
    request += "\n";

    if (send(connectSocket, request.c_str(), request.length(), 0) == SOCKET_ERROR) 
    {
        cout << "send failed: " << WSAGetLastError() << endl;
        closesocket(connectSocket);
        WSACleanup();
        return 5;
    }
    cout << "send data" << endl;

    // HTTP Response
    string response;
    const size_t BUFFERSIZE = 1024;
    char resBuf[BUFFERSIZE];
    int respLength;

    do {
        respLength = recv(connectSocket, resBuf, BUFFERSIZE, 0);
        if (respLength > 0) 
        {
            response += string(resBuf, respLength);
        }
        else 
        {
            cout << "recv failed: " << WSAGetLastError() << endl;
            closesocket(connectSocket);
            WSACleanup();
            return 6;
        }

    } while (respLength == BUFFERSIZE);

    size_t pos;
    string date, country, city, coordinates, temperature, sunrise, sunset;


    pos = response.find("\"dt\":");
    date = "Дата: " + unixTimeToString(stoi(response.substr(pos + 6, response.find(",", pos) - pos - 6)));

    pos = response.find("\"country\":\"");
    country = "Страна: " + response.substr(pos + 11, 2);


    pos = response.find("\"name\":\"");
    city = "Город: " + response.substr(pos + 8, response.find("\",", pos) - pos - 8);

    pos = response.find("\"coord\":");
    size_t lon_pos = response.find("\"lon\":", pos);
    size_t lat_pos = response.find("\"lat\":", pos);
    size_t lon_end_pos = response.find(",", lon_pos);
    size_t lat_end_pos = response.find(",", lat_pos);
    coordinates = "Координаты: " + response.substr(lon_pos + 6, lon_end_pos - lon_pos - 6) + ", " + response.substr(lat_pos + 6, lat_end_pos - lat_pos - 6);



    pos = response.find("\"temp\":");
    float temperatureKelvin = stof(response.substr(pos + 7, response.find(",", pos) - pos - 7));
    // Округляем температуру до двух знаков после запятой
    float temperatureCelsius = temperatureKelvin - 273.15;
    temperatureCelsius = std::round(temperatureCelsius * 100) / 100;

    std::string temperatureStr = std::to_string(temperatureCelsius);
    size_t dotPos = temperatureStr.find(".");
    if (dotPos != std::string::npos && dotPos + 3 < temperatureStr.size()) 
    {
        temperatureStr = temperatureStr.substr(0, dotPos + 3);
    }
    temperature = "Температура в C: " + temperatureStr;



    pos = response.find("\"sunrise\":");
    sunrise = "Рассвет: " + unixTimeToString(stoi(response.substr(pos + 10, response.find(",", pos) - pos - 10)));


    pos = response.find("\"sunset\":");
    sunset = "Закат: " + unixTimeToString(stoi(response.substr(pos + 9, response.find(",", pos) - pos - 9)));



    // Вывод данных
    cout << date << endl;
    cout << country << endl;
    cout << city << endl;
    cout << coordinates << endl;
    cout << temperature << endl;
    cout << sunrise << endl;
    cout << sunset << endl;


    iResult = shutdown(connectSocket, SD_BOTH);
    if (iResult == SOCKET_ERROR)
    {
        cout << "shutdown failed: " << WSAGetLastError() << endl;
        closesocket(connectSocket);
        WSACleanup();
        return 7;
    }

    closesocket(connectSocket);
    WSACleanup();

    return 0;
}
