#include <iostream>
#include <string>
#include <chrono>
#include <thread>

using namespace std;

int main() 
{
    setlocale(0, "ru");
    string server_response;
    cout << "Добро пожаловать в кафе Макдоналдс!" << endl;

    while (true) {
        cout << "Введите ваш заказ: ";
        string order;
        getline(cin, order);

        // Передача заказа на сервер и ожидание ответа
        cout << "Заказ обрабатывается..." << endl;
        this_thread::sleep_for(chrono::seconds(2)); // Эмуляция времени обработки сервером

        // Получение ответа от сервера
        cout << "Ваш заказ готов!" << endl;
        cout << "Заказ: " << server_response << endl;

        // Проверка на наличие других клиентов
        cout << "Пожалуйста, ожидайте..." << endl;
        this_thread::sleep_for(chrono::seconds(9)); // Задержка перед следующим заказом
        cout << "Сделайте свой заказ" << endl;
    }

    return 0;
}
