#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

const int PORT = 8888;

int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    const char *hello = "Hello from client";
    const char *login = "admin\n";
    const char *password = "password\n";

    // Создаем дескриптор сокета
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    // Устанавливаем параметры структуры sockaddr_in
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Конвертируем IPv4-адрес в сетевой формат
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Подключаемся к серверу
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Получаем приветственное сообщение от сервера
    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);
    memset(buffer, 0, sizeof(buffer));

    // Отправляем логин и пароль для авторизации
    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);
    send(sock, login, strlen(login), 0);
    memset(buffer, 0, sizeof(buffer));

    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);
    send(sock, password, strlen(password), 0);

    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);
    if (strcmp(buffer, "Login successful") != 0) {
        std::cout << "Server: login failed";
        exit(EXIT_FAILURE);
    }
    memset(buffer, 0, sizeof buffer);

    while (true) {
        std::string temp;
        std::cin >> temp;
        const char* msg = temp.c_str();
        // Отправляем сообщение на сервер
        send(sock, msg, strlen(msg), 0);
        printf("Hello message sent\n");

        memset(buffer, 0, sizeof buffer);
        // Получаем ответ от сервера
        valread = read(sock, buffer, 1024);
        printf("%s\n",buffer );

        memset(buffer, 0, sizeof buffer);
    }
    return 0;
}