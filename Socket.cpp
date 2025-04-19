#include "Socket.hpp"

using namespace PSO;

Socket::Socket(int domain, int service, int protocol, int port, u_long interface)
{
    sock = socket(domain, service, protocol);
    address.sin_family = domain;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(interface);
}

void Socket::test_connection(int sock, int connection)
{
    if (connection < 0)
    {
        perror("Erro na conexão com a rede...");
        exit(EXIT_FAILURE);
    }
}

// Getters
struct sockaddr_in Socket::get_address() { return address; }
int Socket::get_sock() { return sock; }
int Socket::get_connection() { return connection; }
