#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

int main(int argc, char* argv[]) {
    // Адрес сервера по умолчанию
    const char* server_ip = "172.16.40.1"; 
    int port = 13; // Стандартный порт Daytime (UDP)

    if (argc > 1) {
        server_ip = argv[1];
    }

    // 1. Создание сокета UDP
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("Ошибка создания сокета");
        return 1;
    }

    // 2. Подготовка адресной структуры сервера
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    // Преобразование IP-адреса
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Неверный адрес сервера");
        close(sock);
        return 1;
    }

    // 3. Отправка запроса
    const char* msg = "hello";
    ssize_t sent_bytes = sendto(sock, msg, strlen(msg), 0, 
                               (const struct sockaddr*)&server_addr, sizeof(server_addr));
    
    if (sent_bytes == -1) {
        perror("Ошибка отправки");
        close(sock);
        return 1;
    }

    // 4. Получение ответа
    char buffer[1024];
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);

    ssize_t recv_bytes = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, 
                                 (struct sockaddr*)&from_addr, &from_len);

    if (recv_bytes == -1) {
        perror("Ошибка получения данных (сервер недоступен)");
    } else {
        buffer[recv_bytes] = '\0';
        cout << "Ответ сервера: " << buffer << endl;
    }

    close(sock);
    return 0;
}
