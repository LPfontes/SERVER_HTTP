#ifndef TcpServerSocket_hpp
#define TcpServerSocket_hpp

#include "Socket.hpp"
#include "ThreadPool.cpp"
namespace PSO
{
    class TcpServerSocket : public Socket
    {
        private:
    ThreadPool pool;  // Adicione esta linha como membro da classe
    public:
        TcpServerSocket(int domain, int service, int protocol, int port, u_long interface, int num_threads);
        int connect(int sock, struct sockaddr_in address) override;

        void start_listening(int backlog);
        void accept_connection();
        void handle_client(int client_socket);
    };
}

#endif
