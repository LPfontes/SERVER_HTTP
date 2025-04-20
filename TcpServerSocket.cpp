#include "TcpServerSocket.hpp"

using namespace PSO;

TcpServerSocket::TcpServerSocket(int domain, int service, int protocol, int port, u_long interface)
    : Socket(domain, service, protocol, port, interface) {}

int TcpServerSocket::connect(int sock, struct sockaddr_in address)
{
    // Usa bind para deixar o SO escolher a porta, se address.sin_port == 0
    int connection = bind(sock, (struct sockaddr*)&address, sizeof(address));

    // Captura a porta real que foi associada
    socklen_t addrlen = sizeof(address);
    getsockname(sock, (struct sockaddr *)&address, &addrlen); // read binding
    int local_port = ntohs(address.sin_port);  
    test_connection(sock, connection);
    printf("porta %d\n",local_port);
    return connection;
}

void TcpServerSocket::start_listening(int backlog)
{
    connection = listen(sock, backlog);
    test_connection(sock, connection);
    printf("Servidor ouvindo por conexões...\n");

}

void TcpServerSocket::accept_connection()
{
    while (true)
    {
        int addrlen = sizeof(address);
        int client_socket = accept(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        test_connection(sock, client_socket);
        printf("Conexão aceita com sucesso!\n");

        // Lê a requisição
        char buffer[3000] = {0};
        read(client_socket, buffer, sizeof(buffer));
        printf("Requisição recebida:\n%s\n", buffer);

        // Página HTML de resposta
        const char* html = "<html><body><h1>Servidor HTTP em C++!</h1></body></html>";

        // Cabeçalho HTTP
        std::string response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + std::to_string(strlen(html)) + "\r\n"
            "\r\n" +
            html;

        // Envia a resposta
        send(client_socket, response.c_str(), response.length(), 0);
        close(client_socket);
    }
}

