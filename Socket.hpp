#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>

namespace PSO
{
    // Classe base para abstrair operações de socket
    class Socket
    {
    protected:
        struct sockaddr_in address; 
        int sock;                   
        int connection;             

    public:
        // Construtor da classe Socket
        // - domain: AF_INET (IPv4) ou outros
        // - service: SOCK_STREAM (TCP) ou SOCK_DGRAM (UDP)
        // - protocol: geralmente 0 para IP padrão
        // - port: porta na qual o socket irá operar
        // - interface: IP (normalmente INADDR_ANY para aceitar todas as interfaces locais)
        Socket(int domain, int service, int protocol, int port, u_long interface);

        // Método virtual puro que deve ser implementado pelas classes filhas
        // Usado para conectar ou bindar o socket
        virtual int connect(int sock, struct sockaddr_in address) = 0;

        // Testa se a conexão/bind/listen/accept foi realizada com sucesso
        void test_connection(int sock, int connection);

        // Getters para acessar membros privados/protegidos

        // Retorna a estrutura de endereço
        struct sockaddr_in get_address();

        // Retorna o descritor do socket
        int get_sock();

        // Retorna o status da conexão
        int get_connection();
    };
}

