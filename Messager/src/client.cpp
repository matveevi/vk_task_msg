#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// Закрытие сокета
void close_connection(int socket) {
    shutdown(socket, SHUT_RDWR);
    close(socket);
}

const int PORT = 8888;

int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

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

    // Запрос на регистрацию
    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);
    memset(buffer, 0, sizeof(buffer));

    std::string input;
    std::cin >> input;
    const char* query = input.c_str();
    send(sock, query, strlen(query), 0);

    // Отправляем логин и пароль для авторизации
    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);
    std::cin >> input;
    const char* login = input.c_str();
    send(sock, login, strlen(login), 0);
    memset(buffer, 0, sizeof(buffer));

    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);
    std::cin >> input;
    const char* password = input.c_str();
    send(sock, password, strlen(password), 0);
    memset(buffer, 0, sizeof(buffer));

    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);
    if (strcmp(buffer, "Reg/Login successful") != 0) {
        std::cout << "Server: login failed";
        exit(EXIT_FAILURE);
    }
    memset(buffer, 0, sizeof buffer);

    while (true) {
        std::cout << "Type message:\n";
        std::cin >> input;
        const char* msg = input.c_str();
        // Отправляем сообщение на сервер
        send(sock, msg, strlen(msg), 0);
        // Получаем ответ от сервера
        valread = read(sock, buffer, 1024);
        if (std::string(buffer) == "Bye too") {
            close_connection(sock);
            break;
        }
        printf("%s\n",buffer );

        memset(buffer, 0, sizeof buffer);
    }
    return 0;
}