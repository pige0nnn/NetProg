#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

int main() {
    int port = 7777;
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_sock == -1) {
        perror("Ошибка создания сокета");
        return 1;
    }

    // Разрешаем повторное использование адреса
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Ошибка bind");
        close(server_sock);
        return 1;
    }

    // Ожидание соединений
    if (listen(server_sock, 10) == -1) {
        perror("Ошибка listen");
        close(server_sock);
        return 1;
    }

    cout << "Echo сервер запущен на порту " << port << "..." << endl;

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        // Принятие соединения
        int work_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (work_sock == -1) {
            perror("Ошибка accept");
            continue;
        }

        cout << "Клиент подключен: " << inet_ntoa(client_addr.sin_addr) << endl;

        // чтение и отправка обратно)
        char buffer[1024];
        ssize_t bytes_read;
        
        // Читаем данные от клиента и отправляем обратно
        while ((bytes_read = recv(work_sock, buffer, sizeof(buffer), 0)) > 0) {
            send(work_sock, buffer, bytes_read, 0);
        }

        close(work_sock);
        cout << "Клиент отключен." << endl;
    }

    close(server_sock);
    return 0;
}
