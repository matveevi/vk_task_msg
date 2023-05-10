#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "data_base/db_tools.cpp"
#include <string>

#include <random>
#include <sstream>

// Закрытие сокета
void close_connection(int socket) {
    shutdown(socket, SHUT_RDWR);
    close(socket);
}

const int PORT = 8888;

int main(int argc, char const *argv[]) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    const char *hello = "Hello from server";
    const char *login = "Enter login: ";
    const char *password = "Enter password: ";
    const char *success = "Reg/Login successful";
    const char *failure = "Login failed";
    const char *msg_done = "Message get";
    const char *query = "Enter query: login or reg";
    const char *close_socket = "Bye too";
    int id_user = 1;

    // Создаем дескриптор сокета
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Устанавливаем опцию повторного использования адреса
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Устанавливаем параметры структуры sockaddr_in
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Привязываем сокет к адресу и порту
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    // if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Ожидаем входящих соединений
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Принимаем входящее соединение
    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        // Отправляем сообщение клиенту
        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");

        // Авторизация клиента
        printf("%s\n", query);
        send(new_socket, query, strlen(query), 0);
        std::cout << "Wait login or register\n";
        int valread = read(new_socket, buffer, 1024);
        printf("%s\n", buffer);

        std::string check_to_login(buffer);
        check_to_login.erase(remove(check_to_login.begin(), check_to_login.end(), '\n'), check_to_login.end());
        memset(buffer, 0, sizeof buffer);

        printf("%s\n", login);
        send(new_socket, login, strlen(login), 0);
        std::cout << "Wait login\n";
        valread = read(new_socket, buffer, 1024);
        printf("%s\n", buffer);

        std::string login_str(buffer);
        login_str.erase(remove(login_str.begin(), login_str.end(), '\n'), login_str.end());
        memset(buffer, 0, sizeof buffer);

        send(new_socket, password, strlen(password), 0);
        std::cout << "Wait password\n";
        valread = read(new_socket, buffer, 1024);
        printf("%s\n", buffer);
        std::string password_str(buffer);
        password_str.erase(remove(password_str.begin(), password_str.end(), '\n'), password_str.end());
        memset(buffer, 0, sizeof(buffer));

        Person new_person = {id_user++, "name", login_str, password_str};
        if (check_to_login == "login") {
            // User want to login
            if (check_user_in_db(new_person)) {
                send(new_socket, success, strlen(success), 0);
                printf("%s\n", success);
            } else {
                send(new_socket, failure, strlen(failure), 0);
                printf("%s\n", failure);
                continue;
            }
        } else if (check_to_login == "reg") {
            // User want to register
            insert_into_db(new_person);
            send(new_socket, success, strlen(success), 0);
        } else {
            // Undefined symbol
            std::cout << "Enter 'reg' or 'login' \n";
        }

        // Получаем сообщение от клиента
        while (true) {
            valread = read(new_socket, buffer, 1024);
            printf("%s\n", buffer);
            std::cout << buffer;

            if (std::string(buffer) == "close"){
                send(new_socket, close_socket, strlen(close_socket), 0);
                memset(buffer, 0, sizeof(buffer));
                close_connection(new_socket);
                break;
            }

            send(new_socket, msg_done, strlen(msg_done), 0);

            memset(buffer, 0, sizeof(buffer));
        }
    }


    return 0;
}
