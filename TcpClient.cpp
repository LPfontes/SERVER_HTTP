#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Erro ao criar socket");
        return 1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);

    // Conecta ao localhost (127.0.0.1)
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0)
    {
        perror("Endereço inválido");
        return 1;
    }

    if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
    {
        perror("Erro na conexão com o servidor");
        return 1;
    }

    std::cout << "Conectado ao servidor!\n";

    char buffer[1024] = {0};
    int valread = read(sock, buffer, sizeof(buffer));
    std::cout << "Mensagem recebida do servidor: " << buffer << std::endl;

    close(sock);
    return 0;
}
